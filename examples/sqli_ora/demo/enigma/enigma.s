	.file	"enigma.c"
	.text
	.p2align 4,,15
.globl enigma_init
	.type	enigma_init, @function
enigma_init:
.LFB2:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	pushq	%r15
.LCFI2:
	pushq	%r14
.LCFI3:
	pushq	%r13
.LCFI4:
	movq	%rsi, %r13
	pushq	%r12
.LCFI5:
	movl	%edx, %r12d
	pushq	%rbx
.LCFI6:
	movq	%rdi, %rbx
	subq	$8, %rsp
.LCFI7:
	testq	%rdi, %rdi
	sete	%dl
	testq	%rsi, %rsi
	sete	%al
	orl	%edx, %eax
	movl	%eax, %r15d
	andl	$1, %r15d
	jne	.L1
	testl	%r12d, %r12d
	jle	.L46
.L3:
	movl	%r12d, %esi
	movq	%r13, %rdi
	leaq	256(%rbx), %r14
	call	ssh_crc32
	cmpl	%r12d, %r15d
	movl	%eax, %r11d
	leaq	512(%rbx), %r9
	movl	%eax, %esi
	jge	.L26
	movq	%r13, %rcx
	movl	%r12d, %edx
.L7:
	movsbl	(%rcx),%edi
	incq	%rcx
	addl	%edi, %esi
	decl	%edx
	jne	.L7
.L26:
	andl	$255, %esi
	xorl	%ecx, %ecx
	.p2align 4,,7
.L11:
	movslq	%ecx,%r8
	leal	(%rcx,%rsi), %r10d
	incl	%ecx
	cmpl	$255, %ecx
	movb	%r10b, (%r8,%rbx)
	movb	$0, (%r8,%r9)
	jle	.L11
	cmpl	$256, %r12d
	jle	.L12
	movslq	%r12d,%rcx
	movl	$256, %r12d
	leaq	-256(%rcx,%r13), %r13
.L12:
	xorl	%r10d, %r10d
	movl	$255, %r15d
	jmp	.L20
	.p2align 4,,7
.L15:
	incl	%r10d
	cmpl	$255, %r10d
	jg	.L47
.L20:
	movl	%r10d, %eax
	cltd
	idivl	%r12d
	movslq	%edx,%rsi
	movsbl	(%rsi,%r13),%ecx
	leal	(%rcx,%r11,2), %r11d
	movl	%r11d, %edx
	andl	$2147483647, %edx
	movslq	%edx,%r8
	imulq	$-2147254247, %r8, %rax
	shrq	$32, %rax
	leal	(%rdx,%rax), %edi
	sarl	$15, %edi
	imull	$65529, %edi, %esi
	movl	%edx, %edi
	subl	%esi, %edi
	movl	%r15d, %esi
	subl	%r10d, %esi
	movl	%edi, %eax
	leal	1(%rsi), %ecx
	cltd
	movslq	%esi,%r8
	idivl	%ecx
	movzbl	(%r8,%rbx), %ecx
	movslq	%edx,%rdx
	movzbl	(%rdx,%rbx), %eax
	movb	%al, (%r8,%rbx)
	movb	%cl, (%rdx,%rbx)
	cmpb	$0, (%r8,%r9)
	jne	.L15
	movzbl	%dil,%eax
	jmp	.L44
	.p2align 4,,7
.L48:
	leal	1(%rdx), %eax
.L44:
	cltd
	idivl	%esi
	movslq	%edx,%rdi
	cmpb	$0, (%rdi,%r9)
	jne	.L48
	incl	%r10d
	movb	%dl, (%r8,%r9)
	movslq	%edx,%r8
	cmpl	$255, %r10d
	movb	%sil, (%r8,%r9)
	jle	.L20
.L47:
	xorl	%edx, %edx
	movq	%rbx, %rdi
	.p2align 4,,7
.L24:
	movzbq	(%rdi), %rbx
	incq	%rdi
	movb	%dl, (%rbx,%r14)
	incl	%edx
	cmpl	$255, %edx
	jle	.L24
.L1:
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	leave
	ret
