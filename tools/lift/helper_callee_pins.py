"""Instruction-level byte pins for tools/lift/check_helper_*.py (generated from `objdump --triple=x86_64 -d`
of the pinned private ELF bodies; offsets are relative to each body's first byte). DATA ONLY: the checker, not
this table, decides what a pin means. Regenerate by decoding the same bodies again; a changed byte is a failed
check, never a silent update."""

PINS: dict[str, list[tuple[int, str]]] = {
    "ctor_c580": [  # 0xc580, 0x15f B
        (0x0, "55"),  # pushq %rbp
        (0x1, "4889e5"),  # movq %rsp, %rbp
        (0x4, "4157"),  # pushq %r15
        (0x6, "4156"),  # pushq %r14
        (0x8, "4155"),  # pushq %r13
        (0xa, "4154"),  # pushq %r12
        (0xc, "53"),  # pushq %rbx
        (0xd, "4883ec68"),  # subq $0x68, %rsp
        (0x11, "488b05801f5f01"),  # movq 0x15f1f80(%rip), %rax
        (0x18, "4989d4"),  # movq %rdx, %r12
        (0x1b, "488b5518"),  # movq 0x18(%rbp), %rdx
        (0x1f, "4c8b6d10"),  # movq 0x10(%rbp), %r13
        (0x23, "4989cf"),  # movq %rcx, %r15
        (0x26, "4489c9"),  # movl %r9d, %ecx
        (0x29, "4d89c6"),  # movq %r8, %r14
        (0x2c, "4889fb"),  # movq %rdi, %rbx
        (0x2f, "488b00"),  # movq (%rax), %rax
        (0x32, "488945d0"),  # movq %rax, -0x30(%rbp)
        (0x36, "e835450100"),  # callq 0x20af0 <.text+0x14570>
        (0x3b, "488d05ce7b5801"),  # leaq 0x1587bce(%rip), %rax
        (0x42, "c5fc57c0"),  # vxorps %ymm0, %ymm0, %ymm0
        (0x46, "488d0dd37e5801"),  # leaq 0x1587ed3(%rip), %rcx
        (0x4d, "4883c010"),  # addq $0x10, %rax
        (0x51, "488903"),  # movq %rax, (%rbx)
        (0x54, "4c896360"),  # movq %r12, 0x60(%rbx)
        (0x58, "c6436c00"),  # movb $0x0, 0x6c(%rbx)
        (0x5c, "c6437001"),  # movb $0x1, 0x70(%rbx)
        (0x60, "418b07"),  # movl (%r15), %eax
        (0x63, "89436c"),  # movl %eax, 0x6c(%rbx)
        (0x66, "c6437400"),  # movb $0x0, 0x74(%rbx)
        (0x6a, "c6437801"),  # movb $0x1, 0x78(%rbx)
        (0x6e, "418b06"),  # movl (%r14), %eax
        (0x71, "894374"),  # movl %eax, 0x74(%rbx)
        (0x74, "c5fc118380000000"),  # vmovups %ymm0, 0x80(%rbx)
        (0x7c, "48898bc0000000"),  # movq %rcx, 0xc0(%rbx)
        (0x83, "48c783b000000000000000"),  # movq $0x0, 0xb0(%rbx)
        (0x8e, "48c783c800000000000000"),  # movq $0x0, 0xc8(%rbx)
        (0x99, "c4c178104508"),  # vmovups 0x8(%r13), %xmm0
        (0x9f, "c5f8118388000000"),  # vmovups %xmm0, 0x88(%rbx)
        (0xa7, "498b4500"),  # movq (%r13), %rax
        (0xab, "48898380000000"),  # movq %rax, 0x80(%rbx)
        (0xb2, "498b4518"),  # movq 0x18(%r13), %rax
        (0xb6, "48898398000000"),  # movq %rax, 0x98(%rbx)
        (0xbd, "4885c0"),  # testq %rax, %rax
        (0xc0, "740a"),  # je 0xc64c <.text+0xcc>
        (0xc2, "4889d8"),  # movq %rbx, %rax
        (0xc5, "4883e880"),  # subq $-0x80, %rax
        (0xc9, "488900"),  # movq %rax, (%rax)
        (0xcc, "498b4548"),  # movq 0x48(%r13), %rax
        (0xd0, "4c8db578ffffff"),  # leaq -0x88(%rbp), %r14
        (0xd7, "498d7520"),  # leaq 0x20(%r13), %rsi
        (0xdb, "4c89f7"),  # movq %r14, %rdi
        (0xde, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0xe2, "498b4540"),  # movq 0x40(%r13), %rax
        (0xe6, "ff5008"),  # callq *0x8(%rax)
        (0xe9, "488b4598"),  # movq -0x68(%rbp), %rax
        (0xed, "4c8d7da8"),  # leaq -0x58(%rbp), %r15
        (0xf1, "4c89f6"),  # movq %r14, %rsi
        (0xf4, "4c89ff"),  # movq %r15, %rdi
        (0xf7, "ff10"),  # callq *(%rax)
        (0xf9, "488b83c0000000"),  # movq 0xc0(%rbx), %rax
        (0x100, "4c8da3a0000000"),  # leaq 0xa0(%rbx), %r12
        (0x107, "4c89f7"),  # movq %r14, %rdi
        (0x10a, "4c89e6"),  # movq %r12, %rsi
        (0x10d, "ff10"),  # callq *(%rax)
        (0x10f, "488b45c8"),  # movq -0x38(%rbp), %rax
        (0x113, "4c89e7"),  # movq %r12, %rdi
        (0x116, "4c89fe"),  # movq %r15, %rsi
        (0x119, "ff10"),  # callq *(%rax)
        (0x11b, "488b83c8000000"),  # movq 0xc8(%rbx), %rax
        (0x122, "488b4da0"),  # movq -0x60(%rbp), %rcx
        (0x126, "4c89f7"),  # movq %r14, %rdi
        (0x129, "48898bc8000000"),  # movq %rcx, 0xc8(%rbx)
        (0x130, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0x134, "488b4598"),  # movq -0x68(%rbp), %rax
        (0x138, "ff5018"),  # callq *0x18(%rax)
        (0x13b, "488b05561e5f01"),  # movq 0x15f1e56(%rip), %rax
        (0x142, "488b00"),  # movq (%rax), %rax
        (0x145, "483b45d0"),  # cmpq -0x30(%rbp), %rax
        (0x149, "750f"),  # jne 0xc6da <.text+0x15a>
        (0x14b, "4883c468"),  # addq $0x68, %rsp
        (0x14f, "5b"),  # popq %rbx
        (0x150, "415c"),  # popq %r12
        (0x152, "415d"),  # popq %r13
        (0x154, "415e"),  # popq %r14
        (0x156, "415f"),  # popq %r15
        (0x158, "5d"),  # popq %rbp
        (0x159, "c3"),  # retq
        (0x15a, "e8c12cf800"),  # callq 0xf8f3a0 <.text+0xf82e20>
    ],
    "ctor_c300": [  # 0xc300, 0x160 B
        (0x0, "55"),  # pushq %rbp
        (0x1, "4889e5"),  # movq %rsp, %rbp
        (0x4, "4157"),  # pushq %r15
        (0x6, "4156"),  # pushq %r14
        (0x8, "4155"),  # pushq %r13
        (0xa, "4154"),  # pushq %r12
        (0xc, "53"),  # pushq %rbx
        (0xd, "4883ec68"),  # subq $0x68, %rsp
        (0x11, "488b0500225f01"),  # movq 0x15f2200(%rip), %rax
        (0x18, "4989d4"),  # movq %rdx, %r12
        (0x1b, "488b5518"),  # movq 0x18(%rbp), %rdx
        (0x1f, "4c8b6d10"),  # movq 0x10(%rbp), %r13
        (0x23, "4989cf"),  # movq %rcx, %r15
        (0x26, "4489c9"),  # movl %r9d, %ecx
        (0x29, "4d89c6"),  # movq %r8, %r14
        (0x2c, "4889fb"),  # movq %rdi, %rbx
        (0x2f, "488b00"),  # movq (%rax), %rax
        (0x32, "488945d0"),  # movq %rax, -0x30(%rbp)
        (0x36, "e8b5470100"),  # callq 0x20af0 <.text+0x147f0>
        (0x3b, "488d05d67d5801"),  # leaq 0x1587dd6(%rip), %rax
        (0x42, "c5fc57c0"),  # vxorps %ymm0, %ymm0, %ymm0
        (0x46, "488d0dcb7f5801"),  # leaq 0x1587fcb(%rip), %rcx
        (0x4d, "4883c010"),  # addq $0x10, %rax
        (0x51, "488903"),  # movq %rax, (%rbx)
        (0x54, "4c896360"),  # movq %r12, 0x60(%rbx)
        (0x58, "c6436c00"),  # movb $0x0, 0x6c(%rbx)
        (0x5c, "c6437001"),  # movb $0x1, 0x70(%rbx)
        (0x60, "418b07"),  # movl (%r15), %eax
        (0x63, "89436c"),  # movl %eax, 0x6c(%rbx)
        (0x66, "c6437400"),  # movb $0x0, 0x74(%rbx)
        (0x6a, "c6437801"),  # movb $0x1, 0x78(%rbx)
        (0x6e, "418b06"),  # movl (%r14), %eax
        (0x71, "894374"),  # movl %eax, 0x74(%rbx)
        (0x74, "c5fc118380000000"),  # vmovups %ymm0, 0x80(%rbx)
        (0x7c, "48898bc0000000"),  # movq %rcx, 0xc0(%rbx)
        (0x83, "48c783b000000000000000"),  # movq $0x0, 0xb0(%rbx)
        (0x8e, "48c783c800000000000000"),  # movq $0x0, 0xc8(%rbx)
        (0x99, "c4c178104508"),  # vmovups 0x8(%r13), %xmm0
        (0x9f, "c5f8118388000000"),  # vmovups %xmm0, 0x88(%rbx)
        (0xa7, "498b4500"),  # movq (%r13), %rax
        (0xab, "48898380000000"),  # movq %rax, 0x80(%rbx)
        (0xb2, "498b4518"),  # movq 0x18(%r13), %rax
        (0xb6, "48898398000000"),  # movq %rax, 0x98(%rbx)
        (0xbd, "4885c0"),  # testq %rax, %rax
        (0xc0, "740a"),  # je 0xc3cc <.text+0xcc>
        (0xc2, "4889d8"),  # movq %rbx, %rax
        (0xc5, "4883e880"),  # subq $-0x80, %rax
        (0xc9, "488900"),  # movq %rax, (%rax)
        (0xcc, "498b4548"),  # movq 0x48(%r13), %rax
        (0xd0, "4c8db578ffffff"),  # leaq -0x88(%rbp), %r14
        (0xd7, "498d7520"),  # leaq 0x20(%r13), %rsi
        (0xdb, "4c89f7"),  # movq %r14, %rdi
        (0xde, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0xe2, "498b4540"),  # movq 0x40(%r13), %rax
        (0xe6, "ff5008"),  # callq *0x8(%rax)
        (0xe9, "488b4598"),  # movq -0x68(%rbp), %rax
        (0xed, "4c8d7da8"),  # leaq -0x58(%rbp), %r15
        (0xf1, "4c89f6"),  # movq %r14, %rsi
        (0xf4, "4c89ff"),  # movq %r15, %rdi
        (0xf7, "ff10"),  # callq *(%rax)
        (0xf9, "488b83c0000000"),  # movq 0xc0(%rbx), %rax
        (0x100, "4c8da3a0000000"),  # leaq 0xa0(%rbx), %r12
        (0x107, "4c89f7"),  # movq %r14, %rdi
        (0x10a, "4c89e6"),  # movq %r12, %rsi
        (0x10d, "ff10"),  # callq *(%rax)
        (0x10f, "488b45c8"),  # movq -0x38(%rbp), %rax
        (0x113, "4c89e7"),  # movq %r12, %rdi
        (0x116, "4c89fe"),  # movq %r15, %rsi
        (0x119, "ff10"),  # callq *(%rax)
        (0x11b, "488b83c8000000"),  # movq 0xc8(%rbx), %rax
        (0x122, "488b4da0"),  # movq -0x60(%rbp), %rcx
        (0x126, "4c89f7"),  # movq %r14, %rdi
        (0x129, "48898bc8000000"),  # movq %rcx, 0xc8(%rbx)
        (0x130, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0x134, "488b4598"),  # movq -0x68(%rbp), %rax
        (0x138, "ff5018"),  # callq *0x18(%rax)
        (0x13b, "488b05d6205f01"),  # movq 0x15f20d6(%rip), %rax
        (0x142, "488b00"),  # movq (%rax), %rax
        (0x145, "483b45d0"),  # cmpq -0x30(%rbp), %rax
        (0x149, "750f"),  # jne 0xc45a <.text+0x15a>
        (0x14b, "4883c468"),  # addq $0x68, %rsp
        (0x14f, "5b"),  # popq %rbx
        (0x150, "415c"),  # popq %r12
        (0x152, "415d"),  # popq %r13
        (0x154, "415e"),  # popq %r14
        (0x156, "415f"),  # popq %r15
        (0x158, "5d"),  # popq %rbp
        (0x159, "c3"),  # retq
        (0x15a, "e8412ff800"),  # callq 0xf8f3a0 <.text+0xf830a0>
        (0x15f, "90"),  # nop
    ],
    "ctor_6aa00": [  # 0x6aa00, 0x142 B
        (0x0, "55"),  # pushq %rbp
        (0x1, "4889e5"),  # movq %rsp, %rbp
        (0x4, "4157"),  # pushq %r15
        (0x6, "4156"),  # pushq %r14
        (0x8, "4155"),  # pushq %r13
        (0xa, "4154"),  # pushq %r12
        (0xc, "53"),  # pushq %rbx
        (0xd, "4883ec68"),  # subq $0x68, %rsp
        (0x11, "4c8b2d003b5901"),  # movq 0x1593b00(%rip), %r13
        (0x18, "4989d7"),  # movq %rdx, %r15
        (0x1b, "4c89ca"),  # movq %r9, %rdx
        (0x1e, "4d89c6"),  # movq %r8, %r14
        (0x21, "4889fb"),  # movq %rdi, %rbx
        (0x24, "498b4500"),  # movq (%r13), %rax
        (0x28, "488945d0"),  # movq %rax, -0x30(%rbp)
        (0x2c, "e8bf60fbff"),  # callq 0x20af0
        (0x31, "488d05e0965201"),  # leaq 0x15296e0(%rip), %rax
        (0x38, "c5fc57c0"),  # vxorps %ymm0, %ymm0, %ymm0
        (0x3c, "488d0d35be5201"),  # leaq 0x152be35(%rip), %rcx
        (0x43, "4883c010"),  # addq $0x10, %rax
        (0x47, "488903"),  # movq %rax, (%rbx)
        (0x4a, "4c897b60"),  # movq %r15, 0x60(%rbx)
        (0x4e, "c6436c00"),  # movb $0x0, 0x6c(%rbx)
        (0x52, "c6437000"),  # movb $0x0, 0x70(%rbx)
        (0x56, "c6437400"),  # movb $0x0, 0x74(%rbx)
        (0x5a, "c6437800"),  # movb $0x0, 0x78(%rbx)
        (0x5e, "c5fc118380000000"),  # vmovups %ymm0, 0x80(%rbx)
        (0x66, "48898bc0000000"),  # movq %rcx, 0xc0(%rbx)
        (0x6d, "48c783b000000000000000"),  # movq $0x0, 0xb0(%rbx)
        (0x78, "48c783c800000000000000"),  # movq $0x0, 0xc8(%rbx)
        (0x83, "c4c178104608"),  # vmovups 0x8(%r14), %xmm0
        (0x89, "c5f8118388000000"),  # vmovups %xmm0, 0x88(%rbx)
        (0x91, "498b06"),  # movq (%r14), %rax
        (0x94, "48898380000000"),  # movq %rax, 0x80(%rbx)
        (0x9b, "498b4618"),  # movq 0x18(%r14), %rax
        (0x9f, "48898398000000"),  # movq %rax, 0x98(%rbx)
        (0xa6, "4885c0"),  # testq %rax, %rax
        (0xa9, "740a"),  # je 0x6aab5 <.text+0xb5>
        (0xab, "4889d8"),  # movq %rbx, %rax
        (0xae, "4883e880"),  # subq $-0x80, %rax
        (0xb2, "488900"),  # movq %rax, (%rax)
        (0xb5, "498b4648"),  # movq 0x48(%r14), %rax
        (0xb9, "498d7620"),  # leaq 0x20(%r14), %rsi
        (0xbd, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0xc1, "498b4640"),  # movq 0x40(%r14), %rax
        (0xc5, "4c8db578ffffff"),  # leaq -0x88(%rbp), %r14
        (0xcc, "4c89f7"),  # movq %r14, %rdi
        (0xcf, "ff5008"),  # callq *0x8(%rax)
        (0xd2, "488b4598"),  # movq -0x68(%rbp), %rax
        (0xd6, "4c8d7da8"),  # leaq -0x58(%rbp), %r15
        (0xda, "4c89f6"),  # movq %r14, %rsi
        (0xdd, "4c89ff"),  # movq %r15, %rdi
        (0xe0, "ff10"),  # callq *(%rax)
        (0xe2, "488b83c0000000"),  # movq 0xc0(%rbx), %rax
        (0xe9, "4c8da3a0000000"),  # leaq 0xa0(%rbx), %r12
        (0xf0, "4c89f7"),  # movq %r14, %rdi
        (0xf3, "4c89e6"),  # movq %r12, %rsi
        (0xf6, "ff10"),  # callq *(%rax)
        (0xf8, "488b45c8"),  # movq -0x38(%rbp), %rax
        (0xfc, "4c89e7"),  # movq %r12, %rdi
        (0xff, "4c89fe"),  # movq %r15, %rsi
        (0x102, "ff10"),  # callq *(%rax)
        (0x104, "488b83c8000000"),  # movq 0xc8(%rbx), %rax
        (0x10b, "488b4da0"),  # movq -0x60(%rbp), %rcx
        (0x10f, "4c89f7"),  # movq %r14, %rdi
        (0x112, "48898bc8000000"),  # movq %rcx, 0xc8(%rbx)
        (0x119, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0x11d, "488b4598"),  # movq -0x68(%rbp), %rax
        (0x121, "ff5018"),  # callq *0x18(%rax)
        (0x124, "498b4500"),  # movq (%r13), %rax
        (0x128, "483b45d0"),  # cmpq -0x30(%rbp), %rax
        (0x12c, "750f"),  # jne 0x6ab3d <.text+0x13d>
        (0x12e, "4883c468"),  # addq $0x68, %rsp
        (0x132, "5b"),  # popq %rbx
        (0x133, "415c"),  # popq %r12
        (0x135, "415d"),  # popq %r13
        (0x137, "415e"),  # popq %r14
        (0x139, "415f"),  # popq %r15
        (0x13b, "5d"),  # popq %rbp
        (0x13c, "c3"),  # retq
        (0x13d, "e85e48f200"),  # callq 0xf8f3a0 <.text+0xf249a0>
    ],
    "ctor_3a980": [  # 0x3a980, 0x142 B
        (0x0, "55"),  # pushq %rbp
        (0x1, "4889e5"),  # movq %rsp, %rbp
        (0x4, "4157"),  # pushq %r15
        (0x6, "4156"),  # pushq %r14
        (0x8, "4155"),  # pushq %r13
        (0xa, "4154"),  # pushq %r12
        (0xc, "53"),  # pushq %rbx
        (0xd, "4883ec68"),  # subq $0x68, %rsp
        (0x11, "4c8b2d803b5c01"),  # movq 0x15c3b80(%rip), %r13
        (0x18, "4989d7"),  # movq %rdx, %r15
        (0x1b, "4c89ca"),  # movq %r9, %rdx
        (0x1e, "4d89c6"),  # movq %r8, %r14
        (0x21, "4889fb"),  # movq %rdi, %rbx
        (0x24, "498b4500"),  # movq (%r13), %rax
        (0x28, "488945d0"),  # movq %rax, -0x30(%rbp)
        (0x2c, "e83f61feff"),  # callq 0x20af0
        (0x31, "488d05d8975501"),  # leaq 0x15597d8(%rip), %rax
        (0x38, "c5fc57c0"),  # vxorps %ymm0, %ymm0, %ymm0
        (0x3c, "488d0d9dac5501"),  # leaq 0x155ac9d(%rip), %rcx
        (0x43, "4883c010"),  # addq $0x10, %rax
        (0x47, "488903"),  # movq %rax, (%rbx)
        (0x4a, "4c897b60"),  # movq %r15, 0x60(%rbx)
        (0x4e, "c6436c00"),  # movb $0x0, 0x6c(%rbx)
        (0x52, "c6437000"),  # movb $0x0, 0x70(%rbx)
        (0x56, "c6437400"),  # movb $0x0, 0x74(%rbx)
        (0x5a, "c6437800"),  # movb $0x0, 0x78(%rbx)
        (0x5e, "c5fc118380000000"),  # vmovups %ymm0, 0x80(%rbx)
        (0x66, "48898bc0000000"),  # movq %rcx, 0xc0(%rbx)
        (0x6d, "48c783b000000000000000"),  # movq $0x0, 0xb0(%rbx)
        (0x78, "48c783c800000000000000"),  # movq $0x0, 0xc8(%rbx)
        (0x83, "c4c178104608"),  # vmovups 0x8(%r14), %xmm0
        (0x89, "c5f8118388000000"),  # vmovups %xmm0, 0x88(%rbx)
        (0x91, "498b06"),  # movq (%r14), %rax
        (0x94, "48898380000000"),  # movq %rax, 0x80(%rbx)
        (0x9b, "498b4618"),  # movq 0x18(%r14), %rax
        (0x9f, "48898398000000"),  # movq %rax, 0x98(%rbx)
        (0xa6, "4885c0"),  # testq %rax, %rax
        (0xa9, "740a"),  # je 0x3aa35 <.text+0xb5>
        (0xab, "4889d8"),  # movq %rbx, %rax
        (0xae, "4883e880"),  # subq $-0x80, %rax
        (0xb2, "488900"),  # movq %rax, (%rax)
        (0xb5, "498b4648"),  # movq 0x48(%r14), %rax
        (0xb9, "498d7620"),  # leaq 0x20(%r14), %rsi
        (0xbd, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0xc1, "498b4640"),  # movq 0x40(%r14), %rax
        (0xc5, "4c8db578ffffff"),  # leaq -0x88(%rbp), %r14
        (0xcc, "4c89f7"),  # movq %r14, %rdi
        (0xcf, "ff5008"),  # callq *0x8(%rax)
        (0xd2, "488b4598"),  # movq -0x68(%rbp), %rax
        (0xd6, "4c8d7da8"),  # leaq -0x58(%rbp), %r15
        (0xda, "4c89f6"),  # movq %r14, %rsi
        (0xdd, "4c89ff"),  # movq %r15, %rdi
        (0xe0, "ff10"),  # callq *(%rax)
        (0xe2, "488b83c0000000"),  # movq 0xc0(%rbx), %rax
        (0xe9, "4c8da3a0000000"),  # leaq 0xa0(%rbx), %r12
        (0xf0, "4c89f7"),  # movq %r14, %rdi
        (0xf3, "4c89e6"),  # movq %r12, %rsi
        (0xf6, "ff10"),  # callq *(%rax)
        (0xf8, "488b45c8"),  # movq -0x38(%rbp), %rax
        (0xfc, "4c89e7"),  # movq %r12, %rdi
        (0xff, "4c89fe"),  # movq %r15, %rsi
        (0x102, "ff10"),  # callq *(%rax)
        (0x104, "488b83c8000000"),  # movq 0xc8(%rbx), %rax
        (0x10b, "488b4da0"),  # movq -0x60(%rbp), %rcx
        (0x10f, "4c89f7"),  # movq %r14, %rdi
        (0x112, "48898bc8000000"),  # movq %rcx, 0xc8(%rbx)
        (0x119, "488945a0"),  # movq %rax, -0x60(%rbp)
        (0x11d, "488b4598"),  # movq -0x68(%rbp), %rax
        (0x121, "ff5018"),  # callq *0x18(%rax)
        (0x124, "498b4500"),  # movq (%r13), %rax
        (0x128, "483b45d0"),  # cmpq -0x30(%rbp), %rax
        (0x12c, "750f"),  # jne 0x3aabd <.text+0x13d>
        (0x12e, "4883c468"),  # addq $0x68, %rsp
        (0x132, "5b"),  # popq %rbx
        (0x133, "415c"),  # popq %r12
        (0x135, "415d"),  # popq %r13
        (0x137, "415e"),  # popq %r14
        (0x139, "415f"),  # popq %r15
        (0x13b, "5d"),  # popq %rbp
        (0x13c, "c3"),  # retq
        (0x13d, "e8de48f500"),  # callq 0xf8f3a0 <.text+0xf54a20>
    ],
    "menu_26db0": [  # 0x26db0, 0xef B
        (0x0, "55"),  # pushq %rbp
        (0x1, "4889e5"),  # movq %rsp, %rbp
        (0x4, "4157"),  # pushq %r15
        (0x6, "4156"),  # pushq %r14
        (0x8, "53"),  # pushq %rbx
        (0x9, "50"),  # pushq %rax
        (0xa, "4889fb"),  # movq %rdi, %rbx
        (0xd, "4189cf"),  # movl %ecx, %r15d
        (0x10, "4989d6"),  # movq %rdx, %r14
        (0x13, "48c70300000000"),  # movq $0x0, (%rbx)
        (0x1a, "48895b08"),  # movq %rbx, 0x8(%rbx)
        (0x1e, "48895b10"),  # movq %rbx, 0x10(%rbx)
        (0x22, "48c7431800000000"),  # movq $0x0, 0x18(%rbx)
        (0x2a, "c6432000"),  # movb $0x0, 0x20(%rbx)
        (0x2e, "c5f81006"),  # vmovups (%rsi), %xmm0
        (0x32, "c5f8114328"),  # vmovups %xmm0, 0x28(%rbx)
        (0x37, "488b4610"),  # movq 0x10(%rsi), %rax
        (0x3b, "48894338"),  # movq %rax, 0x38(%rbx)
        (0x3f, "488b4618"),  # movq 0x18(%rsi), %rax
        (0x43, "48894340"),  # movq %rax, 0x40(%rbx)
        (0x47, "4885c0"),  # testq %rax, %rax
        (0x4a, "7404"),  # je 0x26e00 <.text+0x50>
        (0x4c, "f0"),  # lock
        (0x4d, "48ff00"),  # incq (%rax)
        (0x50, "c7434802000000"),  # movl $0x2, 0x48(%rbx)
        (0x57, "c4c178104620"),  # vmovups 0x20(%r14), %xmm0
        (0x5d, "c5f8114350"),  # vmovups %xmm0, 0x50(%rbx)
        (0x62, "498b4630"),  # movq 0x30(%r14), %rax
        (0x66, "48894360"),  # movq %rax, 0x60(%rbx)
        (0x6a, "498b4638"),  # movq 0x38(%r14), %rax
        (0x6e, "48894368"),  # movq %rax, 0x68(%rbx)
        (0x72, "4885c0"),  # testq %rax, %rax
        (0x75, "7404"),  # je 0x26e2b <.text+0x7b>
        (0x77, "f0"),  # lock
        (0x78, "48ff00"),  # incq (%rax)
        (0x7b, "488d7b70"),  # leaq 0x70(%rbx), %rdi
        (0x7f, "e84c83feff"),  # callq 0xf180
        (0x84, "4c89b390000000"),  # movq %r14, 0x90(%rbx)
        (0x8b, "c783980000000000803f"),  # movl $0x3f800000, 0x98(%rbx)
        (0x95, "c7839c00000000002041"),  # movl $0x41200000, 0x9c(%rbx)
        (0x9f, "4489bba0000000"),  # movl %r15d, 0xa0(%rbx)
        (0xa6, "4c89f7"),  # movq %r14, %rdi
        (0xa9, "498b06"),  # movq (%r14), %rax
        (0xac, "ff5018"),  # callq *0x18(%rax)
        (0xaf, "83f801"),  # cmpl $0x1, %eax
        (0xb2, "751f"),  # jne 0x26e83 <.text+0xd3>
        (0xb4, "498b06"),  # movq (%r14), %rax
        (0xb7, "4c89f7"),  # movq %r14, %rdi
        (0xba, "ff5030"),  # callq *0x30(%rax)
        (0xbd, "83f803"),  # cmpl $0x3, %eax
        (0xc0, "7511"),  # jne 0x26e83 <.text+0xd3>
        (0xc2, "48b8cdcccc3d0000003f"),  # movabsq $0x3f0000003dcccccd, %rax
        (0xcc, "48898398000000"),  # movq %rax, 0x98(%rbx)
        (0xd3, "488d052e66dd02"),  # leaq 0x2dd662e(%rip), %rax
        (0xda, "4889df"),  # movq %rbx, %rdi
        (0xdd, "488b30"),  # movq (%rax), %rsi
        (0xe0, "4883c408"),  # addq $0x8, %rsp
        (0xe4, "5b"),  # popq %rbx
        (0xe5, "415e"),  # popq %r14
        (0xe7, "415f"),  # popq %r15
        (0xe9, "5d"),  # popq %rbp
        (0xea, "e911faffff"),  # jmp 0x268b0
    ],
    "fi_copy": [  # 0xc720, 0x14 B
        (0x0, "488b4610"),  # movq 0x10(%rsi), %rax
        (0x4, "488d0d757d5801"),  # leaq 0x1587d75(%rip), %rcx
        (0xb, "48894710"),  # movq %rax, 0x10(%rdi)
        (0xf, "48894f20"),  # movq %rcx, 0x20(%rdi)
        (0x13, "c3"),  # retq
    ],
    "fi_clone": [  # 0xc740, 0x14 B
        (0x0, "488d05597d5801"),  # leaq 0x1587d59(%rip), %rax
        (0x7, "48894720"),  # movq %rax, 0x20(%rdi)
        (0xb, "488b4610"),  # movq 0x10(%rsi), %rax
        (0xf, "48894710"),  # movq %rax, 0x10(%rdi)
        (0x13, "c3"),  # retq
    ],
    "fi_move": [  # 0xc760, 0x9 B
        (0x0, "488b4610"),  # movq 0x10(%rsi), %rax
        (0x4, "48894710"),  # movq %rax, 0x10(%rdi)
        (0x8, "c3"),  # retq
    ],
    "fi_destroy": [  # 0xc770, 0x1 B
        (0x0, "c3"),  # retq
    ],
    "ff_copy": [  # 0xc4a0, 0x14 B
        (0x0, "488b4610"),  # movq 0x10(%rsi), %rax
        (0x4, "488d0d6d7e5801"),  # leaq 0x1587e6d(%rip), %rcx
        (0xb, "48894710"),  # movq %rax, 0x10(%rdi)
        (0xf, "48894f20"),  # movq %rcx, 0x20(%rdi)
        (0x13, "c3"),  # retq
    ],
    "ff_clone": [  # 0xc4c0, 0x14 B
        (0x0, "488d05517e5801"),  # leaq 0x1587e51(%rip), %rax
        (0x7, "48894720"),  # movq %rax, 0x20(%rdi)
        (0xb, "488b4610"),  # movq 0x10(%rsi), %rax
        (0xf, "48894710"),  # movq %rax, 0x10(%rdi)
        (0x13, "c3"),  # retq
    ],
    "ff_move": [  # 0xc4e0, 0x9 B
        (0x0, "488b4610"),  # movq 0x10(%rsi), %rax
        (0x4, "48894710"),  # movq %rax, 0x10(%rdi)
        (0x8, "c3"),  # retq
    ],
    "ff_destroy": [  # 0xc4f0, 0x1 B
        (0x0, "c3"),  # retq
    ],
}

