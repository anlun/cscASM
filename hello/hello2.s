.global main

.code32

.text

main:
	pushl	$msg
	call	printf

	movl	$0,%ebx
	movl	$1,%eax
	int		$0x80

.data

msg:
	.string "Hello world! By printf.\n"
	len = . - msg