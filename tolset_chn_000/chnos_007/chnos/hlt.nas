[FORMAT "BIN"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]

	mov	ax,'A'
	call	2*8:0x24
fin:
	hlt
	jmp	fin
	