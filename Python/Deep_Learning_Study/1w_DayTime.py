import matplotlib.pyplot as plt

lectures = dict()

for _ in range(6):
    day = input('요일 :')
    time = int(input('시간 :' ))
    lectures[day] = time

plt.bar(lectures.keys(), lectures.values())
plt.xlabel("Days")
plt.ylabel("Time (in hours)")
plt.title("Number of lectures by day of the week")
plt.show()