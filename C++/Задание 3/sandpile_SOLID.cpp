#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <map>
#include <memory>

using namespace std;

// Интерфейс для конфигурации
class IConfig {
public:
    virtual uint16_t getWidth() const = 0;
    virtual uint16_t getHeight() const = 0;
    virtual string getInputFile() const = 0;
    virtual string getOutputDir() const = 0;
    virtual uint64_t getMaxIter() const = 0;
    virtual uint64_t getFreq() const = 0;
    virtual ~IConfig() = default;
};

// Конкретная реализация конфигурации
class Config : public IConfig {
    uint16_t width = 0;
    uint16_t height = 0;
    string input_file;
    string output_dir;
    uint64_t max_iter = 0;
    uint64_t freq = 0;

public:
    Config(uint16_t w, uint16_t h, string in, string out, uint64_t max, uint64_t f)
        : width(w), height(h), input_file(move(in)), output_dir(move(out)), max_iter(max), freq(f) {
    }

    uint16_t getWidth() const override { return width; }
    uint16_t getHeight() const override { return height; }
    string getInputFile() const override { return input_file; }
    string getOutputDir() const override { return output_dir; }
    uint64_t getMaxIter() const override { return max_iter; }
    uint64_t getFreq() const override { return freq; }
};

// Интерфейс для работы с BMP
class IBMPHandler {
public:
    virtual void save(const string& filename, const vector<vector<uint64_t>>& grid) = 0;
    virtual ~IBMPHandler() = default;
};

// Конкретная реализация для работы с BMP
class BMPHandler : public IBMPHandler {
#pragma pack(push, 1)
    struct BMPHeader {
        uint16_t file_type = 0x4D42;
        uint32_t file_size;
        uint32_t reserved = 0;
        uint32_t offset_data = 54;
        uint32_t size = 40;
        int32_t width;
        int32_t height;
        uint16_t planes = 1;
        uint16_t bit_count = 24;
        uint32_t compression = 0;
        uint32_t size_image = 0;
        int32_t x_pixels_per_meter = 1000;
        int32_t y_pixels_per_meter = 1000;
        uint32_t colors_used = 0;
        uint32_t colors_important = 0;
    };
#pragma pack(pop)

    uint8_t color_map[5][3] = {
        {255, 255, 255}, // 0 - white
        {0, 255, 0},     // 1 - green
        {128, 0, 128},   // 2 - purple
        {255, 255, 0},   // 3 - yellow
        {0, 0, 0}        // >3 - black
    };

public:
    void save(const string& filename, const vector<vector<uint64_t>>& grid) override {
        int height = grid.size();
        int width = grid[0].size();
        int row_stride = (width * 3 + 3) & ~3;
        int filesize = 54 + row_stride * height;

        BMPHeader header;
        header.file_size = filesize;
        header.width = width;
        header.height = height;
        header.size_image = row_stride * height;

        ofstream file(filename, ios::binary);
        file.write(reinterpret_cast<char*>(&header), sizeof(header));

        vector<uint8_t> row(row_stride);

        for (int y = height - 1; y >= 0; --y) {
            memset(row.data(), 0, row_stride);
            for (int x = 0; x < width; ++x) {
                uint64_t val = grid[y][x];
                int idx = val > 3 ? 4 : static_cast<int>(val);
                row[x * 3 + 0] = color_map[idx][2]; // B
                row[x * 3 + 1] = color_map[idx][1]; // G
                row[x * 3 + 2] = color_map[idx][0]; // R
            }
            file.write(reinterpret_cast<char*>(row.data()), row_stride);
        }
    }
};

// Интерфейс для загрузки данных
class IDataLoader {
public:
    virtual void load(const string& filename, vector<vector<uint64_t>>& grid) = 0;
    virtual ~IDataLoader() = default;
};

// Конкретная реализация загрузки данных
class TSVDataLoader : public IDataLoader {
public:
    void load(const string& filename, vector<vector<uint64_t>>& grid) override {
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            int x, y;
            uint64_t count;
            iss >> x;
            iss.ignore(); // skip tab
            iss >> y;
            iss.ignore();
            iss >> count;
            if (y < grid.size() && x < grid[0].size())
                grid[y][x] += count;
        }
    }
};

