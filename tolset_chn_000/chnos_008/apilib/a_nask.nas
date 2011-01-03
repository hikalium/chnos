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

_api_putchar:
	mov	edx,1
	mov	al,[esp+4]
	int	0x40
	ret

_api_putstr:
	push	ebx
	mov	edx,2
	mov	ebx,[esp+8]
	int	0x40
	pop	ebx
	ret

_api_end:
	mov	edx,4
	int	0x40

_api_openwin:
;col_invは無視されます。bufはunsigned charのみ許可。
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

_api_putstrwin:
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

_api_boxfilwin:
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


_api_initmalloc:
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

_api_malloc:
	push	ebx
	mov	edx,9
	mov	ebx,[cs:0x0020]
	mov	ecx,[esp+8]
	int	0x40
	pop	ebx
	ret

_api_free:
	push	ebx
	mov	edx,10
	mov	ebx,[cs:0x0020]
	mov	eax,[esp+8]
	mov	ecx,[esp+12]
	int	0x40
	pop	ebx
	ret

_api_point:
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

_api_linewin:
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

