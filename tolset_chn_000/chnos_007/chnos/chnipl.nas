
[INSTRSET "i486p"]
	org	0x7c00

	jmp	entry
	db	0x90
	db	"chnos   "
	dw	512
	db	1
	dw	1
	db	2
	dw	224
	dw	2880
	db	0xf0
	dw	9
	dw	18
	dw	2
	dd	0
	dd	2880
	db	0, 0, 0x29
	dd	0xffffffff
	db	"chnosboot  "
	db	"fat12   "
	resb	18

entry:
	mov	ax, 0
	mov	ss, ax
	mov	sp, 0x7c00
	mov	ds, ax
	mov	ax, 0x0820
	mov	es, ax
	mov	ch, 0
	mov	dh, 0
	mov	cl, 2
	mov	bx, 18*2-1
	call	readfast

	mov	bx, 4312+0x8200+2
fatloop:
	add	bx, -2
	cmp	word[bx], 0
	jz	fatloop
	mov	ax, bx
	add	ax, -0x8200-5+54+53
	mov	bl, 54
	div	bl
	cmp	al, 0
	jnz	fatskip1
	mov	al, 1
fatskip1:
	cmp	al, 33
	jbe	fatskip2
	mov	al, 33
fatskip2:
	mov	[0x0ff0], al
	add	al, -1
	jz	fatskip3
	mov	ah, 0
	imul	bx, ax, 18*2
	mov	ax, 0x0c80
	mov	es, ax
	mov	ch, 1
	mov	dh, 0
	mov	cl, 1
	call	readfast
fatskip3:
	jmp	0xc200

error:
	mov	ax, 0
	mov	es, ax
	mov	si, msg
putloop:
	mov	al, [si]
	add	si, 1
	cmp	al, 0
	je	fin
	mov	ah, 0x0e
	mov	bx, 15
	int	0x10
	jmp	putloop
fin:
	hlt
	jmp	fin

msg:	db	0x0a, 0x0a, "load error", 0x0a, 0

readfast:
	mov	ax, es
	shl	ax, 3
	and	ah, 0x7f
	mov	al, 128
	sub	al, ah
	mov	ah, bl
	cmp	bh, 0
	je	.skip1
	mov	ah, 18
.skip1:
	cmp	al, ah
	jbe	.skip2
	mov	al, ah
.skip2:
	mov	ah, 19
	sub	ah, cl
	cmp	al, ah
	jbe	.skip3
	mov	al, ah
.skip3:
	push	bx
	mov	si, 0
retry:
	mov	ah, 0x02
	mov	bx, 0
	mov	dl, 0x00
	push	es
	push	dx
	push	cx
	push	ax
	int	0x13
	jnc	next
	add	si, 1
	cmp	si, 5
	jae	error
	mov	ah, 0x00
	mov	dl, 0x00
	int	0x13
	pop	ax
	pop	cx
	pop	dx
	pop	es
	jmp	retry

next:
	pop	ax
	pop	cx
	pop	dx
	pop	bx
	shr	bx, 5
	mov	ah, 0
	add	bx, ax
	shl	bx, 5
	mov	es, bx
	pop	bx
	sub	bx, ax
	jz	.ret
	add	cl, al
	cmp	cl, 18
	jbe	readfast
	mov	cl, 1
	add	dh, 1
	cmp	dh, 2
	jb	readfast
	mov	dh, 0
	add	ch, 1
	jmp	readfast
.ret:
	ret

	resb	0x7dfe-$
	db	0x55, 0xaa
