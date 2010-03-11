

[FORMAT "WCOFF"]				; �I�u�W�F�N�g�t�@�C������郂�[�h	
[INSTRSET "i486p"]				; 486�̖��߂܂Ŏg�������Ƃ����L�q
[BITS 32]						; 32�r�b�g���[�h�p�̋@�B�����点��
[FILE "naskfunc.nas"]			; �\�[�X�t�@�C�������

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL	_asm_end_app
		GLOBAL	_memtest_sub
		GLOBAL	_farjmp, _farcall
		GLOBAL	_start_app
		GLOBAL  _asm_osselect_third
		GLOBAL  _clts, _fnsave, _frstor
		GLOBAL	_pit_beep_on, _pit_beep_off
		GLOBAL	_asm_inthandler21, _asm_inthandler20, _asm_inthandler2c
		
		EXTERN	_inthandler21, _inthandler20, _inthandler2c



[SECTION .text]
_asm_inthandler21:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler21
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler20:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler20
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_inthandler2c:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthandler2c
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

_asm_osselect_third:   ; void asm_osselect_third(int esp);
        MOV     AX,SS           ; SS�ɂ�8�������Ă���̂ł�����g��
        MOV     FS,AX
        MOV     GS,AX
        MOV     ESP,[ESP+4]
        JMP     2*8:0x0000001b

_clts:          ; void clts(void);
        CLTS
        RET

_fnsave:        ; void fnsave(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FNSAVE  [EAX]
        RET

_frstor:        ; void frstor(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FRSTOR  [EAX]
        RET


_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS �Ƃ����Ӗ�
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS �Ƃ����Ӗ�
		RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]			; tr
		RET

_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; �iEBX, ESI, EDI ���g�������̂Łj
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_farjmp:		; void farjmp(int eip, int cs);
		JMP		FAR	[ESP+4]				; eip, cs
		RET

_farcall:		; void farcall(int eip, int cs);
		CALL	FAR	[ESP+4]				; eip, cs
		RET


_asm_end_app:
;	EAX��tss.esp0�̔Ԓn
		MOV		ESP,[EAX]
		MOV		DWORD [EAX+4],0
		POPAD
		RET					; cmd_app�֋A��

_start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD		; 32�r�b�g���W�X�^��S���ۑ����Ă���
		MOV		EAX,[ESP+36]	; �A�v���p��EIP
		MOV		ECX,[ESP+40]	; �A�v���p��CS
		MOV		EDX,[ESP+44]	; �A�v���p��ESP
		MOV		EBX,[ESP+48]	; �A�v���p��DS/SS
		MOV		EBP,[ESP+52]	; tss.esp0�̔Ԓn
		MOV		[EBP  ],ESP		; OS�p��ESP��ۑ�
		MOV		[EBP+4],SS		; OS�p��SS��ۑ�
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	�ȉ���RETF�ŃA�v���ɍs�����邽�߂̃X�^�b�N����
		OR		ECX,3			; �A�v���p�̃Z�O�����g�ԍ���3��OR����
		OR		EBX,3			; �A�v���p�̃Z�O�����g�ԍ���3��OR����
		PUSH	EBX				; �A�v����SS
		PUSH	EDX				; �A�v����ESP
		PUSH	ECX				; �A�v����CS
		PUSH	EAX				; �A�v����EIP
		RETF
;	�A�v�����I�����Ă������ɂ͗��Ȃ�
