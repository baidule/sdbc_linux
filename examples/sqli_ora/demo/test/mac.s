	.file	"mac.c"
	.section	.rodata
.LC0:
	.string	"%02x"
.LC1:
	.string	"%c"
	.text
.globl getmac
	.type	getmac, @function
getmac:
.LFB5:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	subq	$160, %rsp
.LCFI2:
	movq	%rdi, -152(%rbp)
	movl	%esi, -156(%rbp)
	leaq	-64(%rbp), %rax
	movq	$812151909, (%rax)
	movq	$0, 8(%rax)
	movw	$1, -48(%rbp)
	leaq	-64(%rbp), %rdx
	movl	-156(%rbp), %edi
	movl	$35111, %esi
	movl	$0, %eax
	call	ioctl
	cmpl	$-1, %eax
	jne	.L2
	movl	$-1, -160(%rbp)
	jmp	.L4
.L2:
	movq	-152(%rbp), %rax
	movq	%rax, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L5
.L6:
	leaq	-64(%rbp), %rax
	addq	$18, %rax
	movq	%rax, -8(%rbp)
	movl	-20(%rbp), %eax
	cltq
	addq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movzbl	%al, %edx
	movq	-16(%rbp), %rdi
	movl	$.LC0, %esi
	movl	$0, %eax
	call	sprintf
	cltq
	addq	%rax, -16(%rbp)
	cmpl	$5, -20(%rbp)
	je	.L7
	movq	-16(%rbp), %rdi
	movl	$58, %edx
	movl	$.LC1, %esi
	movl	$0, %eax
	call	sprintf
	cltq
	addq	%rax, -16(%rbp)
.L7:
	addl	$1, -20(%rbp)
.L5:
	cmpl	$5, -20(%rbp)
	jle	.L6
	movl	$0, -160(%rbp)
.L4:
	movl	-160(%rbp), %eax
	leave
	ret
.LFE5:
	.size	getmac, .-getmac
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
	.long	.LFB5
	.long	.LFE5-.LFB5
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI0-.LFB5
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE1:
	.ident	"GCC: (GNU) 4.1.2 20070115 (prerelease) (SUSE Linux)"
	.section	.note.GNU-stack,"",@progbits
