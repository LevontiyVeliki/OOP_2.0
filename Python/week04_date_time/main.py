from my_datetime import MyDateTime
from datetime import datetime, date, time, timedelta

# Создание объекта
dt1 = MyDateTime(date(2023, 10, 8), time(14, 30))
print("Созданный объект:", dt1)

# Сохранение объекта в файл
dt1.save('datetime.json')

# Загрузка объекта из файла
dt2 = MyDateTime.load('datetime.json')
print("Загруженный объект:", dt2)

# Сложение с timedelta
dt3 = dt1 + timedelta(days=1)
print("Сложение с timedelta:", dt3)

# Вычитание MyDateTime
delta = dt3 - dt1
print("Разница между датами:", delta)
