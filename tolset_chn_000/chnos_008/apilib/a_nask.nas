[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
[FILE "a_nask.nas"]
	GLOBAL	_api_putchar
	GLOBAL	_api_putstr
	GLOBAL	_api_end
	GLOBAL	_api_openwin
	GLOBAL	_api_putstrwin
	GLOBAL	_api_boxfilwin
	GLOBAL	_api_initmalloc
	GLOBAL	_api_malloc
	GLOBAL	_api_free
	GLOBAL	_api_point
	GLOBAL	_api_linewin

[SECTION .text]

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

_api_openwin:	;unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
;col_invは無視されます。また、bufはintのサイズで用意して下さい。
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

_api_putstrwin:	;void api_putstrwin(unsigned int win, int x, int y, unsigned int c, int len, unsigned char *s);
;lenは無視されます。
	push	edi
	push	esi
	push	ebp
	push	ebx
	mov	edx,6
	mov	ebx,[esp+20]
	mov	esi,[esp+24]
	mov	edi,[esp+28]
	mov	eax,[esp+32]
	mov	ecx,[esp+36]
	mov	ebp,[esp+40]
	int	0x40
	pop	ebx
	pop	ebp
	pop	esi
	pop	edi
	ret

_api_boxfilwin:	;void api_boxfilwin(unsigned int win, int x0, int y0, int x1, int y1, unsigned int c);
;色指定はとりあえず32ビットで行ってください。
	push	edi
	push	esi
	push	ebp
	push	ebx
	mov	edx,7
	mov	ebx,[esp+20]
	mov	eax,[esp+24]
	mov	ecx,[esp+28]
	mov	esi,[esp+32]
	mov	edi,[esp+36]
	mov	ebp,[esp+40]
	int	0x40
	pop	ebx
	pop	ebp
	pop	esi
	pop	edi
	ret


_api_initmalloc:	;void api_initmalloc(void);
	push	ebx
	mov	edx,8
	mov	ebx,[cs:0x0020]
	mov	eax,ebx
	add	eax,32*1024
	mov	ecx,[cs:0x0000]
	sub	ecx,eax
	int	0x40
	pop	ebx
	ret

_api_malloc:	;void *api_malloc(unsigned int size);
	push	ebx
	mov	edx,9
	mov	ebx,[cs:0x0020]
	mov	ecx,[esp+8]
	int	0x40
	pop	ebx
	ret

_api_free:	;void api_free(void *addr, unsigned int size)
	push	ebx
	mov	edx,10
	mov	ebx,[cs:0x0020]
	mov	eax,[esp+8]
	mov	ecx,[esp+12]
	int	0x40
	pop	ebx
	ret

_api_point:	;void api_point(unsigned int win, int x, int y, int c);
;色指定はとりあえず32ビットで行ってください。
	push	edi
	push	esi
	push	ebx
	mov	edx,11
	mov	ebx,[esp+16]
	mov	esi,[esp+20]
	mov	edi,[esp+24]
	mov	eax,[esp+28]
	int	0x40
	pop	ebx
	pop	esi
	pop	edi
	ret	

_api_linewin:	;void api_linewin(unsigned int win, int x0, int y0, int x1, int y1, int c);
	push	edi
	push	esi
	push	ebp
	push	ebx
	mov	edx,13
	mov	ebx,[esp+20]
	mov	eax,[esp+24]
	mov	ecx,[esp+28]
	mov	esi,[esp+32]
	mov	edi,[esp+36]
	mov	ebp,[esp+40]
	int	0x40
	pop	ebx
	pop	ebp
	pop	esi
	pop	edi
	ret

