from MyDateTime import MyDateTime
import json

class DateTimeCollection:
    def __init__(self):
        self._data = []

    def __str__(self):
        return "\n".join(str(dt) for dt in self._data)

    def __getitem__(self, index):
        return self._data[index]

    def add(self, dt):
        if isinstance(dt, MyDateTime):
            self._data.append(dt)
        else:
            raise ValueError("Only MyDateTime instances can be added.")

    def remove(self, index):
        if 0 <= index < len(self._data):
            del self._data[index]
        else:
            raise IndexError("Index out of range.")

    def save(self, filename):
        with open(filename, 'w') as file:
            json.dump([dt.to_dict() for dt in self._data], file)

    def load(self, filename):
        with open(filename, 'r') as file:
            data = json.load(file)
            self._data = [MyDateTime.from_dict(dt) for dt in data]