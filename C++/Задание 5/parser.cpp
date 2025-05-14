// lib/parser.cpp
#include "parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <charconv>

using namespace std;


namespace omfl {

    OMFLValue OMFLSection::empty_value_;
    const OMFLValue OMFLSection::empty_section_;

    OMFLValue::OMFLValue(Value value) : value_(move(value)) {}

    bool OMFLValue::IsInt() const {
        return holds_alternative<int32_t>(value_);
    }

    bool OMFLValue::IsFloat() const {
        return holds_alternative<float>(value_);
    }

    bool OMFLValue::IsBool() const {
        return holds_alternative<bool>(value_);
    }

    bool OMFLValue::IsString() const {
        return holds_alternative<string>(value_);
    }

    bool OMFLValue::IsArray() const {
        return holds_alternative<vector<OMFLValue>>(value_);
    }

    bool OMFLValue::valid() const {
        return !holds_alternative<monostate>(value_);
    }

    int32_t OMFLValue::AsInt() const {
        if (IsInt()) return get<int32_t>(value_);
        if (IsFloat()) return static_cast<int32_t>(get<float>(value_));
        throw runtime_error("Not an integer");
    }

    int32_t OMFLValue::AsIntOrDefault(int32_t default_value) const {
        return IsInt() ? AsInt() : default_value;
    }

    float OMFLValue::AsFloat() const {
        if (IsFloat()) return get<float>(value_);
        if (IsInt()) return static_cast<float>(get<int32_t>(value_));
        throw runtime_error("Not a float");
    }

    float OMFLValue::AsFloatOrDefault(float default_value) const {
        return IsFloat() || IsInt() ? AsFloat() : default_value;
    }

    bool OMFLValue::AsBool() const {
        if (!IsBool()) throw runtime_error("Not a boolean");
        return get<bool>(value_);
    }

    bool OMFLValue::AsBoolOrDefault(bool default_value) const {
        return IsBool() ? AsBool() : default_value;
    }

    const string& OMFLValue::AsString() const {
        if (!IsString()) throw runtime_error("Not a string");
        return get<string>(value_);
    }

    string OMFLValue::AsStringOrDefault(const string& default_value) const {
        return IsString() ? AsString() : default_value;
    }

    const OMFLValue& OMFLValue::operator[](size_t index) const {
        if (!IsArray()) return OMFLSection::empty_value_;
        const auto& arr = get<vector<OMFLValue>>(value_);
        if (index >= arr.size()) return OMFLSection::empty_value_;
        return arr[index];
    }

    const OMFLValue& OMFLSection::Get(const string& key) const {
        auto it = values.find(key);
        if (it != values.end()) {
            return it->second;
        }
        return empty_value_;
    }

    void OMFLSection::AddValue(const string& key, OMFLValue::Value value) {
        values[key] = OMFLValue(move(value));
    }

    void OMFLSection::AddSection(const string& key, shared_ptr<OMFLSection> section) {
        sections[key] = move(section);
    }

    OMFLRoot OMFLRoot::parse(const string& str) {
        istringstream iss(str);
        OMFLRoot root;
        root.ParseStream(iss);
        return root;
    }

    OMFLRoot OMFLRoot::parse(const filesystem::path& path) {
        ifstream ifs(path);
        OMFLRoot root;
        if (ifs) {
            root.ParseStream(ifs);
        }
        return root;
    }

    bool OMFLRoot::ParseStream(istream& input) {
        string line;
        size_t line_number = 0;
        vector<string> current_section;

        auto trim = [](string& s) {
            s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) { return !isspace(ch); }));
            s.erase(find_if(s.rbegin(), s.rend(), [](int ch) { return !isspace(ch); }).base(), s.end());
            };

        auto remove_comments = [](string& s) {
            size_t pos = s.find('#');
            if (pos != string::npos) {
                s.erase(pos);
            }
            };

        auto is_valid_key = [](const string& key) {
            if (key.empty()) return false;
            for (char c : key) {
                if (!isalnum(c) && c != '-' && c != '_') return false;
            }
            return true;
            };

        while (getline(input, line)) {
            line_number++;
            trim(line);
            remove_comments(line);
            if (line.empty()) continue;

            if (line.front() == '[' && line.back() == ']') {
                string section_name = line.substr(1, line.size() - 2);
                trim(section_name);

                if (section_name.empty() || section_name.front() == '.' || section_name.back() == '.') {
                    return false;
                }

                current_section = {};
                istringstream section_stream(section_name);
                string part;
                while (getline(section_stream, part, '.')) {
                    trim(part);
                    if (!is_valid_key(part)) {
                        return false;
                    }
                    current_section.push_back(part);
                }
            }
            else {
                size_t equal_pos = line.find('=');
                if (equal_pos == string::npos) {
                    return false;
                }

                string key = line.substr(0, equal_pos);
                trim(key);
                if (!is_valid_key(key)) {
                    return false;
                }

                string value_str = line.substr(equal_pos + 1);
                trim(value_str);
                if (value_str.empty()) {
                    return false;
                }

                OMFLValue::Value value;
                if (value_str.front() == '"' && value_str.back() == '"') {
                    value = value_str.substr(1, value_str.size() - 2);
                }
                else if (value_str == "true") {
                    value = true;
                }
                else if (value_str == "false") {
                    value = false;
                }
                else if (value_str.front() == '[' && value_str.back() == ']') {
                    vector<OMFLValue> array;
                    string array_content = value_str.substr(1, value_str.size() - 2);
                    trim(array_content);

                    if (!array_content.empty()) {
                        istringstream array_stream(array_content);
                        string element;
                        while (getline(array_stream, element, ',')) {
                            trim(element);
                            if (element.empty()) continue;

                            if (element.front() == '"' && element.back() == '"') {
                                array.emplace_back(element.substr(1, element.size() - 2));
                            }
                            else if (element == "true") {
                                array.emplace_back(true);
                            }
                            else if (element == "false") {
                                array.emplace_back(false);
                            }
                            else if (element.find('.') != string::npos) {
                                float f;
                                auto result = from_chars(element.data(), element.data() + element.size(), f);
                                if (result.ec != errc() || *result.ptr != '\0') {
                                    return false;
                                }
                                array.emplace_back(f);
                            }
                            else {
                                int32_t i;
                                auto result = from_chars(element.data(), element.data() + element.size(), i);
                                if (result.ec != errc() || *result.ptr != '\0') {
                                    return false;
                                }
                                array.emplace_back(i);
                            }
                        }
                    }
                    value = move(array);
                }
                else if (value_str.find('.') != string::npos) {
                    float f;
                    auto result = from_chars(value_str.data(), value_str.data() + value_str.size(), f);
                    if (result.ec != errc() || *result.ptr != '\0') {
                        return false;
                    }
                    value = f;
                }
                else {
                    int32_t i;
                    auto result = from_chars(value_str.data(), value_str.data() + value_str.size(), i);
                    if (result.ec != errc() || *result.ptr != '\0') {
                        return false;
                    }
                    value = i;
                }

                OMFLSection* current = this;
                for (const auto& section : current_section) {
                    auto it = current->sections.find(section);
                    if (it == current->sections.end()) {
                        current->sections[section] = make_shared<OMFLSection>();
                    }
                    current = current->sections[section].get();
                }

                if (current->values.count(key)) {
                    return false;
                }

                current->AddValue(key, move(value));
            }
        }

        return true;
    }

    OMFLRoot parse(const string& str) {
        return OMFLRoot::parse(str);
    }

    OMFLRoot parse(const filesystem::path& path) {
        return OMFLRoot::parse(path);
    }

}// namespace omfl