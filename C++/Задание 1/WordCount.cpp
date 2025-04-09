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

// Функция для парсинга аргументов командной строки
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

// Функция для подсчета статистики файла
FileStats countFileStats(const wstring& filename) {
    // Открываем файл в бинарном режиме
    ifstream file(filename, ios::binary);
    if (!file) {
        wcerr << L"Ошибка: не удалось открыть файл " << filename << endl;
        exit(1);
    }

    FileStats stats;
    stats.filename = filename;

    // Получаем размер файла в байтах
    file.seekg(0, ios::end);
    stats.bytes = file.tellg();
    file.seekg(0, ios::beg);

    // Читаем содержимое файла в строку
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    // Счетчики
    bool inWord = false;
    stats.lines = 0;
    stats.words = 0;
    stats.chars = 0;

    // Конвертер из UTF-8 в UTF-32
    wstring_convert<codecvt_utf8<char32_t>, char32_t> utf32conv;
    try {
        u32string utf32 = utf32conv.from_bytes(content);

        for (char32_t ch : utf32) {
            stats.chars++;

            if (ch == U'\n') {
                stats.lines++;
            }

            if (iswspace(static_cast<wint_t>(ch))) {
                if (inWord) {
                    stats.words++;
                    inWord = false;
                }
            }
            else {
                inWord = true;
            }
        }
    }
    catch (const exception& e) {
        wcerr << L"Ошибка при обработке файла " << filename << L": " << e.what() << endl;
        exit(1);
    }

    // Учитываем последнее слово, если файл не заканчивается пробелом
    if (inWord) {
        stats.words++;
    }

    // Если файл не заканчивается на \n, учитываем последнюю строку
    if (!content.empty() && content.back() != '\n') {
        stats.lines++;
    }

    return stats;
}

// Функция для вывода результатов
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
        FileStats stats = countFileStats(filename);
        printStats(stats, options);
    }

    return 0;
}