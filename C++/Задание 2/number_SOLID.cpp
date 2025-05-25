#include "number.h"
#include <cstring>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <memory>

using namespace std;

// Реализация операций как производных классов
class AddOperation : public uint2022_t::Operation {
public:
    uint2022_t execute(const uint2022_t& lhs, const uint2022_t& rhs) const override {
        uint2022_t result = {};
        uint64_t carry = 0;
        bool overflow_happened = false;

        const uint2022_t MAX_70_DIGIT = []() {
            uint2022_t num = from_uint(1);
            for (int i = 0; i < 70; i++) {
                num = num * from_uint(10);
            }
            return num;
            }();

        if (lhs > MAX_70_DIGIT || rhs > MAX_70_DIGIT) {
            throw overflow_error("Overflow in uint2022_t addition");
        }

        if (MAX_70_DIGIT - lhs < rhs) {
            throw overflow_error("Overflow in uint2022_t addition");
        }

        for (size_t i = 0; i < LIMBS; ++i) {
            uint64_t sum = uint64_t(lhs.data[i]) + rhs.data[i] + carry;
            result.data[i] = static_cast<uint32_t>(sum);
            carry = sum >> 32;

            if (i == LIMBS - 1 && carry != 0) {
                overflow_happened = true;
            }
        }

        if (overflow_happened || result > MAX_70_DIGIT) {
            throw overflow_error("Overflow in uint2022_t addition");
        }

        return result;
    }
};

class SubOperation : public uint2022_t::Operation {
public:
    uint2022_t execute(const uint2022_t& lhs, const uint2022_t& rhs) const override {
        if (lhs < rhs) {
            throw overflow_error("Overflow in uint2022_t subtraction (negative result)");
        }

        uint2022_t result = {};
        int64_t borrow = 0;

        for (size_t i = 0; i < LIMBS; ++i) {
            int64_t diff = int64_t(lhs.data[i]) - rhs.data[i] - borrow;
            if (diff < 0) {
                diff += (1LL << 32);
                borrow = 1;
            }
            else {
                borrow = 0;
            }
            result.data[i] = static_cast<uint32_t>(diff);
        }

        if (borrow != 0) {
            throw overflow_error("Overflow in uint2022_t subtraction");
        }

        return result;
    }
};

// Аналогично реализуются MulOperation и DivOperation...

// Реализация фабрики операций
std::unique_ptr<uint2022_t::Operation> uint2022_t::OperationFactory::createAddOperation() {
    return std::make_unique<AddOperation>();
}

std::unique_ptr<uint2022_t::Operation> uint2022_t::OperationFactory::createSubOperation() {
    return std::make_unique<SubOperation>();
}

// Методы uint2022_t
uint2022_t uint2022_t::add(const uint2022_t& lhs, const uint2022_t& rhs) {
    auto op = OperationFactory::createAddOperation();
    return op->execute(lhs, rhs);
}

uint2022_t uint2022_t::sub(const uint2022_t& lhs, const uint2022_t& rhs) {
    auto op = OperationFactory::createSubOperation();
    return op->execute(lhs, rhs);
}

// Остальные функции остаются без изменений...
uint2022_t operator+(const uint2022_t& lhs, const uint2022_t& rhs) {
    return uint2022_t::add(lhs, rhs);
}

uint2022_t operator-(const uint2022_t& lhs, const uint2022_t& rhs) {
    return uint2022_t::sub(lhs, rhs);
}