.L46:
	movq	%rsi, %rdi
	call	strlen
	movl	%eax, %r12d
	jmp	.L3
.LFE2:
	.size	enigma_init, .-enigma_init
	.p2align 4,,15
.globl enigma
	.type	enigma, @function
enigma:
.LFB3:
	testq	%rdi, %rdi
	movl	%edx, %r9d
	pushq	%rbx
.LCFI8:
	sete	%dl
	testq	%rsi, %rsi
	sete	%al
	orl	%edx, %eax
	movl	%eax, %r11d
	andl	$1, %r11d
	jne	.L49
	testl	%r9d, %r9d
	jle	.L49
	movq	%r9, %rdx
	movl	%r9d, %eax
	movl	%r9d, %ecx
	sarl	$9, %eax
	andl	$1, %edx
	sarl	%ecx
	movzbl	%al, %r8d 
	salq	$8, %rdx
	leaq	512(%rdi), %r10
	addq	%r8, %rdx
	movzbl	%cl, %eax 
	cmpl	%r9d, %r11d
	leaq	256(%rdi), %rbx
	movzbl	(%rdx,%rdi), %r8d
	movzbl	(%rax,%r10), %ecx
	jge	.L49
	movl	%r9d, %edx
	.p2align 4,,7
.L57:
	movzbl	(%rsi), %eax
	addl	%ecx, %eax
	movzbl	%al, %r11d 
	movzbl	(%r11,%rdi), %eax
	addl	%r8d, %eax
	movzbl	%al, %r9d 
	movsbl	(%r9,%r10),%eax
	subl	%r8d, %eax
	movzbl	%al, %r11d 
	movzbl	(%r11,%rbx), %r9d
	subb	%cl, %r9b
	incl	%ecx
	cmpl	$255, %ecx
	movb	%r9b, (%rsi)
	jle	.L54
	leal	1(%r8), %r9d
	andl	$255, %ecx
	xorl	%r8d, %r8d
	cmpl	$256, %r9d
	cmovne	%r9d, %r8d
.L54:
	incq	%rsi
	decl	%edx
	jne	.L57
	.p2align 4,,7
.L49:
	popq	%rbx
	ret
.LFE3:
	.size	enigma, .-enigma
	.p2align 4,,15
.globl enigma_encrypt
	.type	enigma_encrypt, @function
enigma_encrypt:
.LFB4:
	testq	%rdi, %rdi
	movl	%edx, %r9d
	pushq	%rbx
.LCFI9:
	sete	%dl
	testq	%rsi, %rsi
	sete	%al
	orl	%edx, %eax
	movl	%eax, %r11d
	andl	$1, %r11d
	jne	.L61
	testl	%r9d, %r9d
	jle	.L61
	movq	%r9, %rdx
	movl	%r9d, %eax
	movl	%r9d, %ecx
	sarl	$9, %eax
	andl	$1, %edx
	sarl	%ecx
	movzbl	%al, %r8d 
	salq	$8, %rdx
	leaq	512(%rdi), %r10
	addq	%r8, %rdx
	movzbl	%cl, %eax 
	cmpl	%r9d, %r11d
	leaq	256(%rdi), %rbx
	movzbl	(%rdx,%rdi), %r8d
	movzbl	(%rax,%r10), %ecx
	jge	.L61
	.p2align 4,,7
.L69:
	movzbl	(%rsi), %edx
	addl	%ecx, %edx
	movzbl	%dl, %eax 
	movsbl	(%rax,%rdi),%r11d
	leal	(%r11,%r8), %edx
	andl	$31, %r11d
	movzbl	%dl, %eax 
	movsbl	(%rax,%r10),%edx
	subl	%r8d, %edx
	movzbl	%dl, %eax 
	movzbl	(%rax,%rbx), %edx
	subb	%cl, %dl
	leal	1(%r11,%rcx), %ecx
	movb	%dl, (%rsi)
	cmpl	$255, %ecx
	jle	.L66
	leal	1(%r8), %r11d
	andl	$255, %ecx
	xorl	%r8d, %r8d
	cmpl	$256, %r11d
	cmovne	%r11d, %r8d
