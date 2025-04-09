from datetime import datetime
from Ticket import Ticket

class TicketToRide(Ticket):
    def __init__(self, num, cost, legal_to):
        super().__init__(num, cost, legal_to)