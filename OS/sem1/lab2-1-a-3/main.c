#include <sys/syscall.h>
#include <unistd.h>

ssize_t myWrite(int ff, const void* buf, size_t count){
    return syscall(SYS_write, ff, buf, count);
}

int main(void) {
    myWrite(1, "Hello, World!\n", 14);
    return 0;
}
