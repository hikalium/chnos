[FORMAT "BIN"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]

	mov	ecx,msg
putloop:
	mov	al,[cs:ecx]
	cmp	al,0
	je	fin
	int	0x40
	add	ecx,1
	jmp	putloop
fin:
	retf
msg:
	db	"hello",0
	