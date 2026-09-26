#include <stdio.h>
#include <malloc.h>


typedef struct Tree{
    struct Tree *right, *left;
    int value, high;
}Tree_t;


void Small_turn_right(Tree_t** Tree){
    Tree_t* Help = (*Tree)->right;
    (*Tree)->right = Help->left;
    Help->left = *Tree;
    if((*Tree)->left != NULL && (*Tree)->right != NULL) {
        (*Tree)->high = ((*Tree)->right->high > (*Tree)->left->high ? (*Tree)->right->high : (*Tree)->left->high) + 1;
    }
    else if((*Tree)->left == NULL && (*Tree)->right != NULL) {
        (*Tree)->high = (*Tree)->right->high + 1;
    }
    else if((*Tree)->left != NULL && (*Tree)->right == NULL) {
        (*Tree)->high = (*Tree)->left->high + 1;
    }
    else{
        (*Tree)->high = 1;
    }
    if(Help->left != NULL && Help->right != NULL) {
        Help->high = (Help->right->high > Help->left->high ? Help->right->high : Help->left->high) + 1;
    }
    else if(Help->left == NULL && Help->right != NULL){
        Help->high = Help->right->high + 1;
    }
    else if(Help->left != NULL && Help->right == NULL){
        Help->high = Help->left->high + 1;
    }
    else{
        Help->high = 1;
    }
    *Tree = Help;
}


void Small_turn_left(Tree_t** Tree){
    Tree_t* Help = (*Tree)->left;
    (*Tree)->left = Help->right;
    Help->right = *Tree;
    if((*Tree)->left != NULL && (*Tree)->right != NULL) {
        (*Tree)->high = ((*Tree)->right->high > (*Tree)->left->high ? (*Tree)->right->high : (*Tree)->left->high) + 1;
    }
    else if((*Tree)->left == NULL && (*Tree)->right != NULL) {
        (*Tree)->high = (*Tree)->right->high + 1;
    }
    else if((*Tree)->left != NULL && (*Tree)->right == NULL) {
        (*Tree)->high = (*Tree)->left->high + 1;
    }
    else{
        (*Tree)->high = 1;
    }
    if(Help->left != NULL && Help->right != NULL) {
        Help->high = (Help->right->high > Help->left->high ? Help->right->high : Help->left->high) + 1;
    }
    else if(Help->left == NULL && Help->right != NULL){
        Help->high = Help->right->high + 1;
    }
    else if(Help->left != NULL && Help->right == NULL){
        Help->high = Help->left->high + 1;
    }
    else{
        Help->high = 1;
    }
    *Tree = Help;
}


void Big_turn_right(Tree_t** Tree){
    Small_turn_left(&((*Tree)->right));
    Small_turn_right(Tree);
}


void Big_turn_left(Tree_t** Tree){
    Small_turn_right(&((*Tree)->left));
    Small_turn_left(Tree);
}


void Insert(Tree_t **Tree, int x, Tree_t* Arr_tree, int* index){
    if(x < (*Tree)->value && (*Tree)->left != NULL){
        Insert(&((*Tree)->left), x, Arr_tree, index);
        if((*Tree)->high < (*Tree)->left->high + 1){
            (*Tree)->high = (*Tree)->left->high + 1;
        }
    }
    else if(x >= (*Tree)->value && (*Tree)->right != NULL){
        Insert(&((*Tree)->right), x, Arr_tree, index);
        if((*Tree)->high < (*Tree)->right->high + 1){
            (*Tree)->high = (*Tree)->right->high + 1;
        }
    }
    else if(x < (*Tree)->value && (*Tree)->left == NULL){
        (*Tree)->left = &Arr_tree[*index];
        (*index)++;
        (*Tree)->left->value = x;
        (*Tree)->left->high = 1;
        (*Tree)->left->left = NULL;
        (*Tree)->left->right = NULL;
        if((*Tree)->high < 2){
            (*Tree)->high = 2;
        }
    }
    else if(x >= (*Tree)->value && (*Tree)->right == NULL){
        (*Tree)->right = &Arr_tree[*index];
        (*index)++;
        (*Tree)->right->value = x;
        (*Tree)->right->high = 1;
        (*Tree)->right->left = NULL;
        (*Tree)->right->right = NULL;
        if((*Tree)->high < 2){
            (*Tree)->high = 2;
        }
    }
    int dif1, dif2;
    dif1 = ((*Tree)->right != NULL ? (*Tree)->right->high : 0) - ((*Tree)->left != NULL ? (*Tree)->left->high : 0);
    if((*Tree)->right != NULL) {
        dif2 = ((*Tree)->right->right != NULL ? (*Tree)->right->right->high : 0) - ((*Tree)->right->left != NULL ? (*Tree)->right->left->high : 0);
        if (dif1 == 2 && dif2 == 1) {
            Small_turn_right(Tree);
        }
        if(dif1 == 2 && dif2 == -1){
            Big_turn_right(Tree);
        }
    }
    if((*Tree)->left != NULL) {
        dif2 = ((*Tree)->left->right != NULL ? (*Tree)->left->right->high : 0) - ((*Tree)->left->left != NULL ? (*Tree)->left->left->high : 0);
        if (dif1 == -2 && dif2 == -1) {
            Small_turn_left(Tree);
        }
        if (dif1 == -2 && dif2 == 1) {
            Big_turn_left(Tree);
        }
    }
}


int main() {
    int N, value;
    if(scanf("%d", &N) != 1){
        return 0;
    }
    if(N == 0){
        printf("%d", 0);
        return 0;
    }
    Tree_t *Arr_tree = (Tree_t*) malloc(sizeof(Tree_t) * N);
    Tree_t *Tree = &Arr_tree[0];
    int index = 1;
    Tree->high = 1;
    Tree->right = NULL;
    Tree->left = NULL;
    for(int i = 0; i < N; i++){
        if(scanf("%d", &value) != 1){
            return 0;
        }
        if(i == 0){
            Tree->value = value;
            continue;
        }
        Insert(&Tree, value, Arr_tree, &index);
    }
    printf("%d", Tree->high);
    free(Arr_tree);
    return 0;
}
