	.file	"opt_poly.c"
	.text
	.p2align 4
	.globl	poly_opt
	.type	poly_opt, @function
poly_opt:
.LFB0:
	.cfi_startproc
	endbr64
	movapd	%xmm0, %xmm3
	movapd	%xmm0, %xmm4
	movapd	%xmm0, %xmm5
	mulsd	%xmm0, %xmm3
	movapd	%xmm0, %xmm6
	movapd	%xmm0, %xmm7
	movapd	%xmm0, %xmm8
	leaq	0(,%rsi,8), %r8
	movapd	%xmm0, %xmm9
	leaq	(%rdi,%r8), %rax
	leaq	-1(%rsi), %rdx
	movapd	%xmm0, %xmm2
	movsd	(%rax), %xmm0
	mulsd	%xmm3, %xmm4
	mulsd	%xmm4, %xmm5
	mulsd	%xmm5, %xmm6
	mulsd	%xmm6, %xmm7
	mulsd	%xmm7, %xmm8
	mulsd	%xmm8, %xmm9
	cmpq	$6, %rdx
	jle	.L2
	leaq	-8(%rsi), %rcx
	leaq	-64(%rdi,%r8), %rdx
	shrq	$3, %rcx
	movq	%rcx, %r8
	salq	$6, %r8
	subq	%r8, %rdx
	.p2align 4,,10
	.p2align 3
.L3:
	movsd	-48(%rax), %xmm10
	movapd	%xmm0, %xmm1
	movsd	-56(%rax), %xmm0
	subq	$64, %rax
	mulsd	%xmm9, %xmm1
	mulsd	%xmm3, %xmm10
	mulsd	%xmm2, %xmm0
	addsd	(%rax), %xmm0
	addsd	%xmm10, %xmm0
	movsd	24(%rax), %xmm10
	mulsd	%xmm4, %xmm10
	addsd	%xmm10, %xmm0
	movsd	32(%rax), %xmm10
	mulsd	%xmm5, %xmm10
	addsd	%xmm10, %xmm0
	movsd	40(%rax), %xmm10
	mulsd	%xmm6, %xmm10
	addsd	%xmm10, %xmm0
	movsd	48(%rax), %xmm10
	mulsd	%xmm7, %xmm10
	addsd	%xmm10, %xmm0
	movsd	56(%rax), %xmm10
	mulsd	%xmm8, %xmm10
	addsd	%xmm10, %xmm0
	addsd	%xmm1, %xmm0
	cmpq	%rax, %rdx
	jne	.L3
	negq	%rcx
	leaq	-9(%rsi,%rcx,8), %rdx
.L2:
	testq	%rdx, %rdx
	js	.L1
	.p2align 4,,10
	.p2align 3
.L5:
	mulsd	%xmm2, %xmm0
	addsd	(%rdi,%rdx,8), %xmm0
	subq	$1, %rdx
	cmpq	$-1, %rdx
	jne	.L5
.L1:
	ret
	.cfi_endproc
.LFE0:
	.size	poly_opt, .-poly_opt
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
