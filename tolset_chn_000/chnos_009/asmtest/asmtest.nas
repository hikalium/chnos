[INSTRSET "i486p"]
[BITS 32]

	mov	edx, 2
	mov	ebx, msg
	int	0x40
	retf
msg:
	db	"hello", 0

