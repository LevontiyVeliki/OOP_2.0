from MyDateTime import MyDateTime
from Collector import DateTimeCollection

dt = MyDateTime(2010, 10, 10, 20, 31, 0)
dt_mas = DateTimeCollection()

dt_mas.add(dt)

print(dt_mas[0])