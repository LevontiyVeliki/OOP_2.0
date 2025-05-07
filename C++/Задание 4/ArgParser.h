#pragma once
#include <string>
#include <vector>

using namespace std;

namespace ArgumentParser {

    class ArgParser {
    private:

        // Структура для хранения информации об аргументе командной строки
        struct Argument {
            string long_name;        // Длинное имя аргумента (например, "--output")
            char short_name;         // Короткое имя аргумента (например, '-o')
            string description;      // Описание аргумента для вывода справки
            string* value_str;       // Ссылка на переменную для хранения строкового значения
            vector<int>* value_int;  // Ссылка на контейнер для хранения числовых значений
            bool* value_bool;        // Ссылка на флаг (true/false)
            bool is_set;             // Был ли аргумент передан в командной строке
            bool is_flag;            // Является ли аргумент флагом (без значения)
            bool is_positional;      // Является ли аргумент позиционным (не именованным)
            int min_values;          // Минимальное количество требуемых значений
            string default_value_str; // Значение по умолчанию для строк
            bool default_value_bool;  // Значение по умолчанию для флагов
            bool is_help;            // Является ли аргумент запросом справки

            // Инициализация всех полей безопасными значениями по умолчанию
            Argument() : value_str(nullptr),
                value_int(nullptr),
                value_bool(nullptr),
                is_set(false),
                is_flag(false),
                is_positional(false),
                min_values(0),
                default_value_bool(false),
                default_value_str(""),
                is_help(false) {
            }
        };

        string program_name_;       // Имя программы для вывода в справке
        vector<Argument> arguments_; // Коллекция зарегистрированных аргументов
        bool help_requested_ = false; // Флаг запроса вывода справки
        string help_description_;    // Текст справки по использованию программы

        // Поиск аргумента по короткому имени
        Argument* FindArgumentByShortKey(char short_key) {
            for (auto& argument : arguments_) {
                if (argument.short_name == short_key) {
                    return &argument;
                }
            }
            return nullptr;  // Если аргумент не найден
        }

    public:
        // Конструкторы с разными вариантами инициализации
        ArgParser(string name);
        ArgParser(char short_name, string name);
        ArgParser();
        ~ArgParser();

        // Методы для регистрации строковых аргументов
        ArgParser& AddStringArgument(const string& long_name);
        ArgParser& AddStringArgument(char short_name, const string& long_name);
        ArgParser& AddStringArgument(char short_name, const string& long_name, const string& description);

        // Методы для регистрации числовых аргументов
        ArgParser& AddIntArgument(const string& long_name);
        ArgParser& AddIntArgument(char short_name, const string& long_name);
        ArgParser& AddIntArgument(const string& long_name, const string& help_description_);

        // Методы для регистрации флагов (boolean аргументов)
        ArgParser& AddFlag(char short_name, const string& long_name);
        ArgParser& AddFlag(char short_name_second, const string& long_name, const string& description);
        ArgParser& AddFlag(const string long_name_third, const string& long_name_);

        // Основной метод для разбора аргументов командной строки
        bool Parse(const vector<string>& args);

        // Установка значений по умолчанию для аргументов
        ArgParser& Default(const char* default_value);
        ArgParser& Default(bool default_flag);

        // Настройка аргумента для приема нескольких значений
        ArgParser& MultiValue(size_t min_values = 1);

        // Пометка аргумента как позиционного (расположенного в определенном порядке)
        ArgParser& Positional();

        // Привязка аргументов к внешним переменным для хранения значений
        ArgParser& StoreValue(string& value);
        ArgParser& StoreValues(vector<int>& values);
        ArgParser& StoreValue(bool& flag);

        // Методы для получения значений аргументов
        string GetStringValue(const string& arg_name, const string& default_value = "") const;
        int GetIntValue(const string& arg_name, int default_value = 0) const;
        bool GetFlag(const string& arg_name) const;

        // Настройка и проверка аргумента справки
        ArgParser& AddHelp(char short_name, const string& long_name, const string& description);
        bool Help() const;
        string HelpDescription() const;

        // Вспомогательные методы для обработки аргументов
        bool CheckArgumentsAfterParsing();
        bool ProcessPositionalArgument(const string& arg, size_t& positional_arg_index);
        bool ProcessShortArgument(const string& arg);
        bool ProcessLongArgument(const string& arg);
    };

} // namespace ArgumentParser