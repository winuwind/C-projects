#include <stdio.h>
#include <malloc.h>
#include <string.h>


typedef struct Data {
    int id, age;
    char Name[35], Surname[35], Phone[16];
    float duty;
} Data_t;


typedef struct dict_t {
    unsigned int key[256];
} dict;


unsigned int Search(char *str, char *pattern) {
    int len_p = (int) strlen(pattern), len_s = (int) strlen(str);
    dict letters = {.key = {0}};
    for (unsigned int i = 0; i < len_p - 1; i++) {
        letters.key[pattern[i]] = len_p - i - 1;
    }
    for (unsigned int i = 0; i < 256; i++) {
        if (letters.key[i] == 0) {
            letters.key[i] = len_p;
        }
    }
    for (int i = 0; i < len_s - len_p + 1; i++) {
        int index = len_p - 1, flag = 0;
        for (int j = len_p - 1; j >= 0 && pattern[j] == str[i + j]; j--) {
            index = j;
            flag = 1;
            if (j == 0) {
                break;
            }
        }
        if (flag == 0) {
            i += (int) letters.key[(int) str[i + index]] - 1;
        } else if (index > 0) {
            i += (int) letters.key[(int) str[i + len_p - 1]] - 1;
        } else {
            return i;
        }
    }
    return -1;
}


void Print_string_data(Data_t *String_data, FILE *fp) {
    if (fp == stdout) {
        fprintf(fp, "Id: %d; Name: %s; Surname: %s; Age: ", String_data->id, String_data->Name, String_data->Surname);
        if (String_data->age == -1) {
            fprintf(fp, "unknown; Phone: %s; Duty: ", String_data->Phone);
        } else {
            fprintf(fp, "%d; Phone: %s; Duty: ", String_data->age, String_data->Phone);
        }
        if (String_data->duty == -1.0) {
            fprintf(fp, "unknown");
        } else {
            fprintf(fp, "%f", String_data->duty);
        }
    } else {
        fprintf(fp, "Id: %d; Name: %s; Surname: %s; Age: %d; Phone: %s; Duty: %f", String_data->id, String_data->Name,
                String_data->Surname, String_data->age, String_data->Phone, String_data->duty);
    }
    fprintf(fp, "\n");
}


