from datetime import datetime
from Ticket import Ticket

class LimitedTicket(Ticket):
    def __init__(self, num, cost, legal_to, border : datetime):
        super().__init__(num, cost, legal_to)
        self.border = border

    def off_a_trip(self, time_now : datetime):
        if time_now > self.border:
            print(f"Билет {self._num} истёк!")
        else:
            print(f"Билет {self._num} использован! Оставшееся время: {self.border - time_now}.")