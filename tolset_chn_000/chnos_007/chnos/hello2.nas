[FORMAT "BIN"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	mov	edx,2
	mov	ebx,msg
	int	0x40
	mov	edx,4
	int	0x40
msg:
	db	"hello2",0