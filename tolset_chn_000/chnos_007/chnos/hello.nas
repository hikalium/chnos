[FORMAT "BIN"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]

	mov	ecx,msg
	mov	edx,1
putloop:
	mov	al,[cs:ecx]
	cmp	al,0
	je	fin
	int	0x40
	add	ecx,1
	jmp	putloop
fin:
	mov	edx,4
	int	0x40
msg:
	db	"hello",0
	