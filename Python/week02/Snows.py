import abc

class Snow(abc.ABC):
    def __init__(self):
        self.__count_of_snows = 15
        self.__count_of_snows_save = self.__count_of_snows

    def __call__(self):
        self.__count_of_snows = 15
        self.__count_of_snows_save = self.__count_of_snows

    def __add__(self, n : int):
        self.__count_of_snows += n
        print(self.__count_of_snows)

    def __sub__(self, n : int):
        if self.__count_of_snows - n < 0:
            self.__count_of_snows = 0
        else:
            self.__count_of_snows -= n
        print(self.__count_of_snows)

    def __mul__(self, n : int):
        self.__count_of_snows *= n
        print(self.__count_of_snows)

    def __truediv__(self, n : int):
        if n != 0:
            self.__count_of_snows /= n
        print(self.__count_of_snows)

    def make_snow(self, cil : int):
        while self.__count_of_snows > 0:
            if self.__count_of_snows >= cil:
                self.__count_of_snows -= cil
                print("*" * int(cil) + "\n")
            else:
                print("*" * int(self.__count_of_snows) + "\n")
                self.__count_of_snows = 0
        self.__count_of_snows = self.__count_of_snows_save

    @abc.abstractmethod
    def Snow_man(self):
        pass






class Snow_baby(Snow):
    def __call__(self):
        super().__call__()

    def __add__(self, n : int):
        return super().__add__(n)

    def __sub__(self, n : int):
        return super().__sub__(n)

    def __mul__(self, n : int):
        return super().__mul__(n)

    def __truediv__(self, n : int):
        return super().__truediv__(n)

    def make_snow(self, cil : int):
        return super().make_snow(cil)

    def Snow_man(self):
        print("_(* >*)_")
        return 0


sneg = Snow_baby()
sneg + 15
sneg / 5

sneg.Snow_man()

