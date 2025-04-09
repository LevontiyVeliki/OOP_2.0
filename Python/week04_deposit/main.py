from deposit import TermDeposit, BonusDeposit, CapitalizedDeposit

if __name__ == "__main__":
    print("Добро пожаловать в систему подбора вкладов!")

    while True:
        print("\n-----")
        print("Нажмите 1, чтобы подобрать вклад, или что угодно для выхода.")

        answer = input()
        if answer == "1":

            print("Выберите: 1)Срочный вклад 2)Бонусный вклад 3)Вклад с капитализацией")
            answer_1 = input()
            if answer_1 == "1":
                deposit = TermDeposit("Срочный вклад", 5.0, (1, 36), (100, 100000))

                initial_sum = float(input("1/2: Введите начальную сумму вклада: "))
                period = int(input("2/2: Введите срок вклада (мес.): "))
                print(f"Прибыль: {deposit.get_profit(initial_sum, period):.2f} {deposit.currency}")
            elif answer_1 == "2":
                deposit = BonusDeposit("Бонусный вклад", 6.0, (1, 36), (100, 100000), 1.5)

                initial_sum = float(input("1/2: Введите начальную сумму вклада: "))
                period = int(input("2/2: Введите срок вклада (мес.): "))
                print(f"Прибыль: {deposit.get_profit(initial_sum, period):.2f} {deposit.currency}")
            elif answer_1 == "3":
                deposit = CapitalizedDeposit("Вклад с капитализацией", 4.5, (1, 60), (100, 50000))

                initial_sum = float(input("1/2: Введите начальную сумму вклада: "))
                period = int(input("2/2: Введите срок вклада (мес.): "))
                print(f"Прибыль: {deposit.get_profit(initial_sum, period):.2f} {deposit.currency}")
        else:
            break

    print("\nСпасибо, что воспользовались терминалом банка! До встречи!")

