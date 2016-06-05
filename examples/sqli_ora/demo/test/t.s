	.file	"t.c"
	.text
.globl STRDUP
	.type	STRDUP, @function
STRDUP:
.LFB2:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	pushq	%rbx
.LCFI2:
	subq	$40, %rsp
.LCFI3:
	movq	%rdi, -32(%rbp)
	movq	%rsp, %rax
	movq	%rax, %rbx
	movq	-32(%rbp), %rax
	movq	$-1, %rcx
	movq	%rax, -48(%rbp)
	movl	$0, %eax
	cld
	movq	-48(%rbp), %rdi
	repnz
	scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	$1, %rax
	addq	$15, %rax
	addq	$15, %rax
	shrq	$4, %rax
	salq	$4, %rax
	subq	%rax, %rsp
	movq	%rsp, -40(%rbp)
	movq	-40(%rbp), %rax
	addq	$15, %rax
	shrq	$4, %rax
	salq	$4, %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	movq	-32(%rbp), %rsi
	call	strcpy
	movq	-16(%rbp), %rax
	movq	%rbx, %rsp
	movq	-8(%rbp), %rbx
	leave
	ret
.LFE2:
	.size	STRDUP, .-STRDUP
.globl fo
	.type	fo, @function
fo:
.LFB3:
	pushq	%rbp
.LCFI4:
	movq	%rsp, %rbp
.LCFI5:
	movss	%xmm0, -4(%rbp)
	movss	-4(%rbp), %xmm0
	addss	%xmm0, %xmm0
	cvttss2si	%xmm0, %eax
	leave
	ret
.LFE3:
	.size	fo, .-fo
	.section	.rodata
.LC0:
	.string	"abcdef"
.LC1:
	.string	"stack_dup=%s\n"
.LC3:
	.string	"%f,%d\n"
.LC4:
	.string	"\302A\322|\342|\262|aaaaAAAA"
.LC5:
	.string	"buf=%s\n"
.LC6:
	.string	"tolower buf=%s\n"
	.text
.globl main
	.type	main, @function
main:
.LFB4:
	pushq	%rbp
.LCFI6:
	movq	%rsp, %rbp
.LCFI7:
	subq	$144, %rsp
.LCFI8:
	movl	$.LC0, %edi
	call	STRDUP
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rsi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	movl	$0x3f800000, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -132(%rbp)
	movss	-132(%rbp), %xmm0
	call	fo
	movl	%eax, %esi
	cvtss2sd	-16(%rbp), %xmm0
	movl	$.LC3, %edi
	movl	$1, %eax
	call	printf
	movl	$.LC4, %edi
	call	puts
	leaq	-128(%rbp), %rax
	movl	$2094154178, (%rax)
	movl	$2092072162, 4(%rax)
	movl	$1633771873, 8(%rax)
	movl	$1094795585, 12(%rax)
	movw	$10, 16(%rax)
	leaq	-128(%rbp), %rsi
	movl	$.LC5, %edi
	movl	$0, %eax
	call	printf
	leaq	-128(%rbp), %rdi
	movl	$0, %eax
	call	strlower
	leaq	-128(%rbp), %rsi
	movl	$.LC6, %edi
	movl	$0, %eax
	call	printf
	leave
	ret
.LFE4:
	.size	main, .-main
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	"zR"
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.uleb128 0x1
	.byte	0x3
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
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x0
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
	.long	.LCFI3-.LCFI1
	.byte	0x83
	.uleb128 0x3
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.long	.LFB3
	.long	.LFE3-.LFB3
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI4-.LFB3
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI5-.LCFI4
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE3:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.long	.LFB4
	.long	.LFE4-.LFB4
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI6-.LFB4
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI7-.LCFI6
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE5:
	.ident	"GCC: (GNU) 4.1.2 20070115 (prerelease) (SUSE Linux)"
	.section	.note.GNU-stack,"",@progbits
