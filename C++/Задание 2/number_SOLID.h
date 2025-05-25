#pragma once
#include <cinttypes>
#include <iostream>
#include <functional>
#include <memory>

const size_t LIMBS = 70;

struct uint2022_t {
    uint32_t data[LIMBS]{};

    // Базовый класс для операций
    class Operation {
    public:
        virtual uint2022_t execute(const uint2022_t& lhs, const uint2022_t& rhs) const = 0;
        virtual ~Operation() = default;
    };

    // Фабрика операций
    class OperationFactory {
    public:
        static std::unique_ptr<Operation> createAddOperation();
        static std::unique_ptr<Operation> createSubOperation();
        static std::unique_ptr<Operation> createMulOperation();
        static std::unique_ptr<Operation> createDivOperation();
    };

    // Методы для выполнения операций через фабрику
    static uint2022_t add(const uint2022_t& lhs, const uint2022_t& rhs);
    static uint2022_t sub(const uint2022_t& lhs, const uint2022_t& rhs);
    static uint2022_t mul(const uint2022_t& lhs, const uint2022_t& rhs);
    static uint2022_t div(const uint2022_t& lhs, const uint2022_t& rhs);
};

static_assert(sizeof(uint2022_t) <= 300, "Size of uint2022_t must be no higher than 300 bytes");

uint2022_t from_uint(uint32_t i);
uint2022_t from_string(const char* buff);
bool operator==(const uint2022_t& lhs, const uint2022_t& rhs);
bool operator!=(const uint2022_t& lhs, const uint2022_t& rhs);
bool operator<(const uint2022_t& lhs, const uint2022_t& rhs);
bool operator>(const uint2022_t& lhs, const uint2022_t& rhs);
std::ostream& operator<<(std::ostream& stream, const uint2022_t& value);

// Операторы теперь используют фабрику операций
uint2022_t operator+(const uint2022_t& lhs, const uint2022_t& rhs);
uint2022_t operator-(const uint2022_t& lhs, const uint2022_t& rhs);
uint2022_t operator*(const uint2022_t& lhs, const uint2022_t& rhs);
uint2022_t operator/(const uint2022_t& lhs, const uint2022_t& rhs);