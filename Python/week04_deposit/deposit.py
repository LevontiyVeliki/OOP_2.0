class Deposit:
    def __init__(self, name, interest_rate, period_limit, sum_limit):
        self.name = name
        self._interest_rate = interest_rate
        self._period_limit = period_limit  # tuple (min_months, max_months)
        self._sum_limit = sum_limit  # tuple (min_sum, max_sum)
        self.currency = "RUB"  # Задаем валюту по умолчанию

    def _check_self(self, initial_sum, period):
        if not (self._sum_limit[0] <= initial_sum <= self._sum_limit[1]):
            raise ValueError("Сумма вклада не соответствует лимитам.")
        if not (self._period_limit[0] <= period < self._period_limit[1]):
            raise ValueError("Срок вклада не соответствует лимитам.")

    def get_profit(self, initial_sum, period):
        raise NotImplementedError("Метод get_profit не реализован.")

    def get_sum(self, initial_sum, period):
        return initial_sum + self.get_profit(initial_sum, period)


class TermDeposit(Deposit):
    def get_profit(self, initial_sum, period):
        self._check_self(initial_sum, period)
        return initial_sum * (self._interest_rate / 100) * (period / 12)  # Простой процент


class BonusDeposit(Deposit):
    def __init__(self, name, interest_rate, period_limit, sum_limit, bonus_rate):
        super().__init__(name, interest_rate, period_limit, sum_limit)
        self.bonus_rate = bonus_rate

    def get_profit(self, initial_sum, period):
        self._check_self(initial_sum, period)
        profit = initial_sum * (self._interest_rate / 100) * (period / 12)
        if initial_sum > self._sum_limit[0]:  # Если сумма больше минимальной
            profit += profit * (self.bonus_rate / 100)  # Бонус
        return profit


class CapitalizedDeposit(Deposit):
    def get_profit(self, initial_sum, period):
        self._check_self(initial_sum, period)
        total_sum = initial_sum * (1 + self._interest_rate / 100 / 12) ** (period)
        return total_sum - initial_sum  # Прибыль




