# 클래스를 이용하여 n! = 1 X 2 X ... X n-1 X n을 구하는 프로그램 작성
class Fectorial:
    def __init__(self,n=0, total=1):
        self.n = n
        self.total = total

    def get_data(self):
        k = 1
        if(k != self.n):
            for i in range(self.n):
                self.total = self.total * k
                k += 1
        print(self.total)


fec1 = Fectorial()
n = int(input("n값을 입력하시오: "))

num = Fectorial(n)
num.get_data()

