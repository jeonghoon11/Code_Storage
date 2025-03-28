class Block:
    def __init__(self,id,size):
        self.id = id
        self.size = size

memoryBlocks = [
    Block(0, 99999),
    Block(1, 105),
    Block(2, 5),
    Block(3, 600),
    Block(4, 20),
    Block(5, 205),
    Block(6, 4050),
    Block(7, 230),
    Block(8, 1000),
]

def bestFit(blocks, jobSize):
    wasteMemory = next(block for block in memoryBlocks if block.id == 0).size - jobSize
    FirstWM = wasteMemory
    subscript = 0
    counter = 1
    
    while counter < len(memoryBlocks):
        if jobSize > memoryBlocks[counter].size:
            counter += 1
        else:
            wasteMemory = memoryBlocks[counter].size - jobSize
            if FirstWM > wasteMemory :
                subscript = counter
                FirstWM = wasteMemory
            counter +=1

    if subscript == 0:
        print("작업을 일시적으로 기다립니다.")
    else:
        memoryBlocks[subscript].size -= jobSize
        print(f"작업이 블록 {memoryBlocks[subscript].id}에 할당되었습니다.")
        print(f"블록 사이즈 : {memoryBlocks[subscript].size}")

jobSize = int(input("작업 크기 입력: "))
bestFit(memoryBlocks, jobSize)