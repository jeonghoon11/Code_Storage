hamburger = []
drink = []
for _ in range(3):
    hamburger.append(int(input()))
for _ in range(2):
    drink.append(int(input()))

print((min(hamburger) + min(drink)) - 50)