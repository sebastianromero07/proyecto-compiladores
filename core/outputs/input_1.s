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
.FL0: .double 3.5
.text
    movsd .FL0(%rip), %xmm0
    movsd %xmm0, -24(%rbp)
.data
.FL1: .double 2
.text
    movsd .FL1(%rip), %xmm0
    movsd %xmm0, -32(%rbp)
    # DEBUG: Body statement
.data
str_2: .string "int + int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_2(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_3: .string "int - int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_3(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_4: .string "int * int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_4(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_5: .string "int / int = %d\n"
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
    leaq str_5(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_6: .string "unsigned + unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_6(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_7: .string "unsigned - unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_7(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_8: .string "unsigned * unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_8(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_9: .string "unsigned / unsigned = %u\n"
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
    leaq str_9(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_10: .string "float + float = %f\n"
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
    leaq str_10(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_11: .string "float - float = %f\n"
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
    leaq str_11(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_12: .string "float * float = %f\n"
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
    leaq str_12(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_13: .string "float / float = %f\n"
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
    leaq str_13(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_14: .string "int + unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_14(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_15: .string "int - unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_15(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_16: .string "int * unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_16(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_17: .string "int / unsigned = %u\n"
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
    leaq str_17(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_18: .string "int + float = %f\n"
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
    leaq str_18(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_19: .string "int - float = %f\n"
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
    leaq str_19(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_20: .string "int * float = %f\n"
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
    leaq str_20(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_21: .string "int / float = %f\n"
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
    leaq str_21(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_22: .string "unsigned + float = %f\n"
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
    leaq str_22(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_23: .string "unsigned - float = %f\n"
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
    leaq str_23(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_24: .string "unsigned * float = %f\n"
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
    leaq str_24(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_25: .string "unsigned / float = %f\n"
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
    leaq str_25(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
