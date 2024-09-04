#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 사용할 파일 이름 정의(과제 pdf에 csim.trace를 이용하여 이것을 사용하였습니다.
#define filename "csim.trace"

// 캐시 블록 구조체 정의
struct cache_block {
    int tag;    // 태그 필드
    int valid;  // 유효 비트
    int time;   // LRU를 위한 시간 필드
    int dirty;  // 쓰기 되돌림을 위함. (캐시 메모리에 수정되었지만 주기억장치에 반영되지 않았을 때)
};

/* 전역 변수 */
int total_loads, total_stores; // 총 로드 및 스토어 수
int load_hits, load_misses;    // 로드 히트 및 미스 수
int store_hits, store_misses;  // 스토어 히트 및 미스 수
int total_cycles;              // 총 사이클 수

int time_count; // LRU를 구현하기 위한 시간

struct cache_block **cache; // 캐시 블록을 가리키는 이중 포인터

/* 함수 목록 */
void simulation(int c_size, int b_size, int assoc, char* write_allocate, char* write_policy, char* eviction_policy);
void load_data(int addr, int c_size, int b_size, int assoc, char* write_allocate, char* write_policy, char* eviction_policy);
void store_data(int addr, int c_size, int b_size, int assoc, char* write_allocate, char* write_policy, char* eviction_policy);
int evict(int set, int assoc, char* eviction_policy);
void update_time(int set, int assoc, int index);

int main(int argc, char *argv[]) {
    if (argc != 7) { // 명령줄 인수가 7개가 아닌 경우 사용법 출력
        printf("Usage: %s <cache size> <block size> <associativity> <write-allocate> <write-policy> <eviction-policy>\n", argv[0]);
        return 1;
    }

    int cache_size = atoi(argv[1]); // 캐시 크기
    int block_size = atoi(argv[2]); // 블록 크기
    int associativity = atoi(argv[3]); // 연관도
    char *write_allocate = argv[4]; // 쓰기 할당
    char *write_policy = argv[5]; // 쓰기 정책(write-through or write-back 둘중 하나 선택을 위함)
    char *eviction_policy = argv[6]; // 제거 정책(LRU or FIFO or RANDOM제거 선택을 위함)

    simulation(cache_size, block_size, associativity, write_allocate, write_policy, eviction_policy); // 시뮬레이션 실행

    return 0;
}

/* 시뮬레이션 함수 */
void simulation(int c_size, int b_size, int assoc, char* write_allocate, char* write_policy, char* eviction_policy) {
    /* 전역변수 값 초기화 */
    total_loads = total_stores = 0;
    load_hits = load_misses = 0;
    store_hits = store_misses = 0;
    total_cycles = 0;
    time_count = 0;

    int mode, addr; /* 파일로부터 읽은 값을 저장하는 변수 */
    int num_of_sets = c_size / (b_size * assoc); // 각 캐시의 세트 수

    FILE *fp = NULL;

    // 캐시 블록 초기화
    cache = (struct cache_block**)calloc(num_of_sets, sizeof(struct cache_block*));
    for (int i = 0; i < num_of_sets; i++) {
        cache[i] = (struct cache_block*)calloc(assoc, sizeof(struct cache_block));
    }

    fp = fopen(filename, "r"); // 파일 열기
    if (!fp) { // 파일 열기에 실패한 경우
        perror("Failed to open file");
        exit(1);
    }

    /* 파일로부터 mode와 주소를 읽어 각 함수에 전달한다. */
    while (fscanf(fp, " %c %x\n", (char*)&mode, &addr) != EOF) {
        if (mode == 'l') { // 로드 명령인 경우
            load_data(addr, c_size, b_size, assoc, write_allocate, write_policy, eviction_policy);
            total_loads++;
        } else if (mode == 's') { // 스토어 명령인 경우
            store_data(addr, c_size, b_size, assoc, write_allocate, write_policy, eviction_policy);
            total_stores++;
        }
        time_count++; /* 하나의 명령을 수행할 때마다 시간을 늘린다. */
    }

    for (int i = 0; i < num_of_sets; i++) {
        free(cache[i]);
    }
    free(cache);
    fclose(fp); // 파일 닫기

    // 결과 출력
    printf("Total loads: %d\n", total_loads);
    printf("Total stores: %d\n", total_stores);
    printf("Load hits: %d\n", load_hits);
    printf("Load misses: %d\n", load_misses);
    printf("Store hits: %d\n", store_hits);
    printf("Store misses: %d\n", store_misses);
    printf("Total cycles: %d\n", total_cycles);
}

