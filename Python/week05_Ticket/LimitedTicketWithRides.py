from LimitedTicket import LimitedTicket
from datetime import datetime

class LimitedTicketWithRides(LimitedTicket):
    def __init__(self, num, cost, legal_to, border, rides : int):
        super().__init__(num, cost, legal_to, None)
        self.rides = rides

    def off_a_trip(self):
        if self.rides > 0:
            self.rides -= 1
            print(f"Поездка списана с билета {self._num}. Осталось поездок: {self.rides}.")
        else:
            print(f"Билет {self._num} исчерпан, поездки закончились!")