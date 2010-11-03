[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
[FILE "a_nask.nas"]
	GLOBAL	_api_openwin
	GLOBAL	_api_putchar
	GLOBAL	_api_putstr
	GLOBAL	_api_end

[SECTION .text]

_api_openwin:	;unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
	push	edi
	push	esi
	push	ebx
	mov	edx,5
	mov	ebx,[esp+16]
	mov	esi,[esp+20]
	mov	edi,[esp+24]
	mov	eax,[esp+28]
	mov	ecx,[esp+32]
	int	0x40
	pop	ebx
	pop	esi
	pop	edi
	ret

_api_putchar:	;void api_putchar(unsigned char c);
	mov	edx,1
	mov	al,[esp+4]
	int	0x40
	ret

_api_putstr:	;void api_putstr(unsigned char *s);
	push	ebx
	mov	edx,2
	mov	ebx,[esp+8]
	int	0x40
	pop	ebx
	ret

_api_end:	;void api_end(void);
	mov	edx,4
	int	0x40