#include <stdio.h>
#include <malloc.h>
#include <math.h>


typedef struct tree_t{
    int data;
    int level;
    struct tree_t *left, *right, *root;
}tree;


tree* Creat_tree(int x){
    tree* Tree = (tree*) malloc(sizeof(tree));
    Tree->data = x;
    Tree->level = 0;
    Tree->right = NULL;
    Tree->left = NULL;
    return Tree;
}


int Tree_size(tree **Tree){
    if((*Tree) == NULL){
        return 0;
    }
    return 1 + (int) fmax(Tree_size(&((*Tree)->right)), Tree_size(&((*Tree)->left)));
}


void Destroy_tree(tree **Tree){
    if((*Tree)->left != NULL){
        Destroy_tree(&((*Tree)->left));
        (*Tree)->left = NULL;
    }
    if((*Tree)->right != NULL){
        Destroy_tree(&((*Tree)->right));
        (*Tree)->right = NULL;
    }
    free((*Tree));
}


void Insert(tree **Tree, int x){
    if(x < (*Tree)->data && (*Tree)->left != NULL){
        Insert(&((*Tree)->left), x);
    }
    else if(x >= (*Tree)->data && (*Tree)->right != NULL){
        Insert(&((*Tree)->right), x);
    }
    else if(x < (*Tree)->data && (*Tree)->left == NULL){
        tree *Help = (tree*) malloc(sizeof(tree));
        Help->data = x;
        Help->level = (*Tree)->level + 1;
        Help->left = NULL;
        Help->right = NULL;
        Help->root = *Tree;
        (*Tree)->left = Help;
    }
    else if(x >= (*Tree)->data && (*Tree)->right == NULL){
        tree *Help = (tree*) malloc(sizeof(tree));
        Help->data = x;
        Help->level = (*Tree)->level + 1;
        Help->left = NULL;
        Help->right = NULL;
        Help->root = *Tree;
        (*Tree)->right = Help;
    }
}


void Insert_raw(tree **root, int x, int direction){
    if(direction == 0 && (*root)->left != NULL){
        (*root)->left->data = x;
    }
    else if(direction != 0 && (*root)->right != NULL){
        (*root)->right->data = x;
    }
    else {
        tree *Help = (tree *) malloc(sizeof(tree));
        Help->data = x;
        Help->level = (*root)->level + 1;
        Help->left = NULL;
        Help->right = NULL;
        Help->root = *root;
        if (direction == 0) {
            (*root)->left = Help;
        } else {
            (*root)->right = Help;
        }
    }
}


tree** Find_node_not_sorted(tree** Tree, int x){
    tree** Help = NULL;
    if(x == (*Tree)->data){
        return Tree;
    }
    if((*Tree)->left != NULL){
        Help = Find_node_not_sorted(&((*Tree)->left), x);
    }
    if((*Tree)->right != NULL && Help == NULL){
        Help = Find_node_not_sorted(&((*Tree)->right), x);
    }
    return Help;
}


tree** Find_node(tree** Tree, int x){
    tree** Help;
    if(x == (*Tree)->data){
        return Tree;
    }
    else if((x < (*Tree)->data && (*Tree)->left == NULL) || (x > (*Tree)->data && (*Tree)->right == NULL)){
        return NULL;
    }
    else if(x < (*Tree)->data){
        Help = Find_node(&((*Tree)->left), x);
    }
    else{
        Help = Find_node(&((*Tree)->right), x);
    }
    return Help;
}


void Print_Tab(int count){
    while(count--){
        printf("\t");
    }
}


void Print_root(tree* root, int size){
    if(root->right != NULL){
        Print_root(root->right, size);
    }
    /*else{
        int x = (int) pow(2, size - root->level);
        while(x-- > 1){
            printf("\n");
        }
    }*/
    Print_Tab(root->level);
    printf("%d\n", root->data);
    if(root->left != NULL){
        Print_root(root->left, size);
    }
    /*else{
        int x = (int) pow(2, size - root->level);
        while(x-- > 1){
            printf("\n");
        }
    }*/
}


int main() {
    int n;
    scanf("%d", &n);
    tree* Tree = Creat_tree(n / 2);
    tree** node;
    for(int i = 0; i < n; i++){
        if(i == n / 2){
            continue;
        }
        Insert(&Tree, i);
    }
    Insert_raw(&(Tree->left), 10, 0);
    for(int i = -2; i < n + 2; i++){
        node = Find_node_not_sorted(&Tree, i);
        if(node == NULL){
            printf("data %d not stored in Tree\n", i);
        }
        else if((*node)->data != i){
            printf("the returned nose contains %d instead %d\n", (*node)->data, i);
        }
        else{
            printf("data %d stored in Tree\n", (*node)->data);
        }
    }
    int size = Tree_size(&Tree);
    printf("size: %d\n", size);
    printf("Tree:\n");
    Print_root(Tree, Tree_size(&Tree));
    Destroy_tree(&Tree);
    return 0;
}