void Add_data(Data_t *Arr_data, int *count) {
    if(!Arr_data){
        printf("Function Add_data: Arr_data is null\n");
        return;
    }
    char Value[35];
    printf("Name: (if unknown, then enter: unknown)>");
    if (fgets(Arr_data[*count].Name, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    printf("Surname: (if unknown, then enter: unknown)>");
    if (fgets(Arr_data[*count].Surname, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    printf("Age: (if unknown, then enter: -1)>");
    if (fgets(Value, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    if (sscanf(Value, "%d\n", &Arr_data[*count].age) != 1) {
        printf("\ninput error\n");
        return;
    }
    printf("Phone: (if unknown, then enter: unknown)>");
    if (fgets(Arr_data[*count].Phone, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    printf("Duty: (if unknown, then enter: -1.0)>");
    if (fgets(Value, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    if (sscanf(Value, "%f\n", &Arr_data[*count].duty) != 1) {
        printf("\ninput error\n");
        return;
    }
    int id = 1;
    for (int i = 0; i < *count; i++) {
        if (Arr_data[i].id == id) {
            id++;
        }
    }
    Arr_data[*count].id = id;
    Arr_data[*count].Phone[strlen(Arr_data[*count].Phone) - 1] = 0;
    Arr_data[*count].Name[strlen(Arr_data[*count].Name) - 1] = 0;
    Arr_data[*count].Surname[strlen(Arr_data[*count].Surname) - 1] = 0;
    (*count)++;
    printf("success\n");
}


void Find_data(Data_t *Arr_data, int *count) {
    if(!Arr_data){
        printf("Function Find_data: Arr_data is null\n");
        return;
    }
    if(!count){
        printf("Function Find_data: count is null\n");
        return;
    }
    char Type[9], Value[35];
    printf("enter the type off cell for which you want to find data: (Id, Name, Surname, Age, Phone, Duty)>");
    if (fgets(Type, 9, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    printf("enter the contents of this cell>");
    if (fgets(Value, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    Type[strlen(Type) - 1] = 0;
    Value[strlen(Value) - 1] = 0;
    if (strcmp(Type, "Id") == 0) {
        int id;
        if (sscanf(Value, "%d", &id) != 1) {
            printf("\ninput error\n");
            return;
        }
        for (int i = 0; i < *count; i++) {
            if (id == Arr_data[i].id) {
                Print_string_data(&Arr_data[i], stdout);
                break;
            }
        }
    } else if (strcmp(Type, "Name") == 0) {
        for (int i = 0; i < *count; i++) {
            if (strcmp(Value, Arr_data[i].Name) == 0 && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
            }
        }
    } else if (strcmp(Type, "Surname") == 0) {
        for (int i = 0; i < *count; i++) {
            if (strcmp(Value, Arr_data[i].Surname) == 0 && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
            }
        }
    } else if (strcmp(Type, "Age") == 0) {
        int age;
        if (sscanf(Value, "%d", &age) != 1) {
            printf("\ninput error\n");
            return;
        }
        for (int i = 0; i < *count; i++) {
            if (age == Arr_data[i].age && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
            }
        }
    } else if (strcmp(Type, "Phone") == 0) {
        for (int i = 0; i < *count; i++) {
            if (strcmp(Value, Arr_data[i].Phone) == 0 && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
            }
        }
    } else if (strcmp(Type, "Duty") == 0) {
        float duty;
        if (sscanf(Value, "%f", &duty) != 1) {
            printf("\ninput error\n");
            return;
        }
        for (int i = 0; i < *count; i++) {
            if (duty == Arr_data[i].duty && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
            }
        }
    }
}


void Change_data(Data_t *Arr_data, int *count) {
    if(!Arr_data){
        printf("Function Change_data: Arr_data is null\n");
        return;
    }
    char Value[35], Type[9];
    int id;
    Find_data(Arr_data, count);
    printf("enter id which you want to change>");
    if (fgets(Value, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    Value[strlen(Value) - 1] = 0;
    if (sscanf(Value, "%d", &id) != 1) {
        printf("\ninput error\n");
        return;
    }
    printf("enter the type off cell for which you want to change data: (Name, Surname, Age, Phone, Duty)>");
    if (fgets(Type, 9, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    printf("enter the new contents>");
    if (fgets(Value, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    Type[strlen(Type) - 1] = 0;
    Value[strlen(Value) - 1] = 0;
    for (int i = 0; i < *count; i++) {
        if (Arr_data[i].id == id) {
            if (strcmp(Type, "Name") == 0) {
                for (unsigned int j = 0; j < strlen(Value); j++) {
                    Arr_data[i].Name[j] = Value[j];
                }
                Arr_data[i].Name[strlen(Value)] = 0;
                printf("success\n");
                return;
            } else if (strcmp(Type, "Surname") == 0) {
                for (unsigned int j = 0; j < strlen(Value); j++) {
                    Arr_data[i].Surname[j] = Value[j];
                }
                Arr_data[i].Surname[strlen(Value)] = 0;
                printf("success\n");
                return;
            } else if (strcmp(Type, "Age") == 0) {
                if (sscanf(Value, "%d", &Arr_data[i].age) != 1) {
                    printf("\ninput error\n");
                    return;
                }
                printf("success\n");
                return;
            } else if (strcmp(Type, "Phone") == 0) {
                for (unsigned int j = 0; j < strlen(Value); j++) {
                    Arr_data[i].Phone[j] = Value[j];
                }
                Arr_data[i].Phone[strlen(Value)] = 0;
                printf("success\n");
                return;
            } else if (strcmp(Type, "Duty") == 0) {
                if (sscanf(Value, "%f", &Arr_data[i].duty) != 1) {
                    printf("\ninput error\n");
                    return;
                }
                printf("success\n");
                return;
            } else {
                printf("\ninput error: the entered type is not from the list\n");
                return;
            }
        }
    }
}


void Delete_data(Data_t *Arr_data, int *count) {
    char Type[9], Value[35], Value2[35], flag = '0';
    int id;
    printf("enter the type off cell for which you want to delete data: (Id, Name, Surname, Age, Phone, Duty)>");
    if (fgets(Type, 9, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    printf("enter the contents of this cell>");
    if (fgets(Value, 35, stdin) == NULL) {
        printf("\ninput error\n");
        return;
    }
    Type[strlen(Type) - 1] = 0;
    Value[strlen(Value) - 1] = 0;
    if (strcmp(Type, "Id") == 0) {
        if (sscanf(Value, "%d", &id) != 1) {
            printf("\ninput error\n");
            return;
        }
    } else if (strcmp(Type, "Name") == 0) {
        for (int i = 0; i < *count; i++) {
            if (strcmp(Value, Arr_data[i].Name) == 0 && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
                flag = '1';
            }
        }
        if (flag == '0') {
            printf("data not founded\n");
            return;
        }
        printf("enter id which you want to delete: (all if you want delete all cell>");
        if (fgets(Value2, 35, stdin) == NULL) {
            printf("\ninput error\n");
            return;
        }
        Value2[strlen(Value2) - 1] = 0;
        if (strcmp(Value2, "all") == 0) {
            for (int i = 0; i < *count; i++) {
                if (strcmp(Value, Arr_data[i].Name) == 0 && Arr_data[i].id != 0) {
                    Arr_data[i].id = 0;
                }
            }
            printf("success\n");
            return;
        } else if (sscanf(Value2, "%d", &id) != 1) {
            printf("\ninput error\n");
            return;
        }
    } else if (strcmp(Type, "Surname") == 0) {
        for (int i = 0; i < *count; i++) {
            if (strcmp(Value, Arr_data[i].Surname) == 0 && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
                flag = '1';
            }
        }
        if (flag == '0') {
            printf("data not founded\n");
            return;
        }
        printf("enter id which you want to delete: (all if you want delete all cell>");
        if (fgets(Value2, 35, stdin) == NULL) {
            printf("\ninput error\n");
            return;
        }
        Value2[strlen(Value2) - 1] = 0;
        if (strcmp(Value2, "all") == 0) {
            for (int i = 0; i < *count; i++) {
                if (strcmp(Value, Arr_data[i].Surname) == 0 && Arr_data[i].id != 0) {
                    Arr_data[i].id = 0;
                }
            }
            printf("success\n");
            return;
        } else if (sscanf(Value2, "%d", &id) != 1) {
            printf("\ninput error\n");
            return;
        }
    } else if (strcmp(Type, "Age") == 0) {
        int age;
        if (sscanf(Value, "%d", &age) != 1) {
            printf("\ninput error\n");
            return;
        }
        for (int i = 0; i < *count; i++) {
            if (age == Arr_data[i].age && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
                flag = '1';
            }
        }
        if (flag == '0') {
            printf("data not founded\n");
            return;
        }
        printf("enter id which you want to delete: (all if you want delete all cell>");
        if (fgets(Value2, 35, stdin) == NULL) {
            printf("\ninput error\n");
            return;
        }
        Value2[strlen(Value2) - 1] = 0;
        if (strcmp(Value2, "all") == 0) {
            for (int i = 0; i < *count; i++) {
                if (age == Arr_data[i].age && Arr_data[i].id != 0) {
                    Arr_data[i].id = 0;
                }
            }
            printf("success\n");
            return;
        } else if (sscanf(Value2, "%d", &id) != 1) {
            printf("\ninput error\n");
            return;
        }
    } else if (strcmp(Type, "Phone") == 0) {
        for (int i = 0; i < *count; i++) {
            if (strcmp(Value, Arr_data[i].Phone) == 0 && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
                flag = '1';
            }
        }
        if (flag == '0') {
            printf("data not founded\n");
            return;
        }
        printf("enter id which you want to delete: (all if you want delete all cell>");
        if (fgets(Value2, 35, stdin) == NULL) {
            printf("\ninput error\n");
            return;
        }
        Value2[strlen(Value2) - 1] = 0;
        if (strcmp(Value2, "all") == 0) {
            for (int i = 0; i < *count; i++) {
                if (strcmp(Arr_data[i].Phone, Value) == 0 && Arr_data[i].id != 0) {
                    Arr_data[i].id = 0;
                }
            }
            printf("success\n");
            return;
        } else if (sscanf(Value2, "%d", &id) != 1) {
            printf("\ninput error\n");
            return;
        }
    } else if (strcmp(Type, "Duty") == 0) {
        float duty;
        if (sscanf(Value, "%f", &duty) != 1) {
            printf("\ninput error\n");
            return;
        }
        for (int i = 0; i < *count; i++) {
            if (duty == Arr_data[i].duty && Arr_data[i].id != 0) {
                Print_string_data(&Arr_data[i], stdout);
                flag = '1';
            }
        }
        if (flag == '0') {
            printf("data not founded\n");
            return;
        }
        printf("enter id which you want to delete: (all if you want delete all cell>");
        if (fgets(Value2, 35, stdin) == NULL) {
            printf("\ninput error\n");
            return;
        }
        Value2[strlen(Value2) - 1] = 0;
        if (strcmp(Value2, "all") == 0) {
            for (int i = 0; i < *count; i++) {
                if (duty == Arr_data[i].duty && Arr_data[i].id != 0) {
                    Arr_data[i].id = 0;
                }
            }
            printf("success\n");
            return;
        } else if (sscanf(Value2, "%d", &id) != 1) {
            printf("\ninput error\n");
            return;
        }
    } else {
        printf("\ninput error: the entered type is not from the list\n");
        return;
    }
    for (int i = 0; i < *count; i++) {
        if (id == Arr_data[i].id) {
            Arr_data[i].id = 0;
            printf("success\n");
            return;
        }
    }
    printf("data not founded\n");
}


void Print_all(Data_t *Arr_data, int *count, FILE *fp) {
    for (int i = 0; i < *count; i++) {
        if (Arr_data[i].id != 0) {
            Print_string_data(&Arr_data[i], fp);
        }
    }
}


int Database_read(FILE *fp, Data_t *Arr_data, int count) {
    char string[300], pattern_id[] = "Id\0", pattern_name[] = "Name\0", pattern_surname[] = "Surname\0", pattern_age[] = "Age\0", pattern_phone[] = "Phone\0", pattern_duty[] = "Duty\0", help[100];
    for (int i = 0, j, jj; i < count; i++) {
        if (fgets(string, 300, fp) == NULL) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        if (Search(string, pattern_id) != 0) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        for (j = 4, jj = 0; string[j] != ';'; j++) {
            help[jj] = string[j];
            help[++jj] = 0;
        }
        if (sscanf(help, "%d", &Arr_data[i].id) != 1) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        if (Search(string, pattern_name) != j + 2) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        for (j += 8, jj = 0; string[j] != ';'; j++) {
            Arr_data[i].Name[jj] = string[j];
            Arr_data[i].Name[++jj] = 0;
        }
        if (Search(string, pattern_surname) != j + 2) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        for (j += 11, jj = 0; string[j] != ';'; j++) {
            Arr_data[i].Surname[jj] = string[j];
            Arr_data[i].Surname[++jj] = 0;
        }
        if (Search(string, pattern_age) != j + 2) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        for (j += 7, jj = 0; string[j] != ';'; j++) {
            help[jj] = string[j];
            help[++jj] = 0;
        }
        if (sscanf(help, "%d", &Arr_data[i].age) != 1) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        if (Search(string, pattern_phone) != j + 2) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        for (j += 9, jj = 0; string[j] != ';'; j++) {
            Arr_data[i].Phone[jj] = string[j];
            Arr_data[i].Phone[++jj] = 0;
        }
        if (Search(string, pattern_duty) != j + 2) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
        for (j += 8, jj = 0; j < strlen(string); j++) {
            help[jj] = string[j];
            help[++jj] = 0;
        }
        if (sscanf(help, "%f", &Arr_data[i].duty) != 1) {
            printf("error in file");
            fclose(fp);
            return 0;
        }
    }
    return 1;
}


void Save(FILE *fp, Data_t *Arr_data, int count) {
    int count_new = 0;
    for (int i = 0; i < count; i++) {
        if (Arr_data[i].id != 0) {
            count_new++;
        }
    }
    freopen("notepad.txt", "rw", fp);
    fprintf(fp, "%d\n", count_new);
    Print_all(Arr_data, &count, fp);
}


int main() {
    FILE *fp = fopen("notepad.txt", "r");
    char string[300];
    int count, value;
    if (fgets(string, 300, fp) == NULL) {
        printf("\nerror in file\n");
        return 0;
    }
    if (sscanf(string, "%d", &count) != 1) {
        printf("\nerror in file\n");
        return 0;
    }
    Data_t *Arr_data = (Data_t *) malloc(sizeof(Data_t) * count);
    value = Database_read(fp, Arr_data, count);
    if (!value) {
        free(Arr_data);
        return 0;
    }
    do {
        char Command[12];
        printf("enter command: (add, delete, find, print_all, change, save)>");
        if (fgets(Command, 12, stdin) == NULL) {
            printf("\ninput error\n");
        } else {
            Command[strlen(Command) - 1] = 0;
            if (strcmp(Command, "add") == 0) {
                Arr_data = realloc(Arr_data, sizeof(Data_t) * (count + 1));
                Add_data(Arr_data, &count);
            } else if (strcmp(Command, "delete") == 0) {
                Delete_data(Arr_data, &count);
            } else if (strcmp(Command, "find") == 0) {
                Find_data(Arr_data, &count);
            } else if (strcmp(Command, "print_all") == 0) {
                Print_all(Arr_data, &count, stdout);
            } else if (strcmp(Command, "change") == 0) {
                Change_data(Arr_data, &count);
            } else if (strcmp(Command, "save") == 0) {
                Save(fp, Arr_data, count);
            } else {
                printf("unknown command - %s\n", Command);
            }
        }
        printf("continue? (y, n)>");
        if (fgets(Command, 12, stdin) == NULL) {
            printf("\ninput error\n");
        } else {
            Command[strlen(Command) - 1] = 0;
            if (strcmp(Command, "n") == 0) {
                break;
            } else if (strcmp(Command, "y") == 0) {
                continue;
            } else {
                printf("\ninput error, the program ends\n");
                break;
            }
        }
    } while (1);
    Save(fp, Arr_data, count);
    free(Arr_data);
    fclose(fp);
    return 0;
}
