.data
msg:
    .ascii "Hello, world! (without syscall)\n"

.text
    .global _start

_start:
    movq $4, %rax
    movq $1, %rbx
    movq $msg, %rcx
    movq $32, %rdx
    int $0x80

    movq $60, %rax
    movq $0, %rdi
    syscall