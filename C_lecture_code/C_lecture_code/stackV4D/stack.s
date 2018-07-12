	.file	"stack.c"
	.text
	.globl	stack_create
	.type	stack_create, @function
stack_create:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$44, (%esp)
	call	malloc
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	.L2
	movl	8(%ebp), %eax
	movl	$0, (%eax)
	movl	$3, %eax
	jmp	.L3
.L2:
	movl	-12(%ebp), %eax
	movl	$0, 40(%eax)
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	%edx, (%eax)
	movl	$0, %eax
.L3:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	stack_create, .-stack_create
	.globl	stack_free
	.type	stack_free, @function
stack_free:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	free
	movl	8(%ebp), %eax
	movl	$0, (%eax)
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	stack_free, .-stack_free
	.globl	stack_push
	.type	stack_push, @function
stack_push:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %eax
	movl	40(%eax), %eax
	cmpl	$10, %eax
	jne	.L7
	movl	$2, %eax
	jmp	.L8
.L7:
	movl	8(%ebp), %eax
	movl	40(%eax), %eax
	leal	1(%eax), %ecx
	movl	8(%ebp), %edx
	movl	%ecx, 40(%edx)
	movl	8(%ebp), %edx
	movl	12(%ebp), %ecx
	movl	%ecx, (%edx,%eax,4)
	movl	$0, %eax
.L8:
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	stack_push, .-stack_push
	.globl	stack_pop
	.type	stack_pop, @function
stack_pop:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %eax
	movl	40(%eax), %eax
	testl	%eax, %eax
	jne	.L10
	movl	$1, %eax
	jmp	.L11
.L10:
	movl	8(%ebp), %eax
	movl	40(%eax), %eax
	leal	-1(%eax), %edx
	movl	8(%ebp), %eax
	movl	%edx, 40(%eax)
	movl	$0, %eax
.L11:
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	stack_pop, .-stack_pop
	.globl	stack_top
	.type	stack_top, @function
stack_top:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %eax
	movl	40(%eax), %eax
	testl	%eax, %eax
	jne	.L13
	movl	$1, %eax
	jmp	.L14
.L13:
	movl	8(%ebp), %eax
	movl	40(%eax), %eax
	leal	-1(%eax), %edx
	movl	8(%ebp), %eax
	movl	(%eax,%edx,4), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	movl	$0, %eax
.L14:
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	stack_top, .-stack_top
	.globl	stack_size
	.type	stack_size, @function
stack_size:
.LFB7:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %eax
	movl	40(%eax), %eax
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	stack_size, .-stack_size
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.1) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