SITE_PINS: dict[str, list[tuple[int, str]]] = {
    "site_430": [  # absolute addresses [0x529ec4, 0x529f19)
        (0x529ec4, "c5fc57c0"),  # vxorps	%ymm0, %ymm0, %ymm0
        (0x529ec8, "488d0581e50901"),  # leaq	0x109e581(%rip), %rax
        (0x529ecf, "c5fc11442460"),  # vmovups	%ymm0, 0x60(%rsp)
        (0x529ed5, "48898424a0000000"),  # movq	%rax, 0xa0(%rsp)
        (0x529edd, "48c78424a800000000000000"),  # movq	$0x0, 0xa8(%rsp)
        (0x529ee9, "488d442460"),  # leaq	0x60(%rsp), %rax
        (0x529eee, "4983c610"),  # addq	$0x10, %r14
        (0x529ef2, "488d93d0000000"),  # leaq	0xd0(%rbx), %rdx
        (0x529ef9, "4889df"),  # movq	%rbx, %rdi
        (0x529efc, "4c8933"),  # movq	%r14, (%rbx)
        (0x529eff, "4154"),  # pushq	%r12
        (0x529f01, "50"),  # pushq	%rax
        (0x529f02, "e87926aeff"),  # callq	0xc580
        (0x529f07, "4883c410"),  # addq	$0x10, %rsp
        (0x529f0b, "488b8424a0000000"),  # movq	0xa0(%rsp), %rax
        (0x529f13, "4c89ff"),  # movq	%r15, %rdi
        (0x529f16, "ff5018"),  # callq	*0x18(%rax)
    ],
    "site_504": [  # absolute addresses [0x23f563, 0x23f5b1)
        (0x23f563, "c5fc57c0"),  # vxorps	%ymm0, %ymm0, %ymm0
        (0x23f567, "488d05fac43501"),  # leaq	0x135c4fa(%rip), %rax
        (0x23f56e, "488d93d0000000"),  # leaq	0xd0(%rbx), %rdx
        (0x23f575, "4c89fe"),  # movq	%r15, %rsi
        (0x23f578, "48c744247000000000"),  # movq	$0x0, 0x70(%rsp)
        (0x23f581, "4c892b"),  # movq	%r13, (%rbx)
        (0x23f584, "c5fc11442440"),  # vmovups	%ymm0, 0x40(%rsp)
        (0x23f58a, "4889842480000000"),  # movq	%rax, 0x80(%rsp)
        (0x23f592, "48c784248800000000000000"),  # movq	$0x0, 0x88(%rsp)
        (0x23f59e, "e85db4e2ff"),  # callq	0x6aa00
        (0x23f5a3, "488b842480000000"),  # movq	0x80(%rsp), %rax
        (0x23f5ab, "4c89f7"),  # movq	%r14, %rdi
        (0x23f5ae, "ff5018"),  # callq	*0x18(%rax)
    ],
    "site_555": [  # absolute addresses [0x528b7, 0x52958)
        (0x528b7, "c5fc57c0"),  # vxorps	%ymm0, %ymm0, %ymm0
        (0x528bb, "48c744247000000000"),  # movq	$0x0, 0x70(%rsp)
        (0x528c4, "c5f057c9"),  # vxorps	%xmm1, %xmm1, %xmm1
        (0x528c8, "4c8d442440"),  # leaq	0x40(%rsp), %r8
        (0x528cd, "b901000000"),  # movl	$0x1, %ecx
        (0x528d2, "4c8d642460"),  # leaq	0x60(%rsp), %r12
        (0x528d7, "48890424"),  # movq	%rax, (%rsp)
        (0x528db, "488d057e355401"),  # leaq	0x154357e(%rip), %rax
        (0x528e2, "c5fc11442440"),  # vmovups	%ymm0, 0x40(%rsp)
        (0x528e8, "4c89ee"),  # movq	%r13, %rsi
        (0x528eb, "4d89f1"),  # movq	%r14, %r9
        (0x528ee, "48c744240810000000"),  # movq	$0x10, 0x8(%rsp)
        (0x528f7, "c5f8114c2410"),  # vmovups	%xmm1, 0x10(%rsp)
        (0x528fd, "4889842480000000"),  # movq	%rax, 0x80(%rsp)
        (0x52905, "488d05a975f400"),  # leaq	0xf475a9(%rip), %rax
        (0x5290c, "48c784248800000000000000"),  # movq	$0x0, 0x88(%rsp)
        (0x52918, "4889442420"),  # movq	%rax, 0x20(%rsp)
        (0x5291d, "c5f8114c2428"),  # vmovups	%xmm1, 0x28(%rsp)
        (0x52923, "48c744243800000000"),  # movq	$0x0, 0x38(%rsp)
        (0x5292c, "4983c710"),  # addq	$0x10, %r15
        (0x52930, "488d93d0000000"),  # leaq	0xd0(%rbx), %rdx
        (0x52937, "4889df"),  # movq	%rbx, %rdi
        (0x5293a, "4c893b"),  # movq	%r15, (%rbx)
        (0x5293d, "e83e80feff"),  # callq	0x3a980
        (0x52942, "4c89f7"),  # movq	%r14, %rdi
        (0x52945, "e846ccfbff"),  # callq	0xf590
        (0x5294a, "488b842480000000"),  # movq	0x80(%rsp), %rax
        (0x52952, "4c89e7"),  # movq	%r12, %rdi
        (0x52955, "ff5018"),  # callq	*0x18(%rax)
    ],
}
