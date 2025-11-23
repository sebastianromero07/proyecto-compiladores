.data
print_fmt: .string "%ld \n"
b: .quad 0
a: .quad 0
.text
.globl sumar
sumar:
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rax
    jmp .end_sumar
.end_sumar:
    leave
    ret
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $0, %rax
    movq %rax, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
while_0:
    movq -8(%rbp), %rax
    pushq %rax
    movq $5, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je endwhile_0
    movq -16(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -8(%rbp)
    jmp while_0
endwhile_0:
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    movq %rax, -8(%rbp)
for_1:
    movq -8(%rbp), %rax
    pushq %rax
    movq $3, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je endfor_1
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -8(%rbp)
    jmp for_1
endfor_1:
    movq -16(%rbp), %rax
    movq %rax, %rdi
    movq $3, %rax
    movq %rax, %rsi
call sumar
    movq %rax, a(%rip)
    movq a(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq a(%rip), %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
