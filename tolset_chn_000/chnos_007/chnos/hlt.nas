[FORMAT "BIN"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]

	mov	ax,'h'
	call	2*8:0x24
	mov	ax,'e'
	call	2*8:0x24
	mov	ax,'l'
	call	2*8:0x24
	mov	ax,'l'
	call	2*8:0x24
	mov	ax,'o'
	call	2*8:0x24
	retf
	