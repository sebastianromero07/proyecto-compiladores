.data
print_fmt: .string "%ld \n"
print_float_fmt: .string "%f \n"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $10, %rax
    movq %rax, -8(%rbp)
    movq $20, %rax
    movq %rax, -16(%rbp)
.data
.FL1081480340: .double 3.5
.text
    movsd .FL1081480340(%rip), %xmm0
    movsd %xmm0, -24(%rbp)
.data
.FL1081480341: .double 2
.text
    movsd .FL1081480341(%rip), %xmm0
    movsd %xmm0, -32(%rbp)
    # DEBUG: Body statement
.data
str_1081480342: .string "int + int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480342(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480343: .string "int - int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480343(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480344: .string "int * int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480344(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480345: .string "int / int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    movq $0, %rdx
    cqto
    idivq %rcx
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480345(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480346: .string "unsigned + unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480346(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480347: .string "unsigned - unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480347(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480348: .string "unsigned * unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480348(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480349: .string "unsigned / unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    movq $0, %rdx
    divq %rcx
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480349(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480350: .string "float + float = %f\n"
.text
    movsd -24(%rbp), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd -32(%rbp), %xmm0
    movsd %xmm0, %xmm1
    movsd (%rsp), %xmm0
    addq $8, %rsp
    addsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480350(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480351: .string "float - float = %f\n"
.text
    movsd -24(%rbp), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd -32(%rbp), %xmm0
    movsd %xmm0, %xmm1
    movsd (%rsp), %xmm0
    addq $8, %rsp
    subsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480351(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480352: .string "float * float = %f\n"
.text
    movsd -24(%rbp), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd -32(%rbp), %xmm0
    movsd %xmm0, %xmm1
    movsd (%rsp), %xmm0
    addq $8, %rsp
    mulsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480352(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480353: .string "float / float = %f\n"
.text
    movsd -24(%rbp), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd -32(%rbp), %xmm0
    movsd %xmm0, %xmm1
    movsd (%rsp), %xmm0
    addq $8, %rsp
    divsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480353(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480354: .string "int + unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480354(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480355: .string "int - unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480355(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480356: .string "int * unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480356(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480357: .string "int / unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    movq $0, %rdx
    divq %rcx
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1081480357(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480358: .string "int + float = %f\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    addsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480358(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480359: .string "int - float = %f\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    subsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480359(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480360: .string "int * float = %f\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    mulsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480360(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480361: .string "int / float = %f\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    divsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480361(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480362: .string "unsigned + float = %f\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    addsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480362(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480363: .string "unsigned - float = %f\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    subsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480363(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480364: .string "unsigned * float = %f\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    mulsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480364(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1081480365: .string "unsigned / float = %f\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movsd -24(%rbp), %xmm0
    movsd %xmm0, %xmm1
    popq %rax
    cvtsi2sd %rax, %xmm0
    divsd %xmm1, %xmm0
    movsd %xmm0, %xmm0
    movl $1, %eax
    leaq str_1081480365(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
