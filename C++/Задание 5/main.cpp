// main.cpp
#include "lib/parser.h"
#include "lib/parser.cpp"
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <sstream>

using namespace omfl;
namespace fs = std::filesystem;

void PrintHelp() {
    std::cout << "OMFL Parser Tool\n"
        << "Usage:\n"
        << "  omfl-parser <file> [command] [arguments]\n\n"
        << "Commands:\n"
        << "  get <key.path>  - Get value by key (e.g. 'section.key' or 'section.subsection.key')\n"
        << "  validate        - Validate OMFL file\n"
        << "  dump            - Show all configuration\n"
        << "  help            - Show this help\n";
}

void PrintValue(const OMFLValue& value, int indent = 0) {
    std::string indent_str(indent, ' ');

    if (value.IsInt()) {
        std::cout << indent_str << value.AsInt() << " (int)\n";
    }
    else if (value.IsFloat()) {
        std::cout << indent_str << std::fixed << std::setprecision(6)
            << value.AsFloat() << " (float)\n";
    }
    else if (value.IsBool()) {
        std::cout << indent_str << (value.AsBool() ? "true" : "false") << " (bool)\n";
    }
    else if (value.IsString()) {
        std::cout << indent_str << "\"" << value.AsString() << "\" (string)\n";
    }
    else if (value.IsArray()) {
        std::cout << indent_str << "[\n";
        for (size_t i = 0; ; ++i) {
            const OMFLValue& element = value[i];
            if (!element.valid()) break;
            std::cout << indent_str << "  " << i << ": ";
            PrintValue(element, indent + 4);
        }
        std::cout << indent_str << "]\n";
    }
}

const OMFLValue* FindValue(const OMFLSection& section, const std::vector<std::string>& path) {
    const OMFLSection* current = &section;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        auto it = current->sections.find(path[i]);
        if (it == current->sections.end()) {
            return nullptr;
        }
        current = it->second.get();
    }

    auto it = current->values.find(path.back());
    if (it != current->values.end()) {
        return &it->second;
    }

    return nullptr;
}

void PrintSection(const OMFLSection& section, const std::string& section_name = "", int indent = 0) {
    std::string indent_str(indent, ' ');

    if (!section_name.empty()) {
        std::cout << indent_str << "[" << section_name << "]\n";
    }

    for (const auto& [key, value] : section.values) {
        std::cout << indent_str << key << " = ";
        PrintValue(value, indent + key.length() + 3);
    }

    for (const auto& [name, subsection] : section.sections) {
        std::string full_name = section_name.empty() ? name : section_name + "." + name;
        PrintSection(*subsection, full_name, indent);
    }
}

std::vector<std::string> SplitPath(const std::string& path) {
    std::vector<std::string> result;
    std::stringstream ss(path);
    std::string item;

    while (std::getline(ss, item, '.')) {
        result.push_back(item);
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintHelp();
        return 1;
    }

    fs::path file_path(argv[1]);
    if (!fs::exists(file_path)) {
        std::cerr << "Error: File not found - " << file_path << "\n";
        return 1;
    }

    OMFLRoot config = OMFLRoot::parse(file_path);
    if (!config.valid()) {
        std::cerr << "Error: Invalid OMFL file format\n";
        return 1;
    }

    if (argc == 2) {
        PrintSection(config);
        return 0;
    }

    std::string command(argv[2]);
    if (command == "get" && argc >= 4) {
        std::vector<std::string> path = SplitPath(argv[3]);
        if (path.empty()) {
            std::cerr << "Error: Invalid key path\n";
            return 1;
        }

        const OMFLValue* value = FindValue(config, path);
        if (value && value->valid()) {
            PrintValue(*value);
        }
        else {
            std::cerr << "Key not found: " << argv[3] << "\n";
            return 1;
        }
    }
    else if (command == "validate") {
        std::cout << "OMFL file is valid\n";
    }
    else if (command == "dump") {
        PrintSection(config);
    }
    else if (command == "help") {
        PrintHelp();
    }
    else {
        std::cerr << "Invalid command\n";
        PrintHelp();
        return 1;
    }

    return 0;
}