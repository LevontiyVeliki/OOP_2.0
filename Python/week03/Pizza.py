class Pizza:
    def __init__(self, name, dough, sauce, toppings, price):
        self.name = name
        self.dough = dough
        self.sauce = sauce
        self.toppings = toppings
        self.price = price

    def prepare(self):
        print(f"Готовим {self.name} с тестом '{self.dough}', соусом '{self.sauce}' и начинкой {', '.join(self.toppings)}")

    def bake(self):
        print(f"Выпекаем {self.name}...")

    def cut(self):
        print(f"Режем {self.name} на кусочки.")

    def box(self):
        print(f"Упаковываем {self.name}.")


class PepperoniPizza(Pizza):
    def __init__(self):
        super().__init__("Пепперони", "тонкое", "томатный", ["пепперони", "сыр"], 500)


class BBQPizza(Pizza):
    def __init__(self):
        super().__init__("Барбекю", "толстое", "барбекю", ["курица", "лук", "сыр"], 550)

class SeafoodPizza(Pizza):
    def __init__(self):
        super().__init__("Дары Моря", "тонкое", "томаты", ["креветки", "мидии", "сыр"], 600)


class Order:
    def __init__(self):
        self.pizzas = []

    def add_pizza(self, pizza):
        self.pizzas.append(pizza)

    def get_total(self):
        return sum(pizza.price for pizza in self.pizzas)

    def show_order(self):
        for pizza in self.pizzas:
            print(f"{pizza.name} - {pizza.price} руб.")
        print(f"Общая сумма: {self.get_total()} руб.")


class Terminal:
    def __init__(self):
        self.menu = [
            PepperoniPizza(),
            BBQPizza(),
            SeafoodPizza()
        ]
        self.order = None

    def display_menu(self):
        print("Меню:")
        for i, pizza in enumerate(self.menu):
            print(f"{i + 1}. {pizza.name} - {pizza.price} руб.")

    def take_order(self):
        self.order = Order()
        while True:
            self.display_menu()
            choice = input("Выберите номер пиццы для добавления в заказ (или 'q' для завершения): ")
            if choice.lower() == 'q':
                break
            try:
                index = int(choice) - 1
                if 0 <= index < len(self.menu):
                    self.order.add_pizza(self.menu[index])
                else:
                    print("Неверный номер.")
            except ValueError:
                print("Пожалуйста, введите номер или 'q'.")

    def confirm_order(self):
        if self.order:
            self.order.show_order()
            input("Подтвердите заказ и нажмите Enter для оплаты.")
            print("Оплата принята!")
            print("Заказ принят на выполнение.")
        else:
            print("Нет активного заказа.")


def main():
    terminal = Terminal()
    terminal.take_order()
    terminal.confirm_order()


if __name__ == "__main__":
    main()