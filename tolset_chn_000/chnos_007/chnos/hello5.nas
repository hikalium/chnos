[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
[FILE "hello5.nas"]

	GLOBAL	_CHNMain

[SECTION .text]

_CHNMain:
	mov	edx,2
	mov	ebx,msg
	int	0x40
	mov	edx,4
	int	0x40

[SECTION .data]

msg:
	db	"Hello World 5 asm", 0x0a, 0