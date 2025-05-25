#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <clocale>
#include <io.h>
#include <fcntl.h>
#include <codecvt>
#include <locale>

using namespace std;

// Структура для хранения опций
struct Options {
    bool lines = false;
    bool words = false;
    bool bytes = false;
    bool chars = false;
    bool all = true;
};

// Структура для хранения результатов подсчета
struct FileStats {
    wstring filename;
    int lines = 0;
    int words = 0;
    int bytes = 0;
    int chars = 0;
};

// Класс для чтения файла
class FileReader {
public:
    static string readFile(const wstring& filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            wcerr << L"Ошибка: не удалось открыть файл " << filename << endl;
            exit(1);
        }
        return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    }

    static int getFileSize(const wstring& filename) {
        ifstream file(filename, ios::binary | ios::ate);
        if (!file) {
            wcerr << L"Ошибка: не удалось открыть файл " << filename << endl;
            exit(1);
        }
        return file.tellg();
    }
};

// Класс для подсчета статистики
class StatsCounter {
public:
    static FileStats countStats(const wstring& filename, const string& content) {
        FileStats stats;
        stats.filename = filename;
        stats.bytes = FileReader::getFileSize(filename);
        stats.lines = countLines(content);
        stats.words = countWords(content);
        stats.chars = countChars(content);
        return stats;
    }

private:
    static int countLines(const string& content) {
        int lines = 0;
        for (char ch : content) {
            if (ch == '\n') lines++;
        }
        if (!content.empty() && content.back() != '\n') lines++;
        return lines;
    }

    static int countWords(const string& content) {
        bool inWord = false;
        int words = 0;
        for (char ch : content) {
            if (isspace(ch)) {
                if (inWord) {
                    words++;
                    inWord = false;
                }
            }
            else {
                inWord = true;
            }
        }
        if (inWord) words++;
        return words;
    }

    static int countChars(const string& content) {
        wstring_convert<codecvt_utf8<char32_t>, char32_t> utf32conv;
        try {
            u32string utf32 = utf32conv.from_bytes(content);
            return utf32.size();
        }
        catch (const exception& e) {
            wcerr << L"Ошибка при подсчете символов: " << e.what() << endl;
            exit(1);
        }
    }
};

// Остальной код остается без изменений...
void parseArguments(int argc, wchar_t* argv[], Options& options, vector<wstring>& filenames) {

    for (int i = 1; i < argc; ++i) {
        wstring arg = argv[i];

        if (arg == L"--lines" || arg == L"-l") {
            options.lines = true;
            options.all = false;
        }
        else if (arg == L"--words" || arg == L"-w") {
            options.words = true;
            options.all = false;
        }
        else if (arg == L"--bytes" || arg == L"-c") {
            options.bytes = true;
            options.all = false;
        }
        else if (arg == L"--chars" || arg == L"-m") {
            options.chars = true;
            options.all = false;
        }
        else if (arg[0] == L'-') {
            for (size_t j = 1; j < arg.size(); ++j) {
                switch (arg[j]) {
                case L'l': options.lines = true; options.all = false; break;
                case L'w': options.words = true; options.all = false; break;
                case L'c': options.bytes = true; options.all = false; break;
                case L'm': options.chars = true; options.all = false; break;
                default:
                    wcerr << L"Неизвестная опция: " << arg[j] << endl;
                    exit(1);
                }
            }
        }
        else {
            filenames.push_back(arg);
        }
    }
}

void printStats(const FileStats& stats, const Options& options) {

    if (options.all || options.lines) {
        wcout << stats.lines << L" ";
    }
    if (options.all || options.words) {
        wcout << stats.words << L" ";
    }
    if (options.all || options.bytes) {
        wcout << stats.bytes << L" ";
    }
    if (options.chars) {
        wcout << stats.chars << L" ";
    }
    wcout << stats.filename << endl;
}

int wmain(int argc, wchar_t* argv[]) {

    // Настройка консоли для вывода Unicode
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);

    // Установка локали
    setlocale(LC_ALL, "Russian");

    if (argc < 2) {
        wcerr << L"Использование: " << argv[0] << L" [OPTION]... [FILE]..." << endl;
        wcerr << L"Опции:" << endl;
        wcerr << L"  -l, --lines    вывод количества строк" << endl;
        wcerr << L"  -w, --words    вывод количества слов" << endl;
        wcerr << L"  -c, --bytes    вывод размера файла в байтах" << endl;
        wcerr << L"  -m, --chars    вывод количества символов" << endl;
        return 1;
    }

    Options options;
    vector<wstring> filenames;

    parseArguments(argc, argv, options, filenames);

    if (filenames.empty()) {
        wcerr << L"Ошибка: не указаны файлы для обработки" << endl;
        return 1;
    }

    for (const auto& filename : filenames) {
        string content = FileReader::readFile(filename);
        FileStats stats = StatsCounter::countStats(filename, content);
        printStats(stats, options);
    }

    return 0;
}