

[INSTRSET "i486p"]
VBEMODE	equ		0x114			
; �i��ʃ��[�h�ꗗ�j
;0x0100  640x400  256  
;0x0101  640x480  256  
;0x0102  800x600  16  
;0x0103  800x600  256  
;0x0104  1024x768  16  
;0x0105  1024x768  256  
;0x0106  1280x1024  16  
;0x0107  1280x1024  256  
;0x010D  320x200  ��32000�F(1:5:5:5)  
;0x010E  320x200  ��65000�F(5:6:5)  
;0x010F  320x200  ��1678���F(8:8:8)  
;0x0110  640x480  ��32000�F(1:5:5:5)  
;0x0111  640x480  ��65000�F(5:6:5)  
;0x0112  640x480  ��1678���F(8:8:8)  
;0x0113  800x600  ��32000�F(1:5:5:5)  
;0x0114  800x600  ��65000�F(5:6:5)  
;0x0115  800x600  ��1678���F(8:8:8)  
;0x0116  1024x768  ��32000�F(1:5:5:5)  
;0x0117  1024x768  ��65000�F(5:6:5)  
;0x0118  1024x768  ��1678���F(8:8:8)  
;0x0119  1280x1024  ��32000�F(1:5:5:5)  
;0x011A  1280x1024  ��65000�F(5:6:5)  
;0x011B  1280x1024  ��1678���F(8:8:8)  


BOTPAK	equ		0x00280000		; bootpack�̃��[�h��
DSKCAC	equ		0x00100000		; �f�B�X�N�L���b�V���̏ꏊ
DSKCAC0	equ		0x00008000		; �f�B�X�N�L���b�V���̏ꏊ�i���A�����[�h�j

; BOOT_INFO�֌W
CYLS	equ		0x0ff0			; �u�[�g�Z�N�^���ݒ肷��
LEDS	equ		0x0ff1
VMODE	equ		0x0ff2			; �F���Ɋւ�����B���r�b�g�J���[���H
SCRNX	equ		0x0ff4			; �𑜓x��X
SCRNY	equ		0x0ff6			; �𑜓x��Y
VRAM	equ		0x0ff8			; �O���t�B�b�N�o�b�t�@�̊J�n�Ԓn


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

	call	vbecheck
	call	keyled
	call	pmode


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
	out	0x60, al
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
	xor	cx, cx
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
	db	A20_ENABLE_LOOPS

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
	loop	a20_test_wait

	pop	word [fs:A20_TEST_ADDR]
	pop	ax
	pop	cx
	ret	

empty_8042:
	push	ecx
	mov	ecx, 100000

empty_8042_loop:
	dec	ecx
	jz	empty_8042_end_loop

	call	delay

	in	al, 0x64 		    ; 8042��ԃ|�[�g 
	test	al, 1		; �o�̓o�b�t�@���e�X�g
	jz	no_output

	call	delay
	in	al, 0x60			    ; �ǂ�
	jmp	empty_8042_loop

no_output:
	test	al, 2			    ; ���̓o�b�t�@�������ς��ɂȂ����� 
	jnz	empty_8042_loop		    ; yes ? loop�����

empty_8042_end_loop:
	pop	ecx
	ret

delay:
	out	0x80, al
	ret

vbecheck:

	mov	ax,0xe0
	mov	es,ax
	mov	di,0
	mov	cx,VBEMODE
	mov	ax,0x4f01
	int	0x10

	mov	ax,0x9000
	mov	es,ax
	mov	di,0
	mov	ax,0x4f00
	int	0x10
	cmp	ax,0x004f
	jne	scrn320
	mov	ax,[es:di+4]
	cmp	ax,0x0200
	jb	scrn320
	mov	cx,VBEMODE
	mov	ax,0x4f01
	int	0x10
	cmp	ax,0x004f
	jne	scrn320
	cmp	byte[es:di+0x19],16
	jne	scrn320
	cmp	byte[es:di+0x1b],6
	jne	scrn320
	mov	ax,[es:di+0x00]
	and	ax,0x0080
	jz	scrn320

	mov	bx,VBEMODE+0x4000
	mov	ax,0x4f02
	int	0x10
	mov	byte[VMODE],16
	mov	ax,[es:di+0x12]
	mov	[SCRNX],ax
	mov	ax,[es:di+0x14]
	mov	[SCRNY],ax
	mov	eax,[es:di+0x28]
	mov	[VRAM],eax
	ret

scrn320:
	mov	al,0x13
	mov	ah,0x00
	int	0x10
	mov	byte[VMODE],8
	mov	word[SCRNX],320
	mov	word[SCRNY],200
	mov	dword[VRAM],0x000a0000
	ret

keyled:
	mov	ah,0x02
	int	0x16
	mov	[LEDS],al
	ret

pmode:
	mov	al,0xff
	out	0x21,al
	nop
	out	0xa1,al
	cli

	lgdt	[GDTR0]
	mov	eax,cr0
	and	eax,0x7fffffff
	or	eax,0x00000001
	mov	cr0,eax
	jmp	pipelineflush
pipelineflush:
	mov	ax,1*8
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	mov	ss,ax
	mov	esi,bootpack	;�]����
	mov	edi,BOTPAK	;�]����
	mov	ecx,512*1024/4
	call	memcpy
	mov	esi,0x7c00
	mov	edi,DSKCAC
	mov	ecx,512/4
	call	memcpy
	mov	esi,DSKCAC0+512
	mov	edi,DSKCAC+512
	mov	ecx,0
	mov	cl,byte[CYLS]
	imul	ecx,512*18*2/4
	sub	ecx,512/4
	call	memcpy
	
	mov	ebx,BOTPAK
	mov	ecx,[ebx+16]
	add	ecx,3
	shr	ecx,2
	jz	skip
	mov	esi,[ebx+20]
	add	esi,ebx
	mov	edi,[ebx+12]
	call	memcpy
skip:
	mov	esp,[ebx+12]
	jmp	dword 2*8:0x0000001b


memcpy:
	mov		eax,[esi]
	add		esi,4
	mov		[edi],eax
	add		edi,4
	sub		ecx,1
	jnz		memcpy			; �����Z�������ʂ�0�łȂ����memcpy��
	ret


	



;�f�[�^

msg001:	db	"Welcome to chnos project .",0
msg002:	db	"A20GATE on .",0
msg003:	db	"A20GATE filed .",0
backcc:	db	".",0

		alignb	16
GDT0:
		resb	8				; �k���Z���N�^
		dw		0xffff,0x0000,0x9200,0x00cf	; �ǂݏ����\�Z�O�����g32bit
		dw		0xffff,0x0000,0x9a28,0x0047	; ���s�\�Z�O�����g32bit�ibootpack�p�j

		dw		0
GDTR0:
		dw		8*3-1
		dd		GDT0

		alignb	16
bootpack:



