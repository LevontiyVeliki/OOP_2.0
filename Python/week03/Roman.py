class Roman:
    roman_numerals = {
        1: 'I', 4: 'IV', 5: 'V', 9: 'IX', 10: 'X', 40: 'XL', 50: 'L',
        90: 'XC', 100: 'C', 400: 'CD', 500: 'D', 900: 'CM', 1000: 'M'
    }

    def __init__(self, value):
        if isinstance(value, str):
            self.value = self.roman_to_int(value)
        elif isinstance(value, int):
            self.value = value
        else:
            raise ValueError("Значение должно быть либо строкой, представляющей римское число, либо целым числом.")

    def __add__(self, other):
        return Roman(self.value + other.value)

    def __sub__(self, other):
        result_value = self.value - other.value
        if result_value <= 0:
            raise ValueError("Результат вычитания должен быть положительным.")
        return Roman(result_value)

    def __mul__(self, other):
        return Roman(self.value * other.value)

    def __truediv__(self, other):
        if other.value == 0:
            raise ZeroDivisionError("Деление на ноль.")
        return Roman(self.value // other.value)

    def __str__(self):
        return self.int_to_roman(self.value)

    def __repr__(self):
        return f"Roman('{self}')"

    @staticmethod
    def int_to_roman(num):
        if num <= 0:
            raise ValueError("Римское число не может быть меньше 1.")

        roman_string = ''
        for value in sorted(Roman.roman_numerals.keys(), reverse=True):
            while num >= value:
                roman_string += Roman.roman_numerals[value]
                num -= value
        return roman_string

    @staticmethod
    def roman_to_int(s):
        roman_values = {v: k for k, v in Roman.roman_numerals.items()}
        total = 0
        prev_value = 0

        for char in reversed(s):
            current_value = roman_values[char]
            if current_value < prev_value:
                total -= current_value
            else:
                total += current_value
            prev_value = current_value
        return total


a = Roman("X")
b = Roman("V")

print(a * b)
print(a / b)
print(a + b)
print(a - b)

print(Roman.int_to_roman(10))
print(Roman.roman_to_int("X"))