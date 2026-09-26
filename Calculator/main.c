#include <stdio.h>
#include <string.h>
#include <math.h>


int Priority[7] = {2, 1, 1, 1, 1, 2, 3};
int Flag = 0;


int Operation(int x, int y, int op){
    if((char) op == '*'){
        return x * y;
    }
    else if((char) op == '+'){
        return x + y;
    }
    else if((char) op == '-'){
        return x - y;
    }
    else if((char) op == '/'){
        if(y == 0){
            Flag = 2;
            return 0;
        }
        return x / y;
    }
    else if((char) op == '^'){
        return (int) pow(x, y);
    }
    return 0;
}


int Char_to_Int(char* str, int start, int end){
    int f = 0;
    for(int i = start; i < end; i++){
        f = f * 10 + (int) (str[i] - '0');
    }
    return f;
}


int Calculator(int* ints, int size){
    int stack[500];
    int index = 0;
    for(int i = 0; i < size; i++){
        if(ints[i] >= 0){
            stack[index++] = ints[i];
        }
        else{
            if(index < 2){
                Flag = 1;
            }
            else {
                stack[index - 2] = Operation(stack[index - 2], stack[index - 1], (-ints[i]));
            }
            if(Flag){
                return 0;
            }
            index--;
        }
    }
    return stack[0];
}


int Sort_Station(char* str, int len, int prev, int flag_help){
    int stack[1000], ints[1000], promo[500], index_s = 0, l = 0, index = 0, flag_int = 1, flag_op = 1, count_int = 0, count_op = 0, op1, op2;
    if(flag_help && ((str[0] - '*' < 6 && str[0] >= '*') || str[0] == '^')){
        if(prev < 0){
            ints[index++] = 0;
            count_int++;
            count_op++;
            flag_op = 1;
            stack[index_s] = -(int) '-';
            promo[index_s++] = 3;
            ints[index++] = -prev;
        }
        else {
            ints[index++] = prev;
        }
        count_int++;
        flag_int = 1;
    }
    for(int i = 0, j; i < len; i++){
        j = i;
        if((str[i] >= '0' && str[i] <= '9') || str[i] == '#'){
            flag_int = 1;
            count_int++;
            while(str[j] != '+' && str[j] != '-' && str[j] != '*' && str[j] != '/' && str[j] != '^' && str[j] != ')' && j < len){
                j++;
            }
            if(str[i] == '#'){
                if(prev < 0){
                    ints[index++] = 0;
                    if (index_s == 0) {
                        stack[index_s] = -(int) '-';
                        promo[index_s++] = l + 3;
                    }
                    else if(promo[index_s - 1] < l + 3){
                        stack[index_s] = -(int) '-';
                        promo[index_s++] = l + 3;
                    }
                    else if(promo[index_s - 1] == l + 3){
                        ints[index++] = stack[index_s - 1];
                        stack[index_s - 1] = -(int) '-';
                        promo[index_s - 1] = l + 3;
                    }
                    else{
                        while(index_s > 0 && promo[index_s - 1] > l + 3){
                            ints[index++] = stack[--index_s];
                        }
                        stack[index_s] = -(int) '-';
                        promo[index_s++] = l + 3;
                    }
                    ints[index++] = -prev;
                }
                else {
                    ints[index++] = prev;
                }
            }
            else {
                ints[index++] = Char_to_Int(str, i, j);
            }
            if(j == len){
                break;
            }
            i = j - 1;
        }
        else if((str[i] - '*' < 6 && str[i] >= '*') || str[i] == '^') {
            count_op++;
            flag_op = 1;
            if(str[i] == '-'){
                if(!flag_int && (i == 0 || str[i - 1] == '(')){
                    ints[index++] = 0;
                    count_int++;
                }
            }
            if(index_s > 0) {
                if (-stack[index_s - 1] == '^') {
                    op1 = 6;
                } else {
                    op1 = -stack[index_s - 1] - '*';
                }
            }
            if(str[i] == '^'){
                op2 = 6;
            }
            else{
                op2 = str[i] - '*';
            }
            if (index_s == 0) {
                stack[index_s++] = -(int) str[i];
                promo[index_s - 1] = l;
            }
            else if(Priority[op1] + promo[index_s - 1] < Priority[op2] + l){
                stack[index_s++] = -(int) str[i];
                promo[index_s - 1] = l;
            }
            else if(Priority[op1] + promo[index_s - 1] == Priority[op2] + l){
                if(op1 == op2 && op1 == 6){
                    stack[index_s++] = -(int) str[i];
                    promo[index_s - 1] = l;
                }
                else {
                    ints[index++] = stack[index_s - 1];
                    stack[index_s - 1] = -(int) str[i];
                    promo[index_s - 1] = l;
                }
            }
            else{
                while(index_s > 0 && Priority[op1] + promo[index_s - 1] > Priority[op2] + l){
                    ints[index++] = (stack[--index_s]);
                }
                stack[index_s++] = -(int) str[i];
                promo[index_s - 1] = l;
            }
        }
        else if(str[i] == '('){
            if(flag_op == 0){
                Flag = 1;
            }
            flag_int = 0;
            l += 3;
        }
        else if(str[i] == ')'){
            if(flag_int == 0){
                Flag = 1;
            }
            flag_op = 0;
            l -= 3;
            if(l < 0){
                Flag = 1;
            }
        }
    }
    if(l > 0 || Flag || count_int != count_op + 1){
        Flag = 1;
        return 0;
    }
    while(index_s > 0){
        ints[index++] = stack[--index_s];
    }
    return Calculator(ints, index);
}


int main(int count_arg, char *arr_arg[]) {
    int count_i = 0, flag_i = 0;
    if(count_arg > 1){
        if(sscanf(arr_arg[1], "%d", &flag_i) != 1){
            return 0;
        }
    }
    int answer = 0;
    char str[1002];
    do {
        if(count_i){
            printf("\n");
        }
        if(flag_i){
            printf("<");
        }
        if (fgets(str, 1002, stdin) == NULL || strlen(str) == 1) {
            if(flag_i){
                printf(">");
            }
            printf("syntax error");
            break;
        }
        if(flag_i > 0 && str[0] == '$'){
            break;
        }
        int len = (int) strlen(str) - 1;
        for (int i = 0; i < len; i++) {
            if ((str[i] < '0' || str[i] > '9') && str[i] != '(' && str[i] != ')' && str[i] != '*' && str[i] != '/' &&
                str[i] != '+' && str[i] != '-' && str[i] != '^' && str[i] != '#') {
                Flag = 1;
                break;
            }
        }
        if (len == 1 && (str[0] < '0' || str[0] > '9')) {
            Flag = 1;
        }
        answer = Sort_Station(str, len, answer, count_i);
        count_i = 1;
        if(!Flag){
            if(flag_i){
                printf(">");
            }
            printf("%d", answer);
            continue;
        }
        else if(Flag == 1){
            if(flag_i){
                printf(">");
            }
            printf("syntax error");
        }
        else{
            if(flag_i){
                printf(">");
            }
            printf("division by zero");
        }
        break;
    }while(flag_i);
    return 0;
}