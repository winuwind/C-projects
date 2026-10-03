void hello_from_dyn_runtime_lib();




//#include "dynamic_lib.h"
//
//#include <dlfcn.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//int main() {
//    void *handle;
//    void (*hello_from_dyn_runtime_lib)();
//
//    handle = dlopen("./libdynamic_lib.so", RTLD_LAZY);
//    if (!handle) {
//        printf("Error");
//        exit(EXIT_FAILURE);
//    }
//
//    hello_from_dyn_runtime_lib = (void (*)()) dlsym(handle, "hello_from_dyn_runtime_lib");
//
//    if (!hello_from_dyn_runtime_lib) {
//        printf("Error");
//        dlclose(handle);
////        exit(EXIT_FAILURE);
//    }
//
//    hello_from_dyn_runtime_lib();
//
//    dlclose(handle);
//
//    return 0;
//}