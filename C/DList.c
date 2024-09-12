#include <stdio.h>
#include <stdlib.h>

typedef int element;
typedef struct DListNode {  // 이중연결 노드 타입
    element data;
    struct DListNode* llink;
    struct DListNode* rlink;
} DListNode;

// 이중 연결 리스트를 초기화
void init(DListNode* phead)
{
    phead->llink = NULL;  // phead -> NULL 
    phead->rlink = NULL;  // phead -> NULL 
}

// 이중 연결 리스트의 노드를 출력
void print_dlist(DListNode* phead)
{
    DListNode* p;
    // for (p = phead->rlink; p != phead; p = p->rlink)
    for (p = phead->rlink; p != NULL; p = p->rlink) {  
        printf("<-| |%d| |-> ", p->data);
    }
    printf("\n");
}
// 새로운 데이터를 노드 before의 오른쪽에 삽입한다.
void dinsert(DListNode *before, element search, element input)
{
    DListNode *newnode = (DListNode *)malloc(sizeof(DListNode));
    newnode->data = input;
    int i = 0;

    while (before->rlink != NULL) { //노드 찾기
            if (before->data == search){
                i = 1;
                break;
            }
            before = before->rlink;
        }
    if(i == 1){     //찾고자 하는 값을 찾았을 때
        newnode->llink = before;
        newnode->rlink = before->rlink; 
        if(before->rlink != NULL) before->rlink->llink = newnode;
        before->rlink = newnode;
    }
        
    if(i == 0) {    //(before->rlink == NULL), 즉 못 찾았을 때
        newnode->llink = before;
        newnode->rlink = NULL;
        before->rlink = newnode;
    }
}
// 노드 removed를 삭제한다.
void ddelete(DListNode* head, DListNode* removed)
{
    if (removed == head) return;
    removed->llink->rlink = removed->rlink;
    removed->rlink->llink = removed->llink;
    free(removed);
}

// 이중 연결 리스트 테스트 프로그램
int main(void)
{
    DListNode* head = (DListNode *)malloc(sizeof(DListNode));
    init(head);
    int input;
    int search;
    printf("추가 단계\n");

    for (int i = 0; i < 5; i++) {
        // 헤드 노드의 오른쪽에 삽입
        printf("찾는 값과 추가할 값: ");
        scanf("%d%d", &search, &input);
        dinsert(head, search, input);
        print_dlist(head);
    }
    printf("\n삭제 단계\n");
    for (int i = 0; i < 5; i++) {
        print_dlist(head);
        ddelete(head, head->rlink);
    }
    free(head);
    return 0;
}
