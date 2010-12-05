[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "naskfunc.nas"]

	GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
	GLOBAL	_io_in8,  _io_in16,  _io_in32
	GLOBAL	_io_out8, _io_out16, _io_out32
	GLOBAL	_io_load_eflags, _io_store_eflags
	GLOBAL	_load_gdtr, _load_idtr
	GLOBAL	_load_cr0, _store_cr0
	GLOBAL	_load_cr1, _store_cr1
	GLOBAL	_load_cr2, _store_cr2
	GLOBAL	_load_cr3, _store_cr3
	GLOBAL	_load_tr
	GLOBAL	_asm_end_app
	GLOBAL	_memtest_sub
	GLOBAL	_farjmp, _farcall
	GLOBAL	_start_app
	GLOBAL  _asm_osselect_third
	GLOBAL  _clts, _fnsave, _frstor
	GLOBAL	_pit_beep_on, _pit_beep_off
	GLOBAL	_pipelineflush

	GLOBAL	_asm_inthandler00, _asm_inthandler01, _asm_inthandler02, _asm_inthandler03, _asm_inthandler04, _asm_inthandler05, _asm_inthandler06, _asm_inthandler07, _asm_inthandler08, _asm_inthandler09, _asm_inthandler0a, _asm_inthandler0b, _asm_inthandler0c, _asm_inthandler0d, _asm_inthandler0e, _asm_inthandler0f, _asm_inthandler10, _asm_inthandler11, _asm_inthandler12, _asm_inthandler13, _asm_inthandler14, _asm_inthandler15, _asm_inthandler16, _asm_inthandler17, _asm_inthandler18, _asm_inthandler19, _asm_inthandler1a, _asm_inthandler1b, _asm_inthandler1c, _asm_inthandler1d, _asm_inthandler1e, _asm_inthandler1f
	EXTERN	_inthandler00, _inthandler01, _inthandler02, _inthandler03, _inthandler04, _inthandler05, _inthandler06, _inthandler07, _inthandler08, _inthandler09, _inthandler0a, _inthandler0b, _inthandler0c, _inthandler0d, _inthandler0e, _inthandler0f, _inthandler10, _inthandler11, _inthandler12, _inthandler13, _inthandler14, _inthandler15, _inthandler16, _inthandler17, _inthandler18, _inthandler19, _inthandler1a, _inthandler1b, _inthandler1c, _inthandler1d, _inthandler1e, _inthandler1f

	GLOBAL	_asm_inthandler27
	EXTERN	_inthandler27

[SECTION .text]

_pipelineflush:
	jmp	flush
flush:
	ret


_asm_inthandler00:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler00
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler01:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler01
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler02:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler02
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler03:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler03
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler04:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler04
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler05:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler05
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler06:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler06
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler07:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler07
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler08:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler08
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler09:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler09
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler0a:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler0a
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler0b:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler0b
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler0c:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler0c
	cmp	eax,0
	jne	_asm_end_app
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler0d:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler0d
	cmp	eax,0
	jne	_asm_end_app
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler0e:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler0e
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler0f:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler0f
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler10:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler10
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler11:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler11
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler12:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler12
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler13:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler13
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler14:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler14
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler15:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler15
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler16:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler16
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler17:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler17
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler18:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler18
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler19:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler19
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler1a:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler1a
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler1b:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler1b
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler1c:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler1c
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler1d:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler1d
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler1e:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler1e
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler1f:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler1f
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler27:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler27
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_pit_beep_on:
	in	al,0x61
	or	al,0x03
	and	al,0x0f
	out	0x61,al
	ret

_pit_beep_off:
	in	al,0x61
	and	al,0xd
	out	0x61,al
	ret

_asm_osselect_third:
        mov     ax,ss
        mov     fs,ax
        mov     gs,ax
        mov     esp,[esp+4]
        jmp     2*8:0x0000001b

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

_io_in16:
	mov	edx,[esp+4]
	mov	eax,0
	in	ax,dx
	ret

_io_in32:
	mov	edx,[esp+4]
	in	eax,dx
	ret

_io_out8:
	mov	edx,[esp+4]
	mov	al,[esp+8]
	out	dx,al
	ret

_io_out16:
	mov	edx,[esp+4]
	mov	eax,[esp+8]
	out	dx,ax
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

_load_tr:
	ltr	[esp+4]
	ret

_memtest_sub:
	push	edi
	push	esi
	push	ebx
	mov	esi,0xaa55aa55
	mov	edi,0x55aa55aa
	mov	eax,[esp+12+4]
mts_loop:
	mov	ebx,eax
	add	ebx,0xffc
	mov	edx,[ebx]
	mov	[ebx],esi
	xor	dword [ebx],0xffffffff
	cmp	edi,[ebx]
	jne	mts_fin
	xor	dword [ebx],0xffffffff
	cmp	esi,[ebx]
	jne	mts_fin
	mov	[ebx],edx
	add	eax,0x1000
	cmp	eax,[esp+12+8]
	jbe	mts_loop
	pop	ebx
	pop	esi
	pop	edi
	ret
mts_fin:
	mov	[ebx],edx
	pop	ebx
	pop	esi
	pop	edi
	ret

_farjmp:
	jmp	far	[esp+4]
	ret

_farcall:
	call	far	[esp+4]
	ret


_asm_end_app:
	mov	esp,[eax]
	mov	dword [eax+4],0
	popad
	ret

_start_app:
	pushad
	mov	eax,[esp+36]
	mov	ecx,[esp+40]
	mov	edx,[esp+44]
	mov	ebx,[esp+48]
	mov	ebp,[esp+52]
	mov	[ebp  ],esp
	mov	[ebp+4],ss
	mov	es,bx
	mov	ds,bx
	mov	fs,bx
	mov	gs,bx

	or	ecx,3
	or	ebx,3
	push	ebx
	push	edx
	push	ecx
	push	eax
	retf

