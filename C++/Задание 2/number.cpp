#include "number.h"
#include <cstring>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

int chuse(){

    setlocale(LC_ALL, "Russian");
    int chuse = 0;
    cout << "Выберите: число за границами будет обрезаться справа(1), вычисления будут производиться с числом любой длинны(2):  ";
    cin >> chuse;
    return chuse;

}

uint2022_t from_uint(uint32_t i) {
    uint2022_t result = {};
    result.data[0] = i;
    return result;
}

uint2022_t from_string(const char* buff) {

    //int chek = chuse();
    uint2022_t result = from_uint(0);
    string str(buff);
    int cnt = 1;

    for (char c : str) {
        if (c < '0' || c > '9') throw invalid_argument("Invalid digit in string");
        result = result * from_uint(10);
        result = result + from_uint(c - '0');
        cnt += 1;
        //if (cnt >= 68 && chek == 1)
          //  return result;
    }

    return result;
}

uint2022_t operator+(const uint2022_t& lhs, const uint2022_t& rhs) {
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
        throw overflow_error("Overflow in uint2022_t subtraction (negative result)");
    }

    if (MAX_70_DIGIT - lhs < rhs) {
        throw overflow_error("Overflow in uint2022_t subtraction (negative result)");
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
        throw overflow_error("Overflow in uint2022_t subtraction (negative result)");
    }

    return result;
}

uint2022_t operator-(const uint2022_t& lhs, const uint2022_t& rhs) {
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

uint2022_t operator*(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint64_t temp[2 * LIMBS] = {};
    bool nonZeroLhs = false, nonZeroRhs = false;

    for (size_t i = 0; i < LIMBS; ++i) {
        if (lhs.data[i] != 0) nonZeroLhs = true;
        if (rhs.data[i] != 0) nonZeroRhs = true;
    }

    for (size_t i = 0; i < LIMBS; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < LIMBS; ++j) {
            size_t k = i + j;
            if (k >= 2 * LIMBS) continue;
            uint64_t mul = uint64_t(lhs.data[i]) * rhs.data[j];
            uint64_t sum = temp[k] + mul + carry;
            carry = sum >> 32;
            temp[k] = static_cast<uint32_t>(sum);
        }
        size_t carry_index = i + LIMBS;
        if (carry_index < 2 * LIMBS) {
            temp[carry_index] += carry;
        }
        else if (carry != 0) {
            cerr << "Overflow in uint2022_t multiplication (carry overflow)" << endl;
            throw overflow_error("Overflow in uint2022_t multiplication");
        }
    }

    for (size_t i = LIMBS; i < 2 * LIMBS; ++i) {
        if (temp[i] != 0) {
            cerr << "Overflow in uint2022_t multiplication (higher limbs non-zero)" << endl;
            throw overflow_error("Overflow in uint2022_t multiplication");
        }
    }

    uint2022_t result;
    for (size_t i = 0; i < LIMBS; ++i) {
        result.data[i] = static_cast<uint32_t>(temp[i]);
    }

    if (result == from_uint(0) && (nonZeroLhs && nonZeroRhs)) {
        cerr << "Overflow in uint2022_t multiplication (unexpected zero result)" << endl;
        throw overflow_error("Overflow in uint2022_t multiplication (unexpected zero result)");
    }

    return result;
}


uint2022_t operator/(const uint2022_t& lhs, const uint2022_t& rhs) {
    if (rhs == from_uint(0)) {
        throw domain_error("Division by zero in uint2022_t");
    }
    uint2022_t result = from_uint(0);
    uint2022_t remainder = from_uint(0);

    for (int i = LIMBS * 32 - 1; i >= 0; --i) {
        for (int j = LIMBS - 1; j > 0; --j) {
            remainder.data[j] = (remainder.data[j] << 1) | (remainder.data[j - 1] >> 31);
        }
        remainder.data[0] = (remainder.data[0] << 1) | ((lhs.data[i / 32] >> (i % 32)) & 1);

        if (!(remainder < rhs)) {
            remainder = remainder - rhs;
            result.data[i / 32] |= (1u << (i % 32));
        }
    }

    return result;
}

bool operator==(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (size_t i = 0; i < LIMBS; ++i) {
        if (lhs.data[i] != rhs.data[i]) return false;
    }
    return true;
}

bool operator!=(const uint2022_t& lhs, const uint2022_t& rhs) {
    return !(lhs == rhs);
}

bool operator<(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (int i = LIMBS - 1; i >= 0; --i) {
        if (lhs.data[i] < rhs.data[i]) return true;
        if (lhs.data[i] > rhs.data[i]) return false;
    }
    return false;
}

bool operator>(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (int i = LIMBS - 1; i >= 0; --i) {
        if (lhs.data[i] < rhs.data[i]) return false;
        if (lhs.data[i] > rhs.data[i]) return true;
    }
    return false;
}

pair<uint2022_t, uint8_t> divmod10(const uint2022_t& value) {
    uint2022_t quotient = from_uint(0);
    uint64_t remainder = 0;

    for (int i = LIMBS - 1; i >= 0; --i) {
        uint64_t part = (remainder << 32) | value.data[i];
        quotient.data[i] = static_cast<uint32_t>(part / 10);
        remainder = static_cast<uint8_t>(part % 10);
    }

    return { quotient, static_cast<uint8_t>(remainder) };
}

ostream& operator<<(ostream& stream, const uint2022_t& value) {
    uint2022_t temp = value;
    string result;

    while (!(temp == from_uint(0))) {
        auto [quotient, remainder] = divmod10(temp);
        result += char('0' + remainder);
        temp = quotient;
    }

    if (result.empty()) {
        result = "0";
    }

    reverse(result.begin(), result.end());
    stream << result;
    return stream;
}