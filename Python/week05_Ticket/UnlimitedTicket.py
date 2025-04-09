from datetime import datetime
from Ticket import Ticket

class UnlimitedTicket(Ticket):
    def __init__(self, num, cost):
        super().__init__(num, cost, None)

    def off_a_trip(self):
        print(f"Безлимитный билет {self._num} не требует списания поездок.")