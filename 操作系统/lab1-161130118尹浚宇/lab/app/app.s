.code32

.global start
start:
	# TODO
	pushl %ebp
	movl %esp, %ebp
	pushl $13
	pushl $message
	calll displayStr
	leave
	ret

message:
	.string "World, Hello!\n\0"

displayStr:
	movl 4(%esp), %ebx
	movl 8(%esp), %ecx
	movl $((80*8+0)*2), %edi # print at row 8 col 0
	movb $0x0c, %ah
nextChar:
	movb (%ebx), %al
	movw %ax, %gs:(%edi)
	addl $2, %edi
	incl %ebx
	loopnz nextChar # loopnz decrease ecx by 1
	ret
