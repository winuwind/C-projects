#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

#ifdef __unix__
    #define OS_Windows 0
#elif defined(_WIN32) || defined(WIN32)
    #define OS_Windows 1
#endif

void foo(char* path, char* path_new, unsigned len_path, unsigned len_path_new, unsigned counter){
//    printf("start foo with path = %s\n", path);

    //Создаю новый путь (перевернутый)

    char* path_new_ = (char*) malloc(sizeof(char) * (len_path + 1));
    if(path_new_ == NULL){
//        fprintf(stderr, "can't malloc memory\n");
        return;
    }
    for(unsigned i = 0; i < len_path_new; i++){
        path_new_[i] = path_new[i];
    }
    if(len_path_new != 0) {
        path_new_[len_path_new] = '/';
        for (unsigned i = 1; i < len_path - len_path_new - counter; i++) {
            path_new_[len_path_new + i] = path[len_path - i];
        }
        path_new_[len_path - counter] = 0;
    }
    else{
        for(unsigned i = 0; i < len_path; i++){
            if(i == len_path - 1){
                break;
            }
            if(path[i] == '/'){
                counter = i + 1;
            }
        }
        for (unsigned i = 1; i <= len_path - counter; i++) {
            path_new_[i - 1] = path[len_path - i];
        }
        path_new_[len_path - counter] = 0;
    }

    //Узнаю какой это файл

    struct stat st;
    char type;
    if(stat(path, &st) == -1){
//        fprintf(stderr, "can't get info about %s\n\n", path);
        return;
    }
    switch (st.st_mode & S_IFMT) {
        case(S_IFDIR): type = 'd';
            break;
        case(S_IFREG): type = 'f';
            break;
        default: type = '?';
            break;
    }
//    printf("type: %c\n\n", type);

//Если регулярный

    if(type == 'f'){
        FILE *fp = fopen(path, "rb");
        FILE *fp_ = fopen(path_new_, "wb");
        if(fp_ == NULL){
//            fprintf(stderr, "can't create/open file %s\n", path_new_);
            return;
        }

        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        size_t count = 0;
        int bufSize = 100;
        char* str = (char*) malloc(sizeof(char) * bufSize);
        char* str_reverse = (char*) malloc(sizeof(char) * bufSize);
        for(size_t i = 0; i < size; i += bufSize){
            fseek(fp, size - i, SEEK_SET);
            count = fread(str, sizeof(char), bufSize, fp);
            for(size_t j = 0; j < count; j++){
                str_reverse[j] = str[count - j - 1];
            }
            fwrite(str_reverse, sizeof(char), count, fp_);
            count = i;
        }
        if(size - count != 0){
            fseek(fp, 0, SEEK_SET);
            count = fread(str, sizeof(char), size - count, fp);
            for(size_t j = 0; j < count; j++){
                str_reverse[j] = str[count - j - 1];
            }
            fwrite(str_reverse, sizeof(char), count, fp_);
        }
        fclose(fp);
        fclose(fp_);
        free(str);
        free(str_reverse);
    }

    //Если директория

    else if(type == 'd'){
        //По новому пути хочу создать директорию с перевернутым именем - проверяю, не существует ли уже случайно такого файла или директории
        struct stat st_;
        char type_;
        if(stat(path_new_, &st_) == -1){
//            fprintf(stderr, "can't get info about %s\n\n", path_new_);
            type_ = 'e';
        }
        else {
//            printf("if type is directory: %s\n\n", path_new_);
            switch (st_.st_mode & S_IFMT) {
                case (S_IFDIR):
                    type_ = 'd';
                    break;
                case (S_IFREG):
                    type_ = 'f';
                    break;
                default:
                    type_ = '?';
                    break;
            }
//            printf("type of %s = %c\n\n", path_new_, type_);
        }
        if(type_ != 'e'){
//            fprintf(stderr, "Can't create directory %s because a file with same name exist\n", path_new_);
        }

        //Создаю директорию, если не существует
        else {
            if(OS_Windows == 1) {
                char *command = (char *) malloc(sizeof(char) * (8 + len_path));
                command[0] = 'm';
                command[1] = 'k';
                command[2] = 'd';
                command[3] = 'i';
                command[4] = 'r';
                command[5] = ' ';
                for (unsigned int i = 0; i < len_path + 2; i++) {
                    command[i + 6] = path_new_[i];
                }
//                if (system(command) != 0) {
////                    fprintf(stderr, "%s\n%s\n", path_new_, command);
////                    fprintf(stderr, "Problem creating directory '%s'\n", path_new_);
//                    free(command);
//                    return;
//                }
                free(command);
            }
            else{
//                printf("mkdir %s\n", path_new_);
                if(mkdir(path_new_, 0777) != 0){
                    perror("can't create directory");
                    return;
                }
            }
        }
        //Открываю директорию и паршу файлы в ней
        struct dirent* dp;
        DIR *dir = opendir(path);
        if(dir == NULL){
            return;
        }
        while ((dp = readdir(dir)) != NULL) {
//            printf("in dir: %s\n", dp->d_name);
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                continue;
            }
            char* path_ = (char*) malloc(sizeof(char) * (strlen(dp->d_name) + 2 + len_path));
            if(path_ == NULL){
//                fprintf(stderr, "can't malloc memory\n");
                return;
            }
            for (unsigned int i = 0; i < len_path; i++) {
                path_[i] = path[i];
            }
            path_[len_path] = '/';
            for (unsigned int i = 0; i < strlen(dp->d_name); i++) {
                path_[i + 1 + len_path] = dp->d_name[i];
            }
            path_[strlen(dp->d_name) + 1 + len_path] = 0;
            //рекурсивно перехожу к следующему файлу в директории
            foo(path_, path_new_, len_path + 1 + strlen(dp->d_name), len_path - counter, counter);
            free(path_);
        }
        closedir(dir);
    }
    free(path_new_);
}

int main(int argc, char* argv[]) {
    char* path = "/etc";
    if(argc > 1){
        path = argv[1];
    }
    if(path[strlen(path)] == '/'){
        path[strlen(path)] = 0;
    }
    if(strlen(path) == 0){
        printf("can't work with root dir\n");
        return 0;
    }
    unsigned len_path = strlen(path);
    foo(path, NULL, len_path, 0, 0);
    return 0;
}
