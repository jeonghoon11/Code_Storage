def divideMod2(dividend, divisor):
    if len(divisor) < 2 or divisor[0] != 1:
        raise ValueError("생성다항식의 최고차항은 1이어야 합니다.")

    work = dividend[:]
    n = len(divisor)
    i = 0

    while i <= len(work) - n:
        if work[i] == 1:
            for j in range(n):
                work[i + j] = work[i + j] ^ divisor[j]
        i += 1

    return work[-(n - 1):]


def calculateChecksum(dataBits, generatorBits):
    degree = len(generatorBits) - 1
    augmented = dataBits + [0] * degree
    return divideMod2(augmented, generatorBits)