import json
from datetime import datetime, date, time, timedelta

class MyDateTime:
    def __init__(self, date_obj: date, time_obj: time):
        self._date = date_obj
        self._time = time_obj

    @property
    def date_property(self):
        return self._date

    @date_property.setter
    def date_property(self, value):
        if isinstance(value, date):
            self._date = value
        else:
            raise ValueError("Must be a date object")

    @property
    def time_property(self):
        return self._time

    @time_property.setter
    def time_property(self, value):
        if isinstance(value, time):
            self._time = value
        else:
            raise ValueError("Must be a time object")

    @property
    def datetime_property(self):
        return datetime.combine(self._date, self._time)

    def __str__(self):
        return f"{self._date} {self._time}"

    def __add__(self, other):
        if isinstance(other, timedelta):
            new_datetime = self.datetime_property + other
            return MyDateTime(new_datetime.date(), new_datetime.time())
        return NotImplemented

    def __sub__(self, other):
        if isinstance(other, MyDateTime):
            return self.datetime_property - other.datetime_property
        return NotImplemented

    @classmethod
    def from_string(cls, str_value):
        dt = datetime.strptime(str_value, '%Y-%m-%d %H:%M')
        return cls(dt.date(), dt.time())

    def save(self, filename):
        data = {
            'date': self._date.isoformat(),
            'time': self._time.isoformat()
        }
        with open(filename, 'w') as f:
            json.dump(data, f)

    @classmethod
    def load(cls, filename):
        with open(filename, 'r') as f:
            data = json.load(f)
            return cls(date.fromisoformat(data['date']), time.fromisoformat(data['time']))

    def add_days(self, days):
        self.date_property += timedelta(days=days)

    def subtract_days(self, days):
        self.date_property -= timedelta(days=days)

    def current_time(self):
        return datetime.now().time()