// Интерфейс для итератора песочной кучи
class ISandpileIterator {
public:
    virtual bool iterate(vector<vector<uint64_t>>& grid) = 0;
    virtual ~ISandpileIterator() = default;
};

// Конкретная реализация итератора
class SandpileIterator : public ISandpileIterator {
public:
    bool iterate(vector<vector<uint64_t>>& grid) override {
        int h = grid.size();
        int w = grid[0].size();
        vector<vector<uint64_t>> next = grid;
        bool changed = false;

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (grid[y][x] >= 4) {
                    uint64_t fall = grid[y][x] / 4;
                    next[y][x] -= fall * 4;

                    if (y > 0) next[y - 1][x] += fall;
                    if (y < h - 1) next[y + 1][x] += fall;
                    if (x > 0) next[y][x - 1] += fall;
                    if (x < w - 1) next[y][x + 1] += fall;

                    changed = true;
                }
            }
        }

        grid = next;
        return changed;
    }
};

// Основной класс приложения
class SandpileSimulator {
    unique_ptr<IConfig> config;
    unique_ptr<IBMPHandler> bmpHandler;
    unique_ptr<IDataLoader> dataLoader;
    unique_ptr<ISandpileIterator> iterator;

public:
    SandpileSimulator(unique_ptr<IConfig> cfg,
        unique_ptr<IBMPHandler> bmp,
        unique_ptr<IDataLoader> loader,
        unique_ptr<ISandpileIterator> iter)
        : config(move(cfg)), bmpHandler(move(bmp)), dataLoader(move(loader)), iterator(move(iter)) {
    }

    void run() {
        filesystem::create_directories(config->getOutputDir());

        vector<vector<uint64_t>> grid(config->getHeight(), vector<uint64_t>(config->getWidth(), 0));
        dataLoader->load(config->getInputFile(), grid);

        for (uint64_t iter = 0; iter <= config->getMaxIter(); ++iter) {
            if (config->getFreq() != 0 && iter % config->getFreq() == 0) {
                string filename = config->getOutputDir() + "/state_" + to_string(iter) + ".bmp";
                bmpHandler->save(filename, grid);
            }

            if (!iterator->iterate(grid)) {
                cout << "Stable at iteration: " << iter << endl;
                break;
            }
        }

        if (config->getFreq() == 0) {
            string filename = config->getOutputDir() + "/final.bmp";
            bmpHandler->save(filename, grid);
        }
    }
};

// Фабрика для создания конфигурации
unique_ptr<IConfig> createConfig(int argc, char* argv[]) {
    if (argc < 9) {
        cout << "Usage: ./convbmp -l <height> -w <width> -i <input.tsv> -o <output_dir> -m <max_iter> -f <freq>\n";
        exit(1);
    }

    map<string, string> args;
    for (int i = 1; i < argc - 1; i += 2) {
        args[argv[i]] = argv[i + 1];
    }

    auto getArg = [&args](const string& shortKey, const string& longKey) {
        if (args.count(shortKey)) return args[shortKey];
        if (args.count(longKey)) return args[longKey];
        return string();
        };

    uint16_t height = stoi(getArg("-l", "--length"));
    uint16_t width = stoi(getArg("-w", "--width"));
    string input = getArg("-i", "--input");
    string output = getArg("-o", "--output");
    uint64_t max_iter = stoull(getArg("-m", "--max-iter"));
    uint64_t freq = stoull(getArg("-f", "--freq"));

    return make_unique<Config>(width, height, input, output, max_iter, freq);
}

int main(int argc, char* argv[]) {
    auto config = createConfig(argc, argv);
    auto bmpHandler = make_unique<BMPHandler>();
    auto dataLoader = make_unique<TSVDataLoader>();
    auto iterator = make_unique<SandpileIterator>();

    SandpileSimulator simulator(move(config), move(bmpHandler), move(dataLoader), move(iterator));
    simulator.run();

    return 0;
}