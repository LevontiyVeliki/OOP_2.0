#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <map>

using namespace std;

struct Config { // тут мы вводим все скажем так характеристики для кода, ну то есть высота, ширина и так далее
    uint16_t width = 0;
    uint16_t height = 0;
    string input_file;
    string output_dir;
    uint64_t max_iter = 0;
    uint64_t freq = 0;
};

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
    {255, 255, 255}, // 0 - это белый, ну то есть как бы фон
    {0, 255, 0},     // 1 - зелёный
    {128, 0, 128},   // 2 - фиолетовый
    {255, 255, 0},   // 3 - жёлтый
    {0, 0, 0}        // >3 - чёрный
};

void save_bmp(const string& filename, const vector<vector<uint64_t>>& grid) {
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

void load_input(const string& filename, vector<vector<uint64_t>>& grid) {// тут мы считываем файл и берем из него информацию
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int x, y;
        uint64_t count;
        iss >> x;
        iss.ignore(); // пропустить таб, таб потому что формат этого текстового документа подразумевает, что информация считывается построчно и разделяется табами
        iss >> y;
        iss.ignore();
        iss >> count;
        if (y < grid.size() && x < grid[0].size())
            grid[y][x] += count;
    }
}

bool iterate(vector<vector<uint64_t>>& grid) { // тут мы смотрим на функцию, она по сути отвечает за каждую итерацию и вощвращает да или нет, то есть погменялась ли картинка или нет.
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

Config parse_args(int argc, char* argv[]) {
    Config cfg;
    map<string, string> args;

    for (int i = 1; i < argc - 1; i += 2) {
        args[argv[i]] = argv[i + 1];
    }

    if (args.count("-l")) cfg.height = stoi(args["-l"]);
    if (args.count("--length")) cfg.height = stoi(args["--length"]);

    if (args.count("-w")) cfg.width = stoi(args["-w"]);
    if (args.count("--width")) cfg.width = stoi(args["--width"]);

    if (args.count("-i")) cfg.input_file = args["-i"];
    if (args.count("--input")) cfg.input_file = args["--input"];

    if (args.count("-o")) cfg.output_dir = args["-o"];
    if (args.count("--output")) cfg.output_dir = args["--output"];

    if (args.count("-m")) cfg.max_iter = stoull(args["-m"]);
    if (args.count("--max-iter")) cfg.max_iter = stoull(args["--max-iter"]);

    if (args.count("-f")) cfg.freq = stoull(args["-f"]);
    if (args.count("--freq")) cfg.freq = stoull(args["--freq"]);

    return cfg;
}

int main(int argc, char* argv[]) {
    if (argc < 9) {
        cout << "Usage: ./convbmp -l <height> -w <width> -i <input.tsv> -o <output_dir> -m <max_iter> -f <freq>\n";
        return 1;
    }

    Config cfg = parse_args(argc, argv);
    filesystem::create_directories(cfg.output_dir);

    vector<vector<uint64_t>> grid(cfg.height, vector<uint64_t>(cfg.width, 0));
    load_input(cfg.input_file, grid);

    for (uint64_t iter = 0; iter <= cfg.max_iter; ++iter) {
        if (cfg.freq != 0 && iter % cfg.freq == 0) {
            string filename = cfg.output_dir + "/state_" + to_string(iter) + ".bmp";
            save_bmp(filename, grid);
        }

        if (!iterate(grid)) {
            cout << "Stable at iteration: " << iter << endl;
            break;
        }
    }

    if (cfg.freq == 0) {
        string filename = cfg.output_dir + "/final.bmp";
        save_bmp(filename, grid);
    }

    return 0;
}
