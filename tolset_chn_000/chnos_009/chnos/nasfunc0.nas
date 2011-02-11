[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "nasfunc0.nas"]
	GLOBAL _io_hlt
	GLOBAL _io_cli, _io_sti, _io_stihlt
	GLOBAL _io_in8, _io_out8
	GLOBAL _io_in16, _io_out16
	GLOBAL _io_in32, _io_out32
	GLOBAL _io_load_eflags, _io_store_eflags
	GLOBAL _load_cr0, _store_cr0
	GLOBAL _load_cr1, _store_cr1	;エミュレーターでは使用不可
	GLOBAL _load_cr2, _store_cr2
	GLOBAL _load_cr3, _store_cr3
	GLOBAL _load_cr4, _store_cr4
	GLOBAL _load_gdtr
	GLOBAL _load_idtr
	GLOBAL _load_tr
	GLOBAL _farjmp
	GLOBAL _farcall
	GLOBAL _clts
	GLOBAL _fnsave, _frstor

[SECTION .text]

_io_hlt:
	hlt
	ret

_io_cli:
	cli
	ret

_io_sti:
	sti
	ret

_io_stihlt:
	sti
	hlt
	ret

_io_in8:
	mov	edx,[esp+4]
	mov	eax,0
	in	al,dx
	ret

_io_out8:
	mov	edx,[esp+4]
	mov	al,[esp+8]
	out	dx,al
	ret

_io_in16:
	mov	edx,[esp+4]
	mov	eax,0
	in	ax,dx
	ret

_io_out16:
	mov	edx,[esp+4]
	mov	ax,[esp+8]
	out	dx,ax
	ret

_io_in32:
	mov	edx,[esp+4]
	in	eax,dx
	ret

_io_out32:
	mov	edx,[esp+4]
	mov	eax,[esp+8]
	out	dx,eax
	ret

_io_load_eflags:
	pushfd
	pop	eax
	ret

_io_store_eflags:
	mov	eax,[esp+4]
	push	eax
	popfd
	ret

_load_cr0:
	mov	eax,cr0
	ret

_store_cr0:
	mov	eax,[esp+4]
	mov	cr0,eax
	ret

_load_cr1:
	mov	eax,cr1
	ret

_store_cr1:
	mov	eax,[esp+4]
	mov	cr1,eax
	ret

_load_cr2:
	mov	eax,cr2
	ret

_store_cr2:
	mov	eax,[esp+4]
	mov	cr2,eax
	ret

_load_cr3:
	mov	eax,cr3
	ret

_store_cr3:
	mov	eax,[esp+4]
	mov	cr3,eax
	ret

_load_cr4:
	mov	eax,cr3
	ret

_store_cr4:
	mov	eax,[esp+4]
	mov	cr4,eax
	ret

_load_gdtr:
	mov	ax,[esp+4]
	mov	[esp+6],ax
	lgdt	[esp+6]
	ret

_load_idtr:
	mov	ax,[esp+4]
	mov	[esp+6],ax
	lidt	[esp+6]
	ret

_load_tr:
	ltr	[esp+4]
	ret

_farjmp:
	jmp	far	[esp+4]
	ret

_farcall:
	call	far	[esp+4]
	ret

_clts:
        clts
        ret

_fnsave:
        mov     eax,[esp+4]     ; addr
        fnsave  [eax]
        ret

_frstor:
        mov     eax,[esp+4]     ; addr
        frstor  [eax]
        ret


