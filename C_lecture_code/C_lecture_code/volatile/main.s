	.file	"main.c"
	.section	.rodata
.LC0:
	.string	"Loop used %f seconds.\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$32, %esp
	movl	$0, (%esp)
	call	time
	movl	%eax, 24(%esp)
	movl	$0, 20(%esp)
	jmp	.L2
.L3:
	movl	20(%esp), %eax
	addl	$1, %eax
	movl	%eax, 20(%esp)
.L2:
	movl	20(%esp), %eax
	cmpl	$999999999, %eax
	jbe	.L3
	movl	$0, (%esp)
	call	time
	movl	%eax, 28(%esp)
	movl	24(%esp), %eax
	movl	%eax, 4(%esp)
	movl	28(%esp), %eax
	movl	%eax, (%esp)
	call	difftime
	fstpl	4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
