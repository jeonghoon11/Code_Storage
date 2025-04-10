class LRU:
    def __init__(self, frame_count):
        self.frame_count = frame_count
        self.frames = [None] * frame_count
        self.usage = []
        self.history = [[] for _ in range(frame_count)]

    def access_page(self, page):
        if page in self.frames:
            self.usage.remove(page)
            self.usage.append(page)
        else:
            if None in self.frames:
                empty_index = self.frames.index(None)
                self.frames[empty_index] = page
            else:
                lru_page = self.usage.pop(0)
                lru_index = self.frames.index(lru_page)
                self.frames[lru_index] = page
            self.usage.append(page)
        for i in range(self.frame_count):
            self.history[i].append(self.frames[i] if self.frames[i] else ' ')

    def display(self):
        print("\n===== 캐시 상태 =====")
        for i in range(self.frame_count):
            print(f"Page Frame {i + 1}: ", end="")
            for val in self.history[i]:
                print(f"{val} ", end="")
            print()

frames = int(input("Page Frame 수 입력: "))
sequence = input("페이지 요청 시퀀스 입력 (예: A B C A B): ").split()

sim = LRU(frames)
for page in sequence:
    sim.access_page(page)

sim.display()