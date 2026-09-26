#include <stdio.h>
#include <math.h>
#include <string.h>


int char_to_int(char symbol){
    if(symbol >= '0' && symbol <= '9'){
        return (int) symbol - (int) '0';
    }
    else if(symbol >= 'a' && symbol <= 'z'){
        return 10 + (int) symbol - (int) 'a';
    }
    else{
        return 10 + (int) symbol - (int) 'A';
    }
}


char int_to_char(int n){
    if(n > 9){
        return (char) ((n - 10) + (int) 'a');
    }
    else{
        return (char) (n + (int) '0');
    }
}


long double ToDec(char X[], int b){
    int index = 0;
    long double help = 0;
    for(int i = 0; i < 13; i++){
        if(X[i] == '.'){
            index = i;
            break;
        }
    }
    for(int i = 0; i < index; i++){
        help += char_to_int(X[i]) * pow(b, index - 1 - i);
    }
    if(index < 12){
        for(int i = index + 1; i < 13; i++){
            help += char_to_int(X[i]) * pow(b, index - i);
        }
    }
    return help;
}


int FromDec(long double number, int b, FILE *fp){
    long long int d;
    d = (long long int) number;
    long long int d_save = d;
    int H[100000];
    int count = 0;
    while(d > 0){
        H[count] = (int) (d % b);
        count++;
        d = d / b;
    }
    char print1[count + 1];
    print1[count] = '\0';
    int r = 0;
    for(int i = count; i >= 0; i--){
        if(i == count && H[i] == 0 && i > 0){
            r = 1;
            continue;
        }
        print1[count - i - r] = int_to_char(H[i]);
    }
    fputs(print1, fp);
    double help = (double) number - (double) d_save;
    if(help == 0){
        return 0;
    }
    int HH[100000], x;
    for(int i = 0; i < 100000; i++){
        HH[i] = 0;
    }
    count = 0;
    char print2[12];
    while(count < 12){
        help = help * b;
        HH[count] = (int) help;
        print2[count] = int_to_char(HH[count]);
        if(help >= 1) {
            x = (int) help;
            help = help - (double) x;
        }
        count++;
    }
    fputs(".", fp);
    fputs(print2, fp);
    return 0;
}


int main() {
    FILE *fp = stdin, *fp2 = stdout;
    long long int b1, b2;
    char B[7];
    if(fgets(B, 7, fp) == NULL){
        printf("bad input");
        return 0;
    }
    if(sscanf(B, "%lld %lld", &b1, &b2) != 2){
        printf("bad input");
        return 0;
    }
    if(b1 < 2 || b2 < 2 || b1 > 16 || b2 > 16){
        printf("bad input");
        return 0;
    }
    char str[15];
    if(fgets(str, 15, fp) == NULL){
        printf("bad input");
        return 0;
    }
    int flag = 0;
    if(str[0] == '.' || str[strlen(str) - 2] == '.'){
        printf("bad input");
        return 0;
    }
    for(int i = 0; i < 13; i++) {
        if (str[i] == '.' && !flag) {
            flag = 1;
            continue;
        }
        if(flag && str[i] == '.'){
            printf("bad input");
            return 0;
        }
        if (str[i] - '0' < -15) {
            if(!flag){
                str[i] = '.';
                i++;
            }
            for (int j = i; j < 13; j++) {
                str[j] = '0';
            }
            str[13] = '\0';
            break;
        }
        if (char_to_int(str[i]) >= b1 || str[i] - '0' < 0) {
            printf("bad input");
            return 0;
        }
    }
    long double dec = ToDec(str, (int) b1);
    int x = FromDec(dec, (int) b2, fp2);
    fclose(fp);
    fclose(fp2);
    return x;
}