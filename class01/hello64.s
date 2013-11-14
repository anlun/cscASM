.global main

.text

main:
	movq	$len,	%rdx
	movq	$msg,	%rcx
	movq	$1,		%rbx
	movq	$4,		%rax
	int		$0x80

	movq	$0,		%rbx
	movq	$1,		%rax
	int		$0x80

.data

msg:
	.ascii	"Hello world! By sys_write.\n"
	len = . - msg