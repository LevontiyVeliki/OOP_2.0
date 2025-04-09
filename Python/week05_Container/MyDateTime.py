import json
from datetime import datetime, timedelta


class MyDateTime:
    def __init__(self, year, month, day, hour=0, minute=0, second=0):
        self.date_time = datetime(year, month, day, hour, minute, second)

    def __str__(self):
        return self.date_time.strftime("%Y-%m-%d %H:%M:%S")

    def __add__(self, other):
        if isinstance(other, timedelta):
            return MyDateTime.from_datetime(self.date_time + other)
        raise TypeError("Unsupported type for addition")

    def __sub__(self, other):
        if isinstance(other, MyDateTime):
            return self.date_time - other.date_time
        elif isinstance(other, timedelta):
            return MyDateTime.from_datetime(self.date_time - other)
        raise TypeError("Unsupported type for subtraction")

    @classmethod
    def from_string(cls, str_value):
        dt = datetime.strptime(str_value, "%Y-%m-%d %H:%M:%S")
        return cls(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second)

    def to_dict(self):
        return {
            'year': self.date_time.year,
            'month': self.date_time.month,
            'day': self.date_time.day,
            'hour': self.date_time.hour,
            'minute': self.date_time.minute,
            'second': self.date_time.second
        }

    @classmethod
    def from_dict(cls, dt_dict):
        return cls(dt_dict['year'], dt_dict['month'], dt_dict['day'],
                   dt_dict['hour'], dt_dict['minute'], dt_dict['second'])

    def save(self, filename):
        with open(filename, 'w') as f:
            json.dump(self.date_time.strftime("%Y-%m-%d %H:%M:%S"), f)

    def load(self, filename):
        with open(filename, 'r') as f:
            dt_str = json.load(f)
            self.date_time = self.from_string(dt_str).date_time

    def get_day(self):
        return self.date_time.day

    def get_month(self):
        return self.date_time.month

    def get_year(self):
        return self.date_time.year

    def to_unix_timestamp(self):
        return int(self.date_time.timestamp())

    @classmethod
    def from_datetime(cls, dt):
        return cls(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second)