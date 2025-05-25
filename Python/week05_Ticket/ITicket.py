from abc import ABC, abstractmethod
from datetime import datetime


class ITicket(ABC):
    @abstractmethod
    def off_a_trip(self):
        pass

    @abstractmethod
    def info(self) -> str:
        pass

    @abstractmethod
    def __str__(self) -> str:
        pass