.globl suma
suma:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $0, -24(%rbp)
    leaq -24(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    popq %rcx
    movq %rax, (%rcx)
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
    leaq -8(%rbp), %rax
    pushq %rax
    movq $2, %rax
    popq %rcx
    movq %rax, (%rcx)
    leaq -16(%rbp), %rax
    pushq %rax
    movq $3, %rax
    popq %rcx
    movq %rax, (%rcx)
    leaq -24(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rdi
    movq -16(%rbp), %rax
    movq %rax, %rsi
    movl $0, %eax
    call suma@PLT
    popq %rcx
    movq %rax, (%rcx)
.data
str_0: .string "suma = %d\n"
.text
    movq -24(%rbp), %rax
    pushq %rax
    movq 0(%rsp), %rsi
    addq $8, %rsp
    leaq str_0(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret

.section .note.GNU-stack,"",@progbits
