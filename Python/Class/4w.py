resent_time = int(input("현재 시간을 입력하시오: (예:1230 12시 30분)"))
next_class = int(input("다음 수업시간을 입력하시오:"))
money = int(input("현재 가진 돈을 입력하시오:"))

time = next_class - resent_time


if time < 15 and money > 6000:
    print("걸어서 학교로 갑니다.")

else:
    print("택시타고 학교갑니다.")