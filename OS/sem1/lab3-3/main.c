#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

unsigned long long toLongLong(char* str){
    unsigned long long x = 0;
    char y[2];
    y[1] = 0;
    for(unsigned i = 0; i < strlen(str); i++){
        y[0] = str[i];
        x = x * 16 + (unsigned long long) strtoll(y, NULL, 16);
    }
    return x;
}

void getAddress(char* line, char* start, char* end){
    unsigned i = 0;
    unsigned line_len = strlen(line);
    for(; i < line_len && line[i] != '-'; i++){
        start[i] = line[i];
    }
    start[i++] = 0;
    unsigned k = i;
    for(; i < line_len && line[i] != ' '; i++){
        end[i - k] = line[i];
    }
    end[i - k] = 0;
}

void getName(char* line, char* name){
    unsigned int index_of_last_space = 0;
    unsigned line_len = strlen(line);
    for (unsigned int i = 0; i < line_len && line[i] != '/'; i++) {
        if (line[i] == ' ') {
            index_of_last_space = i;
        }
    }
    const unsigned int name_len = line_len - index_of_last_space;
    for (unsigned int i = 0; i < name_len; i++) {
        name[i] = line[i + index_of_last_space];
    }
    name[name_len] = '\0';
}

int getData(FILE *fp, unsigned long long first, unsigned long long count, char* name){
    if(fseek(fp, first * 8, SEEK_SET) != 0){
        fprintf(stderr, "FSEEK error\n");
        return -1;
    }
    unsigned long long data = 0;
    for(long long j = 0; j < count; j++){
        if(fread(&data, sizeof(data), 1, fp) != 1){
            fprintf(stderr, "FREAD error\n");
            return -1;
        }
        unsigned long long present = data >> 63 ? 1 : 0;
        unsigned long long swapped = data >> 62 ? 1 : 0;
        unsigned long long file_mapped_page = data >> 61 ? 1 : 0;
        unsigned long long protection = data >> 57 ? 1 : 0;
        unsigned long long exclusively_mapped = data >> 56 ? 1 : 0;
        unsigned long long soft_dirty = data >> 55 ? 1 : 0;
        unsigned long long physical_page_index = data & ((((unsigned long long) 1) << 54) - 1);
        printf("%#-16llx           %llu          %llu         %llu          %llu            %llu             %llu                %#-16llx              %s",
               first + j, present, swapped, file_mapped_page, protection, exclusively_mapped, soft_dirty, physical_page_index, name);
    }
    return 0;
}

int main(void) {
    FILE* maps = fopen("/proc/self/maps", "r");
    if(maps == NULL){
        fprintf(stderr, "Error when opening a file /proc/self/maps\n");
        return -1;
    }
    FILE *pagemap = fopen("/proc/self/pagemap", "r");
    if(pagemap == NULL){
        fprintf(stderr, "Error when opening a file /proc/self/pagemap\n");
        fclose(maps);
        return -1;
    }
    char line[1025];
    char name[257];
    char start_address[17];
    char end_address[17];
    const long long page_size = sysconf(_SC_PAGESIZE);
    printf("PAGE                    PRESENT    SWAPPED    FILE    PROTECTION   EXCLUSIVELY    SOFT_DIRTY      PHYSICAL_PAGE"
           "                       REGION\n");
    while(fgets(line, 1024, maps)){
        getAddress(line, start_address, end_address);
        getName(line, name);
        unsigned long long start = toLongLong(start_address);
        unsigned long long end = toLongLong(end_address);
        unsigned long long first_page = start / page_size;
        unsigned long long last_page = end / page_size;
        unsigned long long count = last_page - first_page;
        if(getData(pagemap, first_page, count, name) != 0){
            printf("Data print error\n");
            fclose(maps);
            fclose(pagemap);
            return -1;
        }
    }
    fclose(maps);
    fclose(pagemap);
    return 0;
}
