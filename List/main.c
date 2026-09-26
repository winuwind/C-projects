#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>


typedef struct list_t{
    struct list_t *prew;
    struct list_t *next;
    int value;
}list;


void around(list **Head1, list **Head2){
    if((*Head2)->next != NULL){
        around(Head1, &((*Head2)->next));
    }
    else{
        (*Head2)->next = (*Head1);
        (*Head1)->prew = (*Head2);
    }
}


void push(list **Head, int n){
    if((*Head)->next != NULL){
        push(&((*Head)->next), n);
    }
    else{
        list *help = (list*) malloc(sizeof(list));
        help->next = NULL;
        help->value = n;
        help->prew = (*Head);
        (*Head)->next = help;
    }
}


int pop_last(list **Head){
    int last = 0;
    if((*Head)->next != NULL) {
        if (((*Head)->next)->next == NULL) {
            last = ((*Head)->next)->value;
            free((*Head)->next);
            (*Head)->next = NULL;
        }
        else {
            last = pop_last(&((*Head)->next));
        }
    }
    return last;
}


void Print_list(list **Numbers){
    printf("%d ", (*Numbers)->value);
    if(((*Numbers)->next) != NULL){
        Print_list(&(*Numbers)->next);
    }
}


void part1() {
    int N;
    scanf("%d", &N);
    list *Head = (list*) malloc(sizeof(list));
    Head->next = NULL;
    Head->value = 0;
    /*for(int i = 0; i < N; i++){
        Push(&Head, i);
    }*/
    for(int i = 1; i < N; i++){
        push(&Head, i);
    }
    Print_list(&Head);
    printf("\n");
    while(Head->next != NULL){
        N = pop_last(&Head);
        printf("%d ", N);
    }
    printf("%d", Head->value);
    free(Head);
}


list* Task5(list **Head, int k){
    list *help;
    while(k-- > 1){
        (*Head) = (*Head)->next;
    }
    help = (*Head)->next;
    (*Head)->next = (*Head)->next->next;
    free(help);
    return (*Head)->next;;
}


list* part3(list **Head, int k){
    list *help;
    while(k-- > 1){
        (*Head) = (*Head)->next;
    }
    (*Head)->prew->next = (*Head)->next;
    help = (*Head);
    (*Head)->next->prew = (*Head)->prew;
    (*Head) = (*Head)->next;
    free(help);
    return (*Head);
}


int main(){
    int number;
    scanf("%d", &number);
    if(number == 1){
        part1();
        return 0;
    }
    int N, K;
    scanf("%d %d", &N, &K);
    if(K == 1){
        printf("%d", N);
        return 0;
    }
    else if(K <= 0){
        printf("K must be >= 1");
        return 0;
    }
    list *Head = (list*) malloc(sizeof(list));
    Head->next = NULL;
    Head->value = 1;
    Head->prew = NULL;
    for(int i = 2; i <= N; i++){
        push(&Head, i);
    }
    around(&Head, &Head);
    if(number == 3){
        while(N > 1){
            Head = part3(&Head, K);
            N--;
        }
        printf("%d", Head->value);
        return 0;
    }
    while(N != 1){
        Head = Task5(&Head, K - 1);
        N--;
    }
    int answer = Head->value;
    printf("%d", answer);
    return 0;
}