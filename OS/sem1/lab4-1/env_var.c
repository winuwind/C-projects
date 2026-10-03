#include <stdio.h>
#include <stdlib.h>

int main(){
    //    export MY_VAR=InitialValue
    char* var = getenv("MY_VAR");
    printf("$MY_VAR = %s\n", var);
    setenv("MY_VAR", "NewValue", 1);
    var = getenv("MY_VAR");
    printf("new $MY_VAR = %s\n", var);
    return 0;
}