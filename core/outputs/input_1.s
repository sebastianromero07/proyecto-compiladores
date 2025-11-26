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
.FL1594377488: .double 3.5
.text
    movsd .FL1594377488(%rip), %xmm0
    movsd %xmm0, -24(%rbp)
.data
.FL1594377489: .double 2
.text
    movsd .FL1594377489(%rip), %xmm0
    movsd %xmm0, -32(%rbp)
    # DEBUG: Body statement
.data
str_1594377490: .string "int + int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377490(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377491: .string "int - int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377491(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377492: .string "int * int = %d\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377492(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377493: .string "int / int = %d\n"
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
    leaq str_1594377493(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377494: .string "unsigned + unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377494(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377495: .string "unsigned - unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377495(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377496: .string "unsigned * unsigned = %u\n"
.text
    movq -16(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377496(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377497: .string "unsigned / unsigned = %u\n"
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
    leaq str_1594377497(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377498: .string "float + float = %f\n"
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
    leaq str_1594377498(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377499: .string "float - float = %f\n"
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
    leaq str_1594377499(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377500: .string "float * float = %f\n"
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
    leaq str_1594377500(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377501: .string "float / float = %f\n"
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
    leaq str_1594377501(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377502: .string "int + unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377502(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377503: .string "int - unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377503(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377504: .string "int * unsigned = %u\n"
.text
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    movl $0, %eax
    leaq str_1594377504(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377505: .string "int / unsigned = %u\n"
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
    leaq str_1594377505(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377506: .string "int + float = %f\n"
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
    leaq str_1594377506(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377507: .string "int - float = %f\n"
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
    leaq str_1594377507(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377508: .string "int * float = %f\n"
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
    leaq str_1594377508(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377509: .string "int / float = %f\n"
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
    leaq str_1594377509(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377510: .string "unsigned + float = %f\n"
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
    leaq str_1594377510(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377511: .string "unsigned - float = %f\n"
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
    leaq str_1594377511(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377512: .string "unsigned * float = %f\n"
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
    leaq str_1594377512(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
.data
str_1594377513: .string "unsigned / float = %f\n"
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
    leaq str_1594377513(%rip), %rdi
    call printf@PLT
    # DEBUG: Body statement
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
