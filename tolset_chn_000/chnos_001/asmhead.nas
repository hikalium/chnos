

[INSTRSET "i486p"]
[BITS 16]


	org	0xc200
asmhead:

	mov	ax, 0
	mov	ss, ax
	mov	sp, 0xc200
	mov	ds, ax
	mov	es, ax

	call	backc
	lea	esi, [msg001]    ; �����񂪂���Ƃ���̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800
	mov	es, ax		    ; es��0xB800������B
	mov	edi, 0		    ; ��ʂ̈�ԑO����n�߂�B
	call	printf
	call	a20_try_loop
	lea	esi, [msg002]    ; �����񂪂���Ƃ���̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800
	mov	es, ax		    ; es��0xB800������B
	mov	edi, 80*2	    ; ��ʂ̓�s�ڂ���n�߂�B
	call	printf


	call	halthalt



;	�T�u���[�`��

halthalt:
	hlt
	jmp	halthalt

printf:
	push	eax		    ; ���łɂ�����eax�l���X�^�b�N�ɕۑ����Ă����B

printf_loop:
	mov	al, byte [esi]	    ; esi���w���Ă���A�h���X���當����1�����Ă���B
	mov	byte [es:edi], al   ; ��������ʂɕ\������
	or	al, al		    ; al��0�Ȃ̂��𒲂ׂ�B
	jz	printf_end	    ; 0�ł���΁Aprintf_end�ɃW�����v����B
	inc	edi		    ; 0�łȂ���΁Aedi��1���₵��
	mov	byte [es:edi], 0x06 ; �����̐F�Ɣw�i�F�̒l������B
	inc	esi		    ; ���̕��������o�����߂�esi��1���₷�B
	inc	edi		    ; ��ʂɎ��̕�����\�����邽�߂�edi�𑝂₷�B
	jmp	printf_loop	    ; ���[�v�����B

printf_end:
	pop	eax		    ; �X�^�b�N�ɕۑ����Ă�����eax���Ăю��o���B
	ret			    ; �Ăяo�����ɖ߂�B

backc:
	mov	ax,0xb800
	mov	es,ax
	mov	di,0
	mov	ax,word[backcc]
	mov	cx,0x7ff
	
paint:
	mov	word[es:di],ax
	add	di,2
	dec	cx
	jnz	paint
	ret

A20_TEST_LOOPS		equ  32 	  ; wait���邽�тɌJ��Ԃ���
A20_ENABLE_LOOPS	equ 255 	  ;  �e�X�g���� loop �̍��v
A20_TEST_ADDR		equ 4*0x80


a20_try_loop:

; �܂��A�R���s���[�^��A20���Ȃ������ׂ�
a20_none:
	call	a20_test
	jnz	a20_done

; ����	BIOS (INT 0x15, AX=0x2401)���e�X�g���Ă݂�
	a20_bios:
	mov	ax, 0x2401 
	pushfd				    ; FLAG��������Ɛ[���^��
	int	0x15
	popfd

	call	a20_test
	jnz	a20_done
	
; �L�[�{�[�h�R���g���[����ʂ���A20��ON�ɂ��Ă݂�
a20_kbc:
	call	empty_8042

	call	a20_test		    ; BIOS�@�\�Ŏ{����Ă���ꍇ 
	jnz	a20_done		    ; delay���ꂽ����

	mov	al, 0xD1		    ; command�����C�g����
	out	0x64, al
	call	empty_8042

	mov	al, 0xDF		    ; A20 on
	out    0x60, al
	call	empty_8042

; A20���{����ON�ɂȂ�܂ő҂B����͂���V�X�e���ł�
; �����������Ԃ��|��\��������
; Toshiba Tecras�͂����������������Ă���ƌ�����
a20_kbc_wait:
	xor	cx, cx
a20_kbc_wait_loop:
	call	a20_test
	jnz	a20_done
	loop	a20_kbc_wait_loop

; �Ō�̎���: "����|�[�g A"
a20_fast:
	in	al, 0x92		    ; ����|�[�gA
	or	al, 0x02		    ; "fast A20" �o�[�W������
	and	al, 0xFE		    ; �}��RESET����Ȃ�
	out	0x92, al

; ����|�[�gA�Ɍ��ʂ��o��̂�҂�
a20_fast_wait:
	xor    cx, cx
a20_fast_wait_loop:
	call	a20_test
	jnz	a20_done
	loop	a20_fast_wait_loop

; A20���܂������������Ȃ��B�J��Ԃ��Ă݂�
; 
	dec	byte [a20_tries]
	jnz	a20_try_loop

a20_die:
	lea	esi, [msg003]    ; �����񂪂���Ƃ���̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800
	mov	es, ax		    ; es��0xB800������B
	mov	edi, 80*2	    ; ��ʂ̓�s�ڂ���n�߂�B
	call	printf
	jmp	halthalt

a20_tries:
	db   A20_ENABLE_LOOPS

; �����܂ŗ���ƁAA20��ON�ɂȂ����Ƃ��Ă����B
a20_done:
	ret

a20_test:
	push	cx
	push	ax
	xor	cx, cx
	mov	fs, cx			    ; Low memory
	dec	cx
	mov	gs, cx			    ; High memory area
	mov	cx, A20_TEST_LOOPS
	mov	ax, word [fs:A20_TEST_ADDR]
	push	ax
a20_test_wait:
	inc	ax
	mov	word [fs:A20_TEST_ADDR], ax
	call	delay				
	cmp	ax, word [gs:A20_TEST_ADDR+0x10]
	loop   a20_test_wait

	pop	word [fs:A20_TEST_ADDR]
	pop	ax
	pop	cx
	ret	

empty_8042:
	push   ecx
	mov    ecx, 100000

empty_8042_loop:
	dec    ecx
	jz     empty_8042_end_loop

	call   delay

	in     al, 0x64 		    ; 8042��ԃ|�[�g 
	test   al, 1			    ; �o�̓o�b�t�@���e�X�g
	jz     no_output

	call   delay
	in    al, 0x60			    ; �ǂ�
	jmp    empty_8042_loop

no_output:
	test   al, 2			    ; ���̓o�b�t�@�������ς��ɂȂ����� 
	jnz    empty_8042_loop		    ; yes ? loop�����

empty_8042_end_loop:
	pop    ecx
	ret

delay:
	out    0x80, al
	ret


;�f�[�^

msg001:	db	"Welcome to chnos project .",0
msg002:	db	"A20GATE on .",0
msg003:	db	"A20GATE filed .",0
backcc:	db	".",0



