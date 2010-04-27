

[INSTRSET "i486p"]
VBEMODE	equ		0x0115	;�W����0x0115
; �i��ʃ��[�h�ꗗ�j
;0x0100  640x400  256  
;0x0101  640x480  256  
;0x0102  800x600  16  *
;0x0103  800x600  256  
;0x0104  1024x768  16  *
;0x0105  1024x768  256  
;0x0106  1280x1024  16  *
;0x0107  1280x1024  256  *
;0x010D  320x200  ��32000�F(1:5:5:5)  *
;0x010E  320x200  ��65000�F(5:6:5)  *
;0x010F  320x200  ��1678���F(8:8:8)  *
;0x0110  640x480  ��32000�F(1:5:5:5)  ?
;0x0111  640x480  ��65000�F(5:6:5)  
;0x0112  640x480  ��1678���F(8:8:8)  
;0x0113  800x600  ��32000�F(1:5:5:5) ? 
;0x0114  800x600  ��65000�F(5:6:5)  
;0x0115  800x600  ��1678���F(8:8:8)  
;0x0116  1024x768  ��32000�F(1:5:5:5)?  
;0x0117  1024x768  ��65000�F(5:6:5)  
;0x0118  1024x768  ��1678���F(8:8:8)  
;0x0119  1280x1024  ��32000�F(1:5:5:5)* 
;0x011A  1280x1024  ��65000�F(5:6:5) * 
;0x011B  1280x1024  ��1678���F(8:8:8)  *


BOTPAK	equ		0x00280000		;bootpack�̃��[�h��
DSKCAC	equ		0x00100000		;�f�B�X�N�L���b�V���̏ꏊ
DSKCAC0	equ		0x00008000		;�f�B�X�N�L���b�V���̏ꏊ�i���A�����[�h�j

; BOOT_INFO�֌W
CYLS	equ		0x0ff0			;�u�[�g�Z�N�^���ݒ肷��
LEDS	equ		0x0ff1			;�L�[�{�[�h�̏�Ԃ��L�^
VMODE	equ		0x0ff2			;�F���Ɋւ�����B���r�b�g�J���[���H�iVESAINFO���g�p���邱�Ƃ𐄏��j
SCRNX	equ		0x0ff4			;�𑜓x��X�iVESAINFO���g�p���邱�Ƃ𐄏��j
SCRNY	equ		0x0ff6			;�𑜓x��Y�iVESAINFO���g�p���邱�Ƃ𐄏��j
VRAM	equ		0x0ff8			;�O���t�B�b�N�o�b�t�@�̊J�n�Ԓn�iVESAINFO���g�p���邱�Ƃ𐄏��j


[BITS 16]

	org	0xc200				;���̃v���O�����́A0xc200�Ԓn����n�܂�
asmhead:					
	mov	ax, 0				;���W�X�^�������Bax��0�ɁB
	mov	ss, ax				;���W�X�^�������Bss��ax�ɁB�܂�Ass��0�ɁB
	mov	sp, 0xc200			;�X�^�b�N�|�C���^���������Bsp��0xc200�ɁB
	mov	ds, ax				;���W�X�^�������Bds��ax�ɁB�܂�Ads��0�ɁB
	mov	es, ax				;���W�X�^�������Bes��ax�ɁB�܂�Aes��0�ɁB

	call	backc				;�e�L�X�g���[�h�w�i�̏������B
	lea	esi, [msg001]    		;msg001�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax		    		;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 0			    	;�����̈ʒu�w��B�[���s�ڂ�0�B��s�ڂ�80*2�B
	call	printf				;printf���Ăяo���B

	call	a20_try_loop			;A20GATE��on�ɂ���B���s����ƁA�߂��Ă��Ȃ��B
	lea	esi, [msg002]			;msg002�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax				;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 80*2			;��ʂ̈�s�ڂ���n�߂�B
	call	printf				;printf���Ăяo���B

	call	vbecheck			;VesaBiosExtention�ŉ�ʃ��[�h�ؑցB���s����ƁA�߂��Ă��Ȃ��B
	call	keyled				;BIOS�𗘗p���āA�L�[�{�[�h��LED�Ȃǂ̏����擾�B
	call	pmode				;�v���e�N�e�b�h�E�o�[�`�����A�h���X���[�h�Ɉڍs�B�߂��Ă��Ȃ��B


;�T�u���[�`��

halthalt:
	lea	esi, [msg005]			;msg003�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax		    		;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 80*2*2*2*2			;��ʂ̎l�s�ڂ���n�߂�B
	call	printf				;printf���Ăяo���B
	call	entkeywait			;�G���^�[�L�[�������܂Ŗ߂��Ă��Ȃ��B
	call	shutdown			;�V���b�g�_�E������B

printf:						;�e�L�X�g���[�h�����\�����[�`��
	push	eax		 		;eax���X�^�b�N�ɕۑ��B
printf_loop:
	mov	al, byte [esi]			;esi�Ԓn�̕�����al�ɑ���B
	mov	byte [es:edi], al		;al����ʂ̎w�肵���Ԓn�ɑ���B
	or	al, al				;al��0�Ȃ̂��𒲂ׂ�B
	jz	printf_end			;0�ł���΁Aprintf_end�ɃW�����v����B
	inc	edi				;0�łȂ���΁Aedi��1���₷
	mov	byte [es:edi], 0x06		;�����̐F�Ɣw�i�F�̒l������B
	inc	esi				;���̕��������o�����߂�esi��1���₷�B
	inc	edi				;��ʂɎ��̕�����\�����邽�߂�edi�𑝂₷�B
	jmp	printf_loop			;printf_loop�ɍs���B����𕶎������J��Ԃ��B
printf_end:
	pop	eax				;�X�^�b�N�ɕۑ����Ă�����eax��eax�ɖ߂��B
	ret					;�Ăяo�����ɖ߂�B

backc:						;�e�L�X�g���[�h�w�i�F�`�惋�[�`��
	mov	ax,0xb800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es,ax				;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	di,0				;��ʂ̈�ԏ��߂���n�߂�B
	mov	ax,word[backcc]			;ax�ɏ������������B
	mov	cx,0x7ff			;0x7ff��J��Ԃ��B
	
paint:						;�e�L�X�g���[�h�w�i�F�`�惋�[�`�����[�v
	mov	word[es:di],ax			;��ʂ̔Ԓn��ax�����B
	add	di,2				;��ʂ̔Ԓn�𑝂₷�B
	dec	cx				;cx�����炵�āA�񐔂𐔂���B
	jnz	paint				;cx���[���ł͂Ȃ�������A�܂��J��Ԃ��B
	ret					;cx���[����������A�Ăяo�����ɖ߂�B

A20_TEST_LOOPS		equ	32 	  	;A20GATE��ON�ɂȂ������e�X�g���郋�[�v�̉񐔁B
A20_ENABLE_LOOPS	equ	255 	 	;A20GATE��ON�ɂȂ�܂ł���΂�S�̂̃��[�v�̉񐔁B 
A20_TEST_ADDR		equ	4*0x80		;A20GATE�e�X�g�Ώۂ̃A�h���X�B


a20_try_loop:					;A20GATE�L�������[�`��
a20_none:
	call	a20_test			;A20GATE�����ł�ON�ɂȂ��Ă��Ȃ������ׂ�B
	jnz	a20_done			;���ł�ON��������K�v�Ȃ��̂ŁA�I���B
a20_bios:					;BIOS (INT 0x15, AX=0x2401)���g�p����A20GATE�L�����������B
	mov	ax, 0x2401			;�@�\�ԍ�0x2401=A20���I���ɂ���B
						;�@�\�ԍ�0x2400=A20���I�t�ɂ���B
	pushfd					;FLAGS���X�^�b�N�ɕۑ��B
	int	0x15				;BIOS INT 0x15
	popfd					;FLAGS���X�^�b�N���畜���B
	call	a20_test			;ON�ɂȂ��������ׂ�B
	jnz	a20_done			;ON�ɂȂ��Ă�����I���B��������Ȃ������瑱���B
a20_kbc:					;�L�[�{�[�h�R���g���[���[�iKBC�j���g�p����A20GATE�L�����������B
	call	empty_8042			;KBC�̃X�e�[�^�X���W�X�^��ǂ�ŁA�f�[�^�|�[�g����ɂȂ�܂ő҂B
	call	a20_test			;�Ȃ�ƂȂ��������AON�ɂȂ��������ׂ�B
	jnz	a20_done			;ON�ɂȂ��Ă�����I���B��������Ȃ������瑱���B
	mov	al, 0xD1			;al��0xd1�����B
	out	0x64, al			;KBC�̃R�}���h�o�́i�|�[�g0x64�j��0xd1�����B���ڑ���ł��Ȃ��̂ŁB
	call	empty_8042			;KBC�̃X�e�[�^�X���W�X�^��ǂ�ŁA�f�[�^�|�[�g����ɂȂ�܂ő҂B
	mov	al, 0xDF			;al��0xdf�����B
	out	0x60, al			;KBC�̃f�[�^�o�́i�|�[�g0x60�j��0x60(A20GATE)
	call	empty_8042			;KBC�̃X�e�[�^�X���W�X�^��ǂ�ŁA�f�[�^�|�[�g����ɂȂ�܂ő҂B
a20_kbc_wait:					;�Ƃɂ����҂B
	xor	cx, cx				;loop�̉񐔂��w��B
a20_kbc_wait_loop:
	call	a20_test			;ON�ɂȂ��������ׂ�B
	jnz	a20_done			;ON�ɂȂ��Ă�����I���B
	loop	a20_kbc_wait_loop		;loop(dec cx�����āA�[���ɂȂ�܂ŉ��x�����[�v)�B
a20_fast:					;�ŏI��i�@system control port�i�|�[�g0x92�j�B
	in	al, 0x92			;system control port��al�ɑ���B
	or	al, 0x02			;al��0x02��or�v�Z����B�܂�Abit1��1�ɂ��Ă���B
						;bit1=1=A20GATE��ON�ɂ���Bbit1=0=A20GATE��OFF�ɂ���B
	and	al, 0xFE			;����ɁAal��0xfe��and�v�Z����B
	out	0x92, al			;system control port��al��߂��B
a20_fast_wait:					;�Ƃɂ����҂B
	xor	cx, cx				;loop�̉񐔂��w��B
a20_fast_wait_loop:
	call	a20_test			;ON�ɂȂ��������ׂ�B
	jnz	a20_done			;ON�ɂȂ��Ă�����I���B
	loop	a20_fast_wait_loop		;loop(dec cx�����āA�[���ɂȂ�܂ŉ��x�����[�v)�B
	dec	byte [a20_tries]		;�S�̂̌J��Ԃ��񐔂���񌸂炷�B
	jnz	a20_try_loop			;�[���ɂȂ�܂ł�����߂��A�ŏ�����B�B�B

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
;	in	al, 0x60			    ; �ǂ�
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

getc:
	mov	ah,0x00
	int	0x16
	ret

vbecheck:
	lea	esi, [msg007]			;msg003�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax		    		;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 80*2*3		;��ʂ̎l�s�ڂ���n�߂�B
	call	printf				;printf���Ăяo���B

	lea	esi, [msg008]			;msg003�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax		    		;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 80*2*4		;��ʂ̎l�s�ڂ���n�߂�B
	call	printf				;printf���Ăяo���B

	lea	esi, [msg009]			;msg003�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax		    		;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 80*2*5		;��ʂ̎l�s�ڂ���n�߂�B
	call	printf				;printf���Ăяo���B

	lea	esi, [msg010]			;msg003�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax		    		;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 80*2*6		;��ʂ̎l�s�ڂ���n�߂�B
	call	printf				;printf���Ăяo���B

	lea	esi, [msg011]			;msg003�̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800			;�A�h���X�̎w��B�e�L�X�g���[�h�́A0xB800����n�܂�B
	mov	es, ax		    		;es��0xB800������Bes�ɒ��ڑ���͂ł��Ȃ��̂ŁB
	mov	edi, 80*2*7		;��ʂ̎l�s�ڂ���n�߂�B
	call	printf				;printf���Ăяo���B

	call	getc

	cmp	ah,0x01
	je	vbe00

	cmp	ah,0x02
	je	vbe01
	cmp	ah,0x03
	je	vbe02
	cmp	ah,0x04
	je	vbe03
	cmp	ah,0x05
	je	vbe04
	cmp	ah,0x06
	je	vbe05
	cmp	ah,0x07
	je	vbe06
	cmp	ah,0x08
	je	vbe07
	cmp	ah,0x09
	je	vbe08
	cmp	ah,0x0a
	je	vbe09

	cmp	ah,0x1e
	je	vbe0a
	cmp	ah,0x30
	je	vbe0b
	cmp	ah,0x2e
	je	vbe0c
	cmp	ah,0x20
	je	vbe0d
	cmp	ah,0x12
	je	vbe0e
	cmp	ah,0x21
	je	vbe0f
	cmp	ah,0x22
	je	vbe0g
	cmp	ah,0x23
	je	vbe0h
	cmp	ah,0x17
	je	vbe0i

	jmp	vbecheck

vbe00:
	mov	al,0x13
	mov	ah,0x00
	int	0x10
	mov	ax,0xe0
	mov	es,ax
	mov	di,0
	mov	word[es:di+0x12],320
	mov	word[es:di+0x14],200
	mov	byte[es:di+0x19],8
	mov	dword[es:di+0x28],0x000a0000
	mov	byte[VMODE],8
	mov	word[SCRNX],320
	mov	word[SCRNY],200
	mov	dword[VRAM],0x000a0000
	ret

vbe01:
	mov	word[videomode],0x010f
	jmp	vbesub
vbe02:
	mov	word[videomode],0x0112
	jmp	vbesub
vbe03:
	mov	word[videomode],0x0115
	jmp	vbesub
vbe04:
	mov	word[videomode],0x0118
	jmp	vbesub
vbe05:
	mov	word[videomode],0x011b
	jmp	vbesub
vbe06:
	mov	word[videomode],0x011f
	jmp	vbesub
vbe07:
	mov	word[videomode],0x010e
	jmp	vbesub
vbe08:
	mov	word[videomode],0x0111
	jmp	vbesub
vbe09:
	mov	word[videomode],0x0114
	jmp	vbesub
vbe0a:
	mov	word[videomode],0x0117
	jmp	vbesub
vbe0b:
	mov	word[videomode],0x011a
	jmp	vbesub
vbe0c:
	mov	word[videomode],0x011e
	jmp	vbesub
vbe0d:
	mov	word[videomode],0x0100
	jmp	vbesub
vbe0e:
	mov	word[videomode],0x0101
	jmp	vbesub
vbe0f:
	mov	word[videomode],0x0103
	jmp	vbesub
vbe0g:
	mov	word[videomode],0x0105
	jmp	vbesub
vbe0h:
	mov	word[videomode],0x0107
	jmp	vbesub
vbe0i:
	mov	word[videomode],0x011c
	jmp	vbesub

vbesub:
	mov	ax,0xe0
	mov	es,ax
	mov	di,0
	mov	cx,[videomode]
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
	mov	cx,[videomode]
	mov	ax,0x4f01
	int	0x10
	cmp	ax,0x004f
	jne	scrn320

	mov	bx,[videomode]
	add	bx,0x4000
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

	lea	esi, [msg004]    ; �����񂪂���Ƃ���̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800
	mov	es, ax		    ; es��0xB800������B
	mov	edi, 80*2*2*2	    ; ��ʂ̓�s�ڂ���n�߂�B
	call	printf
	jmp	halthalt

	mov	al,0x13
	mov	ah,0x00
	int	0x10

	mov	ax,0xe0
	mov	es,ax
	mov	di,0
	mov	word[es:di+0x12],320
	mov	word[es:di+0x14],200
	mov	byte[es:di+0x19],8
	mov	dword[es:di+0x28],0x000a0000

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
	mov	eax,[esi]
	add	esi,4
	mov	[edi],eax
	add	edi,4
	sub	ecx,1
	jnz	memcpy			; �����Z�������ʂ�0�łȂ����memcpy��
	ret

entkeywait:
	mov	ah,0
	int	0x16
	cmp	ah,0x1c
	jne	entkeywait
	ret

shutdown:
	mov	ax,0x5300
	mov	bx,0
	int	0x15
	jc	thend
	mov	ax,0x5301
	mov	bx,0
	int	0x15
	mov	ax,0x530e
	mov	bx,0
	mov	cx,0x0101
	int	0x15
	jc	thend
	mov	ax,0x530f
	mov	bx,0x0001
	mov	cx,0x0001
	int	0x15
	jc	thend
	mov	ax,0x5308
	mov	bx,0x0001
	mov	cx,0x0001
	int	0x15
	jc	thend
	mov	ax,0x5307
	mov	bx,0x0001
	mov	cx,0x0003
	int	0x15
	hlt

thend:
	lea	esi, [msg006]    ; �����񂪂���Ƃ���̃A�h���X��esi�ɑ������B
	mov	ax, 0xB800
	mov	es, ax		    ; es��0xB800������B
	mov	edi, 80*2*2*2*2	    ; ��ʂ̓�s�ڂ���n�߂�B
	call	printf
thend2:
	hlt
	jmp	thend2	

;�f�[�^

msg001:	db	"Welcome to chnos project .",0
msg002:	db	"A20GATE on .",0
msg003:	db	"A20GATE filed .",0
msg004:	db	"Video mode is not supported or invalid.",0
msg005:	db	"Press the Enter key to shut down ...",0
msg006:	db	"Sorry . Shutdown filed . Press power button .",0
msg007:	db	"Video Mode List. Please select the mode number.",0
msg008:	db	"32bit--1:320x200--2:640x480--3:800x600--4:1024x768--5:1280x1024--6:1600x1200",0
msg009:	db	"16bit--7:320x200--8:640x480--9:800x600--a:1024x768--b:1280x1024--c:1600x1200",0
msg010:	db	" 8bit--d:640x400--e:640x480--f:800x600--g:1024x768--h:1280x1024--i:1600x1200",0
msg011:	db	"Press ESC to start in VGA mode.",0
backcc:	db	".",0

videomode:	dw 0

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



