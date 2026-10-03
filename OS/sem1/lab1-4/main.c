#include "dynamic_lib.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    void *handle;
    void (*hello_from_dyn_runtime_lib)();

    handle = dlopen("./libdynamic_lib.so", RTLD_LAZY);

    hello_from_dyn_runtime_lib = (void (*)()) dlsym(handle, "hello_from_dyn_runtime_lib");

    hello_from_dyn_runtime_lib();

    dlclose(handle);

    return 0;
}