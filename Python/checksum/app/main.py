import sys, os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from src.parse_generator import getGeneratorBits, getDataBits
from src.checksum_calculator import calculateChecksum

def main():
    print("=== 다항 체크섬 계산기 ===")

    polyStr = input("생성다항식을 입력하세요 (예: x5 + x2 + 1): ")
    dataStr = input("원본 데이터를 입력하세요 (예: 101101001): ")

    generatorBits = getGeneratorBits(polyStr)
    dataBits = getDataBits(dataStr)
    checksum = calculateChecksum(dataBits, generatorBits)

    print("출력 결과 :", *checksum)


if __name__ == "__main__":
    main()