.L66:
	incq	%rsi
	decl	%r9d
	jne	.L69
	.p2align 4,,7
.L61:
	popq	%rbx
	ret
.LFE4:
	.size	enigma_encrypt, .-enigma_encrypt
	.p2align 4,,15
.globl enigma_decrypt
	.type	enigma_decrypt, @function
enigma_decrypt:
.LFB5:
	testq	%rdi, %rdi
	movl	%edx, %r9d
	pushq	%rbx
.LCFI10:
	sete	%dl
	testq	%rsi, %rsi
	sete	%al
	orl	%edx, %eax
	movl	%eax, %r11d
	andl	$1, %r11d
	jne	.L73
	testl	%r9d, %r9d
	jle	.L73
	movq	%r9, %rdx
	movl	%r9d, %eax
	movl	%r9d, %ecx
	sarl	$9, %eax
	andl	$1, %edx
	sarl	%ecx
	movzbl	%al, %r8d 
	salq	$8, %rdx
	leaq	512(%rdi), %r10
	addq	%r8, %rdx
	movzbl	%cl, %eax 
	cmpl	%r9d, %r11d
	leaq	256(%rdi), %rbx
	movzbl	(%rdx,%rdi), %r8d
	movzbl	(%rax,%r10), %ecx
	jge	.L73
	.p2align 4,,7
.L81:
	movzbl	(%rsi), %edx
	addl	%ecx, %edx
	movzbl	%dl, %eax 
	movzbl	(%rax,%rdi), %edx
	addl	%r8d, %edx
	movzbl	%dl, %r11d 
	movsbl	(%r11,%r10),%edx
	subl	%r8d, %edx
	movzbl	%dl, %eax 
	andl	$31, %edx
	movzbl	(%rax,%rbx), %r11d
	subb	%cl, %r11b
	leal	1(%rdx,%rcx), %ecx
	movb	%r11b, (%rsi)
	cmpl	$255, %ecx
	jle	.L78
	leal	1(%r8), %r11d
	andl	$255, %ecx
	xorl	%r8d, %r8d
	cmpl	$256, %r11d
	cmovne	%r11d, %r8d
.L78:
	incq	%rsi
	decl	%r9d
	jne	.L81
	.p2align 4,,7
.L73:
	popq	%rbx
	ret
.LFE5:
	.size	enigma_decrypt, .-enigma_decrypt
	.section	.eh_frame,"a",@unwind
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.quad	.LFB2
	.quad	.LFE2-.LFB2
	.byte	0x4
	.long	.LCFI0-.LFB2
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.long	.LCFI4-.LCFI1
	.byte	0x8d
	.uleb128 0x5
	.byte	0x8e
	.uleb128 0x4
	.byte	0x8f
	.uleb128 0x3
	.byte	0x4
	.long	.LCFI5-.LCFI4
	.byte	0x8c
	.uleb128 0x6
	.byte	0x4
	.long	.LCFI6-.LCFI5
	.byte	0x83
	.uleb128 0x7
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.quad	.LFB3
	.quad	.LFE3-.LFB3
	.byte	0x4
	.long	.LCFI8-.LFB3
	.byte	0xe
	.uleb128 0x10
	.byte	0x83
	.uleb128 0x2
	.align 8
.LEFDE3:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.quad	.LFB4
	.quad	.LFE4-.LFB4
	.byte	0x4
	.long	.LCFI9-.LFB4
	.byte	0xe
	.uleb128 0x10
	.byte	0x83
	.uleb128 0x2
	.align 8
.LEFDE5:
.LSFDE7:
	.long	.LEFDE7-.LASFDE7
.LASFDE7:
	.long	.LASFDE7-.Lframe1
	.quad	.LFB5
	.quad	.LFE5-.LFB5
	.byte	0x4
	.long	.LCFI10-.LFB5
	.byte	0xe
	.uleb128 0x10
	.byte	0x83
	.uleb128 0x2
	.align 8
.LEFDE7:
	.ident	"GCC: (GNU) 3.4.3 (csl-sol210-3_4-branch+sol_rpath)"
