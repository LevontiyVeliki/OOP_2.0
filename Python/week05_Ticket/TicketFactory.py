from Ticket import Ticket
from TicketToRide import TicketToRide
from UnlimitedTicket import UnlimitedTicket
from LimitedTicketWithRides import LimitedTicketWithRides
from datetime import datetime

class TicketFactory:
    @staticmethod
    def create_ticket(ticket_type: str, *args):
        if ticket_type == "regular":
            return Ticket(*args)
        elif ticket_type == "ride":
            return TicketToRide(*args)
        elif ticket_type == "unlimited":
            return UnlimitedTicket(*args[0], args[1])  # num, cost
        elif ticket_type == "limited_rides":
            return LimitedTicketWithRides(*args)
        else:
            raise ValueError("Unknown ticket type")