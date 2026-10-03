#include "dynamic_lib.h"

int printf(const char* arg, ...);

void hello_from_dyn_runtime_lib(){
    printf("Hello world");
}