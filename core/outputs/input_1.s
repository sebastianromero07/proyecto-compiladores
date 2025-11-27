.data
print_fmt: .string "%ld \n"
print_float_fmt: .string "%f \n"
.text
.globl suma
suma:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $0, -24(%rbp)
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rax
    jmp .end_suma
.end_suma:
    leave
    ret
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $0, -8(%rbp)
    movq $0, -16(%rbp)
    movq $0, -24(%rbp)
    movq $2, %rax
    movq %rax, -8(%rbp)
    movq $3, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rdi
    movq -16(%rbp), %rax
    movq %rax, %rsi
    movl $0, %eax
    call suma@PLT
    movq %rax, -24(%rbp)
.data
str_0: .string "suma = %d\n"
.text
    movq -24(%rbp), %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_0(%rip), %rdi
    call printf@PLT
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
