#include <stdexcept>
#include "ArgParser.h"
#include <iostream>
#include <string>
#include <sstream>

namespace ArgumentParser {

ArgParser::ArgParser(std::string program_name)
    : program_name_(program_name), help_requested_(false) {
}

ArgParser::ArgParser(char short_name, const std::string name)
    : program_name_(name), help_requested_(false) {
}

ArgParser::ArgParser()
    : help_requested_(false) {
}

ArgParser::~ArgParser() {
}

ArgParser& ArgParser::AddStringArgument(const std::string& long_name) {
  Argument arg;
  arg.long_name = long_name;
  arg.is_flag = false;
  arg.is_set = false;
  arg.value_str = new std::string;
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::AddStringArgument(char short_name, const std::string& long_name) {
  Argument arg;
  arg.long_name = long_name;
  arg.short_name = short_name;
  arg.is_flag = false;
  arg.is_set = false;
  arg.value_str = new std::string;
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::AddStringArgument(char short_name, const std::string& long_name, const std::string& description) {
  Argument new_argument;
  new_argument.short_name = short_name;
  new_argument.long_name = long_name;
  new_argument.description = description;
  new_argument.is_flag = false;

  arguments_.push_back(new_argument);
  return *this;
}

ArgParser& ArgParser::AddIntArgument(const std::string& long_name) {
  Argument arg;
  arg.long_name = long_name;
  arg.short_name = '\0';
  arg.is_flag = false;
  arg.is_set = false;
  arg.value_int = new std::vector<int>();
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::AddIntArgument(char short_name, const std::string& long_name) {
  Argument arg;
  arg.long_name = long_name;
  arg.short_name = short_name;
  arg.is_flag = false;
  arg.is_set = false;
  arg.value_int = new std::vector<int>();
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::AddIntArgument(const std::string& long_name, const std::string& help_description_) {
  Argument arg;
  arg.long_name = long_name;
  arg.short_name = '\0';
  arg.is_flag = false;
  arg.is_set = false;
  arg.value_int = new std::vector<int>();
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::AddFlag(char short_name, const std::string& long_name) {
  Argument arg;
  arg.long_name = long_name;
  arg.short_name = short_name;
  arg.is_flag = true;
  arg.is_set = false;
  arg.value_bool = new bool(false);
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::AddFlag(char short_name, const std::string& long_name, const std::string& description) {
  Argument arg;
  arg.long_name = long_name;
  arg.short_name = short_name;
  arg.is_flag = true;
  arg.is_set = false;
  arg.value_bool = new bool(false);
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::AddFlag(const std::string long_name, const std::string& long_name_) {
  Argument arg;
  arg.long_name = long_name;
  arg.short_name = '\0';
  arg.is_flag = true;
  arg.is_set = false;
  arg.value_bool = new bool(false);
  arguments_.push_back(arg);

  return *this;
}

ArgParser& ArgParser::StoreValue(std::string& value) {
  if (!arguments_.empty()) {
    auto& arg = arguments_.back();
    if (!arg.is_flag) {
      arg.value_str = &value;
    } else {
      throw std::logic_error("Attempt to store string value in flag argument.");
    }
    return *this;
  } else {
    throw std::logic_error("No argument to store value for.");
  }
}

ArgParser& ArgParser::StoreValue(bool& value) {
  if (!arguments_.empty()) {
    auto& arg = arguments_.back();
    if (arg.is_flag) {
      arg.value_bool = &value;
    } else {
      throw std::logic_error("Attempt to store bool value in non-flag argument.");
    }
    return *this;
  } else {
    throw std::logic_error("No argument to store value for.");
  }
}

ArgParser& ArgParser::StoreValues(std::vector<int>& values) {
  if (!arguments_.empty()) {
    auto& arg = arguments_.back();
    if (!arg.is_flag) {
      arg.value_int = &values;
    } else {
      throw std::logic_error("Attempt to store int values in flag argument.");
    }
    return *this;
  } else {
    throw std::logic_error("No argument to store values for.");
  }
}

ArgParser& ArgParser::MultiValue(size_t min_values) {
  if (!arguments_.empty()) {
    arguments_.back().min_values = min_values;
    return *this;
  } else {
    throw std::logic_error("No argument to set multi-value for.");
  }
}

ArgParser& ArgParser::Positional() {
  if (!arguments_.empty()) {
    arguments_.back().is_positional = true;
    return *this;
  } else {
    throw std::logic_error("No argument to set as positional.");
  }
}

ArgParser& ArgParser::Default(bool default_flag) {
  if (!arguments_.empty() && arguments_.back().is_flag) {
    arguments_.back().default_value_bool = default_flag;
    return *this;
  } else {
    throw std::logic_error("privet");
  }
}

ArgParser& ArgParser::Default(const char* default_value) {
  if (!arguments_.empty()) {
    auto& lastArg = arguments_.back();
    if (!lastArg.is_flag) {
      lastArg.default_value_str = default_value;
      if (!lastArg.is_set && lastArg.value_str) {
        *lastArg.value_str = default_value;
      }
      return *this;
    }
  }
  throw std::logic_error("No string argument to set default value for.");
}

std::string ArgParser::GetStringValue(const std::string& arg_name, const std::string& default_value) const {
  for (const auto& arg : arguments_) {
    if (arg.long_name == arg_name && arg.value_str) {
      return *arg.value_str;
    }
  }
  return default_value;
}

int ArgParser::GetIntValue(const std::string& arg_name, int default_value) const {
  for (const auto& arg : arguments_) {
    if (arg.long_name == arg_name && arg.value_int) {
      if (!arg.value_int->empty()) {
        return arg.value_int->front(); // Возвращает первое значение вектора
      } else {
        return default_value; // Возвращает значение по умолчанию, если вектор пуст
      }
    }
  }
  return default_value; // Возвращает значение по умолчанию, если аргумент не найден
}

bool ArgParser::GetFlag(const std::string& arg_name) const {
  for (const auto& arg : arguments_) {
    if (arg.long_name == arg_name && arg.is_flag) {
      if (arg.value_bool) {
        return *(arg.value_bool); // Возвращаем значение по указателю
      }
      return arg.default_value_bool; // Возвращаем значение по умолчанию для флага
    }
  }
  return false; // Возвращаем false, если флаг не найден
}

ArgParser& ArgParser::AddHelp(char short_name, const std::string& long_name, const std::string& description) {
  Argument help_argument;
  help_argument.long_name = long_name;
  help_argument.short_name = short_name;
  help_argument.description = description;
  help_argument.is_flag = true;
  help_argument.is_help = true; // Устанавливаем флаг is_help в true для аргумента справки
  help_argument.value_bool = &help_requested_; // Ссылка на флаг, показывающий была ли запрошена справка
  arguments_.push_back(help_argument);

  return *this;
}

bool ArgParser::Help() const {
  return help_requested_;
}

std::string ArgParser::HelpDescription() const {
  std::ostringstream help_stream;
  help_stream << program_name_ << "\n";
  if (!help_description_.empty()) {
    help_stream << help_description_ << "\n\n";
  }

  for (const auto& arg : arguments_) {
    // Для аргументов, имеющих короткое и длинное имя
    if (arg.short_name != '\0') {
      help_stream << "-" << arg.short_name << ", ";
    }
    if (!arg.long_name.empty()) {
      help_stream << "--" << arg.long_name;
      if (!arg.is_flag) {
        help_stream << "=<" << (arg.value_str ? "string" : "int") << ">";
      }
    }

    // Добавляем описание
    if (!arg.description.empty()) {
      help_stream << ", " << arg.description;
    }

    // Дополнительная информация, например, значение по умолчанию или обязательность аргумента
    if (arg.min_values > 0) {
      help_stream << " [repeated, min args = " << arg.min_values << "]";
    }
    if (arg.is_flag && arg.default_value_bool) {
      help_stream << " [default = true]";
    }

    help_stream << "\n";
  }

  // Заключительная строка для справки
  help_stream << "\n-h, --help Display this help and exit\n";

  return help_stream.str();
}

bool ArgParser::ProcessLongArgument(const std::string& arg) {
  size_t equal_pos = arg.find('=');
  std::string key = arg.substr(2, equal_pos - 2);
  std::string value = (equal_pos != std::string::npos) ? arg.substr(equal_pos + 1) : "true";

  for (auto& argument : arguments_) {
    if (argument.long_name == key) {
      if (argument.is_flag) {
        if (equal_pos != std::string::npos) {
          return false; // Ошибка: флаги не должны иметь значения.
        }
        if (argument.value_bool != nullptr) {
          *(argument.value_bool) = true;
        }
      } else {
        if (equal_pos == std::string::npos) {
          return false; // Ошибка: ожидалось значение после '='.
        }
        if (argument.value_int != nullptr) {
          char* end = nullptr;
          long val = strtol(value.c_str(), &end, 10);
          if (end != value.c_str() && *end == '\0' && val >= INT_MIN && val <= INT_MAX) {
            argument.value_int->push_back(static_cast<int>(val));
          } else {
            return false; // Ошибка: неправильное числовое значение.
          }
        } else if (argument.value_str != nullptr) {
          *(argument.value_str) = value;
        }
      }
      argument.is_set = true;
      return true; // Аргумент обработан.
    }
  }
  return false; // Ошибка: неизвестный аргумент.
}

bool ArgParser::ProcessShortArgument(const std::string& arg) {
  for (size_t j = 1; j < arg.length(); ++j) {
    char short_key = arg[j];
    bool is_last_character = (j == arg.length() - 1);

    Argument* argument = FindArgumentByShortKey(short_key);
    if (!argument) {
      return false; // Не найден соответствующий короткий ключ
    }

    if (argument->is_flag) {
      if (argument->value_bool) {
        *(argument->value_bool) = true;
      }
    } else {
      if (is_last_character) {
        return false; // Ошибка: ожидалось значение для аргумента.
      }
      std::string value = arg.substr(j + 2);
      if (argument->value_str) {
        *(argument->value_str) = value;
      }
      j = arg.length(); // Пропускаем оставшуюся часть строки.
    }
    argument->is_set = true;
  }
  return true;
}

bool ArgParser::ProcessPositionalArgument(const std::string& arg, size_t& positional_arg_index) {
  // В этом контексте мы ожидаем только один позиционный аргумент, который может иметь много значений.
  if (positional_arg_index >= arguments_.size()) {
    return false; // Ошибка: слишком много позиционных аргументов.
  }

  Argument& positional_arg = arguments_[positional_arg_index];
  if (!positional_arg.is_positional) {
    return false; // Ошибка: ожидался позиционный аргумент, но текущий аргумент не является позиционным.
  }

  char* end = nullptr;
  long val = strtol(arg.c_str(), &end, 10);
  if (end != arg.c_str() && *end == '\0' && val >= INT_MIN && val <= INT_MAX) {
    positional_arg.value_int->push_back(static_cast<int>(val));
    positional_arg.is_set = true;
    // Не увеличиваем positional_arg_index, так как мы ожидаем мультизначный аргумент.
  } else {
    return false; // Ошибка: неверное числовое значение.
  }
  return true;
}

bool ArgParser::Parse(const std::vector<std::string>& args) {
  size_t positional_arg_index = 0;
  for (size_t i = 1; i < args.size(); ++i) {
    std::string arg = args[i];

    if (arg.substr(0, 2) == "--") {
      if (!ProcessLongArgument(arg)) {
        return false;
      }
    } else if (arg[0] == '-') {
      if (!ProcessShortArgument(arg)) {
        return false;
      }
    } else {
      if (!ProcessPositionalArgument(arg, positional_arg_index)) {
        return false;
      }
    }
  }

  return CheckArgumentsAfterParsing();
}

bool ArgParser::CheckArgumentsAfterParsing() {
  // Проверка минимального количества значений для позиционных аргументов
  for (const auto& argument : arguments_) {
    if (argument.is_positional && argument.min_values > 0 && argument.value_int
        && argument.value_int->size() < argument.min_values) {
      return false; // Недостаточное количество значений для аргумента
    }
  }

  // Проверка минимального количества значений для мультизначных аргументов
  for (const auto& argument : arguments_) {
    if (argument.min_values > 0 && argument.value_int && argument.value_int->size() < argument.min_values) {
      return false; // Недостаточное количество значений для аргумента
    }
  }

  // Установка значений по умолчанию для аргументов, которые не были установлены
  for (auto& argument : arguments_) {
    if (!argument.is_set) {
      if (!argument.is_flag) {
        if (argument.value_str) {
          *argument.value_str = argument.default_value_str;
        }
      } else {
        if (argument.value_bool) {
          *argument.value_bool = argument.default_value_bool;
        }
      }
    }
  }

  for (auto& argument : arguments_) {
    if (argument.is_help && argument.is_set) {
      help_requested_ = true; // Устанавливаем флаг, что справка была запрошена
      return true; // Можно сразу вернуть true, так как дальнейший разбор аргументов не требуется
    }
  }

  // Проверка наличия всех обязательных аргументов
  for (const auto& argument : arguments_) {
    if (!argument.is_flag && !argument.is_set && argument.default_value_str.empty()) {
      return false; // Обязательный аргумент не установлен
    }
  }

  return true;
}
}