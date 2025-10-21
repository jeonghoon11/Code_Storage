def getGeneratorBits(polyStr):
    s = polyStr.replace(" ", "")
    terms = s.split("+")
    degrees = []

    for term in terms:
        if term == "1":
            degrees.append(0)
            continue

        if term == "x":
            degrees.append(1)
        elif term.startswith("x"):
            degrees.append(int(term[1:]))
        else:
            raise ValueError("변수는 x만 허용됩니다: " + term)

    maxDegree = max(degrees)
    bits = [0] * (maxDegree + 1)
    for d in degrees:
        bits[maxDegree - d] = 1
    return bits


def getDataBits(bitStr):
    bitStr = bitStr.strip()
    if not bitStr:
        raise ValueError("빈 데이터 문자열입니다.")
    for c in bitStr:
        if c not in "01":
            raise ValueError("데이터는 0/1만 허용됩니다.")
    return [int(c) for c in bitStr]