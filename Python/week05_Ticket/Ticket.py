from datetime import datetime
from ITicket import ITicket

class Ticket(ITicket):
    def __init__(self, num: str, cost: int, legal_to: datetime):
        self._num = num
        self.__cost = cost
        self.legal_to = legal_to

    def __str__(self):
        return str(self._num) + " " + str(self.__cost) + " " + str(self.legal_to)

    def off_a_trip(self):
        print(f"Билет {self._num} использован!")

    def info(self):
        return f"Билет {self._num}: стоимость {self.__cost}, действителен до {self.legal_to}."