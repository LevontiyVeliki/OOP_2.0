from datetime import datetime
from TicketToRide import TicketToRide
from LimitedTicketWithRides import LimitedTicketWithRides

ticket1 = TicketToRide("001", 1200, datetime(2025, 1, 25, 10, 40, 0 ))

ticket2 = LimitedTicketWithRides("002", 2500, datetime(2025, 1, 25, 10, 40, 0 ), datetime(2025, 2, 25, 10, 40, 0), 5)
ticket2.off_a_trip()
print(ticket1)