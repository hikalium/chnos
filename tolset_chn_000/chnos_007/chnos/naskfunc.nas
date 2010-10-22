
[FORMAT "WCOFF"]				; オブジェクトファイルを作るモード	
[INSTRSET "i486p"]				; 486の命令まで使いたいという記述
[BITS 32]					; 32ビットモード用の機械語を作らせる
[FILE "naskfunc.nas"]				; ソースファイル名情報

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
		GLOBAL  _clts, _fnsave, _frstore
		GLOBAL	_pit_beep_on, _pit_beep_off

		GLOBAL	_asm_hrb_api
		EXTERN	_hrb_api

		GLOBAL	_asm_inthandler00, _asm_inthandler01, _asm_inthandler02, _asm_inthandler03, _asm_inthandler04, _asm_inthandler05, _asm_inthandler06, _asm_inthandler07, _asm_inthandler08, _asm_inthandler09, _asm_inthandler0a, _asm_inthandler0b, _asm_inthandler0c, _asm_inthandler0d, _asm_inthandler0e, _asm_inthandler0f, _asm_inthandler10, _asm_inthandler11, _asm_inthandler12, _asm_inthandler13, _asm_inthandler14, _asm_inthandler15, _asm_inthandler16, _asm_inthandler17, _asm_inthandler18, _asm_inthandler19, _asm_inthandler1a, _asm_inthandler1b, _asm_inthandler1c, _asm_inthandler1d, _asm_inthandler1e, _asm_inthandler1f
		GLOBAL	_asm_inthandler21, _asm_inthandler20, _asm_inthandler2c, _asm_inthandler27
		
		EXTERN	_inthandler21, _inthandler20, _inthandler2c, _inthandler27
		EXTERN	_inthandler00, _inthandler01, _inthandler02, _inthandler03, _inthandler04, _inthandler05, _inthandler06, _inthandler07, _inthandler08, _inthandler09, _inthandler0a, _inthandler0b, _inthandler0c, _inthandler0d, _inthandler0e, _inthandler0f, _inthandler10, _inthandler11, _inthandler12, _inthandler13, _inthandler14, _inthandler15, _inthandler16, _inthandler17, _inthandler18, _inthandler19, _inthandler1a, _inthandler1b, _inthandler1c, _inthandler1d, _inthandler1e, _inthandler1f

[SECTION .text]

_asm_hrb_api:
	sti
	pushad
	pushad
	call _hrb_api
	add	esp,32
	popad
	iretd

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
        MOV     AX,SS           ; SSには8が入っているのでそれを使う
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

_frstore:        ; void frstore(int *addr);
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
		PUSHFD		; PUSH EFLAGS という意味
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS という意味
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
		PUSH	EDI						; （EBX, ESI, EDI も使いたいので）
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
;	EAXはtss.esp0の番地
		MOV		ESP,[EAX]
		MOV		DWORD [EAX+4],0
		POPAD
		RET					; cmd_appへ帰る

_start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD		; 32ビットレジスタを全部保存しておく
		MOV		EAX,[ESP+36]	; アプリ用のEIP
		MOV		ECX,[ESP+40]	; アプリ用のCS
		MOV		EDX,[ESP+44]	; アプリ用のESP
		MOV		EBX,[ESP+48]	; アプリ用のDS/SS
		MOV		EBP,[ESP+52]	; tss.esp0の番地
		MOV		[EBP  ],ESP		; OS用のESPを保存
		MOV		[EBP+4],SS		; OS用のSSを保存
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	以下はRETFでアプリに行かせるためのスタック調整
		OR		ECX,3			; アプリ用のセグメント番号に3をORする
		OR		EBX,3			; アプリ用のセグメント番号に3をORする
		PUSH	EBX				; アプリのSS
		PUSH	EDX				; アプリのESP
		PUSH	ECX				; アプリのCS
		PUSH	EAX				; アプリのEIP
		RETF
;	アプリが終了してもここには来ない