/* 로드 데이터 함수 */
void load_data(int addr, int c_size, int b_size, int assoc, char* write_allocate, char* write_policy, char* eviction_policy) {
    int num_of_sets = c_size / (b_size * assoc); // 세트의 개수
    int set_index = (addr / b_size) % num_of_sets; // 입력받은 주소로부터 세트 인덱스를 구함
    int tag = (addr / b_size) / num_of_sets; // 태그를 구함
    struct cache_block *set = cache[set_index]; // 해당 세트를 가리킴

    // 캐시 히트를 확인
    for (int i = 0; i < assoc; i++) {
        if (set[i].valid && set[i].tag == tag) { // 캐시 히트인 경우
            load_hits++;
            total_cycles++; // 캐시 접근 시간
            update_time(set_index, assoc, i); // 시간 업데이트
            return;
        }
    }

    // 캐시 미스인 경우
    load_misses++;
    total_cycles += 100 * (b_size / 4); // 메모리 접근 시간 추가

    // 빈 슬롯을 찾기
    for (int i = 0; i < assoc; i++) {
        if (!set[i].valid) { // 빈 슬롯인 경우
            set[i].valid = 1;
            set[i].tag = tag;
            set[i].time = time_count;
            return;
        }
    }

    // 빈 슬롯이 없는 경우 교체
    int evict_index = evict(set_index, assoc, eviction_policy);
    if (set[evict_index].dirty) { // 더티 비트가 설정된 경우 메모리에 쓰기
        total_cycles += 100 * (b_size / 4);
    }
    set[evict_index].valid = 1;
    set[evict_index].tag = tag;
    set[evict_index].time = time_count;
    set[evict_index].dirty = 0; // 로드이므로 더티 비트는 0
}

/* 스토어 데이터 함수 */
void store_data(int addr, int c_size, int b_size, int assoc, char* write_allocate, char* write_policy, char* eviction_policy) {
    int num_of_sets = c_size / (b_size * assoc); // 세트의 개수
    int set_index = (addr / b_size) % num_of_sets; // 입력받은 주소로부터 세트 인덱스를 구함
    int tag = (addr / b_size) / num_of_sets; // 태그를 구함
    struct cache_block *set = cache[set_index]; // 해당 세트를 가리킴

    // 캐시 히트를 확인
    for (int i = 0; i < assoc; i++) {
        if (set[i].valid && set[i].tag == tag) { // 캐시 히트인 경우
            store_hits++;
            total_cycles++; // 캐시 접근 시간
            if (strcmp(write_policy, "write-back") == 0) { //write-back
                set[i].dirty = 1;
            } else { // write-through
                total_cycles += 100; // 메모리에 즉시 쓰기
            }
            update_time(set_index, assoc, i); // 시간 업데이트
            return;
        }
    }

    // 캐시 미스인 경우
    store_misses++;
    total_cycles += 100 * (b_size / 4); // 메모리 접근 시간 추가

    if (strcmp(write_allocate, "write-allocate") == 0) {
        // 빈 슬롯을 찾기
        for (int i = 0; i < assoc; i++) {
            if (!set[i].valid) { // 빈 슬롯인 경우
                set[i].valid = 1;
                set[i].tag = tag;
                set[i].time = time_count;
                if (strcmp(write_policy, "write-back") == 0) {
                    set[i].dirty = 1;
                } else { // write-through일 경우
                    total_cycles += 100; // 메모리에 즉시 쓰기
                }
                return;
            }
        }

        // 빈 슬롯이 없는 경우 교체
        int evict_index = evict(set_index, assoc, eviction_policy);
        if (set[evict_index].dirty) { // 더티 비트가 설정된 경우 메모리에 쓰기(캐시 메모리에 수정되었지만 주기억장치에 반영되지 않았을 때 메모리에 쓰기)
            total_cycles += 100 * (b_size / 4);
        }
        set[evict_index].valid = 1;
        set[evict_index].tag = tag;
        set[evict_index].time = time_count;
        if (strcmp(write_policy, "write-back") == 0) { // write-back인 경우
            set[evict_index].dirty = 1;
        } else { // write-through인 경우
            total_cycles += 100; // 메모리에 즉시 쓰기
        }
    } else {
        total_cycles += 100; // write-allocate가 아닐 경우 메모리에 바로 쓰기
    }
}

/* 캐시 블록을 교체할 인덱스를 결정하는 함수 */
int evict(int set, int assoc, char* eviction_policy) {
    int evict_index = 0;
    if (strcmp(eviction_policy, "lru") == 0) { // LRU
        int min_time = cache[set][0].time;
        for (int i = 1; i < assoc; i++) {
            if (cache[set][i].time < min_time) {
                min_time = cache[set][i].time;
                evict_index = i;
            }
        }
    } else if (strcmp(eviction_policy, "fifo") == 0) { // FIFO
        int min_time = cache[set][0].time;
        for (int i = 1; i < assoc; i++) {
            if (cache[set][i].time < min_time) {
                min_time = cache[set][i].time;
                evict_index = i;
            }
        }
    } else if (strcmp(eviction_policy, "random") == 0) { // 랜덤
        evict_index = rand() % assoc;
    }
    return evict_index;
}

/* 시간 업데이트 함수 */
void update_time(int set, int assoc, int index) {
    cache[set][index].time = time_count;
}
