#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void create_dir(char* name){
    if(mkdir(name, 700) != 0){
        fprintf(stderr, "can't create dir with name = %s\n", name);
    }
}

void ls(char* name){
    struct dirent* dp;
    DIR *dir = opendir(name);
    if(dir == NULL){
        fprintf(stderr, "can't open dir with name = %s\n", name);
        return;
    }
    while ((dp = readdir(dir)) != NULL){
        printf("%s\n", dp->d_name);
    }
}

void delete_dir(char* name){
    if(rmdir(name) != 0) {
        fprintf(stderr, "can't delete dir with name = %s\n", name);
    }
}

void create_file(char* name){
    struct stat st;
    if(stat(name, &st) != -1){
        fprintf(stderr, "file with name = %s exist\n", name);
        return;
    }
    FILE *fp = fopen(name, "w");
    if (stat(name, &st) == -1){
        fprintf(stderr, "can't create file with name = %s\n", name);
    }
    fclose(fp);
}

void cat(char* name){
    struct stat st;
    if(stat(name, &st) == -1){
        fprintf(stderr, "can't open file with name = %s\n", name);
        return;
    }
    char type;
    switch (st.st_mode & S_IFMT) {
        case(S_IFDIR): type = 'd';
            break;
        case(S_IFREG): type = 'f';
            break;
        default: type = '?';
            break;
    }
    if(type != 'f'){
        fprintf(stderr, "%s is not a file\n", name);
        return;
    }
    FILE *fp = fopen(name, "rb");
    if(fp == NULL){
        fprintf(stderr, "can't open file = %s\n", name);
        return;
    }
    int x = 0;
    while(x != EOF){
        x = fgetc(fp);
        printf("%c", x);
    }
    fclose(fp);
}

void delete_file(char* name){
    if(remove(name) == 0){
        return;
    }
    fprintf(stderr, "can't delete file with name = %s\n", name);
}

void create_symbol_link(char* name, char* name_link){
    char file_name[256];
    if(name_link == NULL || strlen(name_link) == 0){
        sprintf(file_name, "%s_link", name);
    }
    else{
        sprintf(file_name, "%s", name_link);
    }
    if(symlink(name, file_name) != 0){
        fprintf(stderr, "can't create symbol link for file %s\n", name);
    }
}

void read_symbol_link(char* name){
    char buf[1024];
    int len = readlink(name, buf, sizeof(buf) - 1);
    if(len == -1){
        fprintf(stderr, "can't read symbol link with name = %s\n", name);
        return;
    }
    buf[len] = 0;
    printf("%s\n", buf);
}

void delete_link(char* name){
    if(unlink(name) != 0){
        fprintf(stderr, "can't delete link with name = %s\n", name);
    }
}

void create_hard_link(char* name, char* name_link){
    struct stat st;
    if(stat(name, &st) == -1){
        fprintf(stderr, "can't create link on file with name = %s\n", name);
        return;
    }
    char type;
    switch (st.st_mode & S_IFMT) {
        case(S_IFDIR): type = 'd';
            break;
        case(S_IFREG): type = 'f';
            break;
        default: type = '?';
            break;
    }
    if(type != 'f'){
        fprintf(stderr, "can't create a tough link to an irregular file with name = %s\n", name);
        return;
    }
    char file_name[256];
    if(name_link == NULL || strlen(name_link) == 0){
        sprintf(file_name, "%s_link", name);
    }
    else{
        sprintf(file_name, "%s", name_link);
    }
    if(link(name, file_name) != 0){
        fprintf(stderr, "can't create hard link for file %s\n", name);
    }
}

void print_mode(char* name){
    struct stat st;
    if(stat(name, &st) == -1){
        fprintf(stderr, "can't get info about file with name = %s\n", name);
        return;
    }

    if (st.st_mode & S_IRUSR) {
        printf("r");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IWUSR) {
        printf("w");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IXUSR) {
        printf("x");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IRGRP) {
        printf("r");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IWGRP) {
        printf("w");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IXGRP) {
        printf("x");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IROTH) {
        printf("r");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IWOTH) {
        printf("w");
    }
    else{
        printf("-");
    }
    if (st.st_mode & S_IXOTH) {
        printf("x");
    }
    else{
        printf("-");
    }
    printf("\nnumber of hard link on file %s is %lu\n", name, st.st_nlink);
}

void change_mode(char* name, int mode){
    if(chmod(name, mode) != 0){
        fprintf(stderr, "can't change mode for file with name = %s", name);
    }
}

int main(int argc, char* argv[]) {
    char* name;
    char* arg_2 = NULL;
    if(argc == 1){
        fprintf(stderr, "there are not enough arguments\n");
        return 0;
    }
    else if(argc == 2){
        name = argv[1];
    }
    else{
        name = argv[1];
        arg_2 = argv[2];
    }
    unsigned counter = 0;
    for(unsigned i = 0; i < strlen(argv[0]); i++){
        if(argv[0][i] == '/'){
            counter = i + 1;
        }
    }
    if(argv[0][counter] == 'a'){
        create_dir(name);
    }
    else if(argv[0][counter] == 'b'){
        ls(name);
    }
    else if(argv[0][counter] == 'c'){
        delete_dir(name);
    }
    else if(argv[0][counter] == 'd'){
        create_file(name);
    }
    else if(argv[0][counter] == 'e' || argv[0][counter] == 'i'){
        cat(name);
    }
    else if(argv[0][counter] == 'f'){
        delete_file(name);
    }
    else if(argv[0][counter] == 'g'){
        create_symbol_link(name, arg_2);
    }
    else if(argv[0][counter] == 'h'){
        read_symbol_link(name);
    }
//    else if(strcmp(&argv[0][counter], "i.out") == 0){
//        cat(name);
//    }
    else if(argv[0][counter] == 'j' || argv[0][counter] == 'l'){
        delete_link(name);
    }
    else if(argv[0][counter] == 'k'){
        create_hard_link(name, arg_2);
    }
//    else if(strcmp(&argv[0][counter], "l.out") == 0){
//        delete_link(name);
//    }
    else if(argv[0][counter] == 'm'){
        print_mode(name);
    }
    else if(argv[0][counter] == 'n'){
        int mode = 0;
        mode = strtol(arg_2, NULL, 8);
        change_mode(name, mode);
    }
}
