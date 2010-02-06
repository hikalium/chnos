

[INSTRSET "i486p"]
VBEMODE	equ		0x0114			
; （画面モード一覧）
;0x0100  640x400  256  
;0x0101  640x480  256  
;0x0102  800x600  16  
;0x0103  800x600  256  
;0x0104  1024x768  16  
;0x0105  1024x768  256  
;0x0106  1280x1024  16  
;0x0107  1280x1024  256  
;0x010D  320x200  約32000色(1:5:5:5)  
;0x010E  320x200  約65000色(5:6:5)  
;0x010F  320x200  約1678万色(8:8:8)  
;0x0110  640x480  約32000色(1:5:5:5)  
;0x0111  640x480  約65000色(5:6:5)  
;0x0112  640x480  約1678万色(8:8:8)  
;0x0113  800x600  約32000色(1:5:5:5)  
;0x0114  800x600  約65000色(5:6:5)  
;0x0115  800x600  約1678万色(8:8:8)  
;0x0116  1024x768  約32000色(1:5:5:5)  
;0x0117  1024x768  約65000色(5:6:5)  
;0x0118  1024x768  約1678万色(8:8:8)  
;0x0119  1280x1024  約32000色(1:5:5:5)  
;0x011A  1280x1024  約65000色(5:6:5)  
;0x011B  1280x1024  約1678万色(8:8:8)  


BOTPAK	equ		0x00280000		; bootpackのロード先
DSKCAC	equ		0x00100000		; ディスクキャッシュの場所
DSKCAC0	equ		0x00008000		; ディスクキャッシュの場所（リアルモード）

; BOOT_INFO関係
CYLS	equ		0x0ff0			; ブートセクタが設定する
LEDS	equ		0x0ff1
VMODE	equ		0x0ff2			; 色数に関する情報。何ビットカラーか？
SCRNX	equ		0x0ff4			; 解像度のX
SCRNY	equ		0x0ff6			; 解像度のY
VRAM	equ		0x0ff8			; グラフィックバッファの開始番地


[BITS 16]

	org	0xc200
asmhead:
	mov	ax, 0
	mov	ss, ax
	mov	sp, 0xc200
	mov	ds, ax
	mov	es, ax

	call	backc
	lea	esi, [msg001]    ; 文字列があるところのアドレスをesiに代入する。
	mov	ax, 0xB800
	mov	es, ax		    ; esに0xB800を入れる。
	mov	edi, 0		    ; 画面の一番前から始める。
	call	printf

	call	a20_try_loop
	lea	esi, [msg002]    ; 文字列があるところのアドレスをesiに代入する。
	mov	ax, 0xB800
	mov	es, ax		    ; esに0xB800を入れる。
	mov	edi, 80*2	    ; 画面の二行目から始める。
	call	printf

	call	vbecheck
	call	keyled
	call	pmode


	call	halthalt



;	サブルーチン

halthalt:
	lea	esi, [msg005]    ; 文字列があるところのアドレスをesiに代入する。
	mov	ax, 0xB800
	mov	es, ax		    ; esに0xB800を入れる。
	mov	edi, 80*2*2*2*2	    ; 画面の二行目から始める。
	call	printf	
	call	entkeywait
	call	shutdown

printf:
	push	eax		    ; すでにあったeax値をスタックに保存しておく。

printf_loop:
	mov	al, byte [esi]	    ; esiが指しているアドレスから文字を1つ持ってくる。
	mov	byte [es:edi], al   ; 文字を画面に表示する
	or	al, al		    ; alが0なのかを調べる。
	jz	printf_end	    ; 0であれば、printf_endにジャンプする。
	inc	edi		    ; 0でなければ、ediを1つ増やして
	mov	byte [es:edi], 0x06 ; 文字の色と背景色の値を入れる。
	inc	esi		    ; 次の文字を取り出すためにesiを1つ増やす。
	inc	edi		    ; 画面に次の文字を表示するためにediを増やす。
	jmp	printf_loop	    ; ループを回る。

printf_end:
	pop	eax		    ; スタックに保存しておいたeaxを再び取り出す。
	ret			    ; 呼び出し元に戻る。

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

A20_TEST_LOOPS		equ  32 	  ; waitするたびに繰り返す回数
A20_ENABLE_LOOPS	equ 255 	  ;  テストする loop の合計
A20_TEST_ADDR		equ 4*0x80


a20_try_loop:

; まず、コンピュータにA20がないか調べる
a20_none:
	call	a20_test
	jnz	a20_done

; 次に	BIOS (INT 0x15, AX=0x2401)をテストしてみる
	a20_bios:
	mov	ax, 0x2401 
	pushfd				    ; FLAGをちょっと深く疑う
	int	0x15
	popfd

	call	a20_test
	jnz	a20_done
	
; キーボードコントローラを通じてA20をONにしてみる
a20_kbc:
	call	empty_8042

	call	a20_test		    ; BIOS機能で施されている場合 
	jnz	a20_done		    ; delayされた反応

	mov	al, 0xD1		    ; commandをライトする
	out	0x64, al
	call	empty_8042

	mov	al, 0xDF		    ; A20 on
	out	0x60, al
	call	empty_8042

; A20が本当にONになるまで待つ。これはあるシステムでは
; 相当長い時間が掛る可能性がある
; Toshiba Tecrasはこういう問題を持っていると言われる
a20_kbc_wait:
	xor	cx, cx
a20_kbc_wait_loop:
	call	a20_test
	jnz	a20_done
	loop	a20_kbc_wait_loop

; 最後の試み: "制御ポート A"
a20_fast:
	in	al, 0x92		    ; 制御ポートA
	or	al, 0x02		    ; "fast A20" バージョンは
	and	al, 0xFE		    ; 急にRESETされない
	out	0x92, al

; 制御ポートAに効果が出るのを待つ
a20_fast_wait:
	xor	cx, cx
a20_fast_wait_loop:
	call	a20_test
	jnz	a20_done
	loop	a20_fast_wait_loop

; A20がまだ反応を見せない。繰り返してみる
; 
	dec	byte [a20_tries]
	jnz	a20_try_loop

a20_die:
	lea	esi, [msg003]    ; 文字列があるところのアドレスをesiに代入する。
	mov	ax, 0xB800
	mov	es, ax		    ; esに0xB800を入れる。
	mov	edi, 80*2	    ; 画面の二行目から始める。
	call	printf
	jmp	halthalt

a20_tries:
	db	A20_ENABLE_LOOPS

; ここまで来ると、A20がONになったとしていい。
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

	in	al, 0x64 		    ; 8042状態ポート 
	test	al, 1		; 出力バッファをテスト
	jz	no_output

	call	delay
	in	al, 0x60			    ; 読む
	jmp	empty_8042_loop

no_output:
	test	al, 2			    ; 入力バッファがいっぱいになったか 
	jnz	empty_8042_loop		    ; yes ? loopを回る

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

	lea	esi, [msg004]    ; 文字列があるところのアドレスをesiに代入する。
	mov	ax, 0xB800
	mov	es, ax		    ; esに0xB800を入れる。
	mov	edi, 80*2*2*2	    ; 画面の二行目から始める。
	call	printf
	jmp	halthalt
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
	mov	esi,bootpack	;転送元
	mov	edi,BOTPAK	;転送先
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
	jnz	memcpy			; 引き算した結果が0でなければmemcpyへ
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
	lea	esi, [msg006]    ; 文字列があるところのアドレスをesiに代入する。
	mov	ax, 0xB800
	mov	es, ax		    ; esに0xB800を入れる。
	mov	edi, 80*2*2*2*2	    ; 画面の二行目から始める。
	call	printf
thend2:
	hlt
	jmp	thend2	

;データ

msg001:	db	"Welcome to chnos project .",0
msg002:	db	"A20GATE on .",0
msg003:	db	"A20GATE filed .",0
msg004:	db	"Video mode is not supported or invalid.",0
msg005:	db	"Press the Enter key to shut down ...",0
msg006:	db	"Sorry . Shutdown filed . Press power button ."
backcc:	db	".",0

		alignb	16
GDT0:
		resb	8				; ヌルセレクタ
		dw		0xffff,0x0000,0x9200,0x00cf	; 読み書き可能セグメント32bit
		dw		0xffff,0x0000,0x9a28,0x0047	; 実行可能セグメント32bit（bootpack用）

		dw		0
GDTR0:
		dw		8*3-1
		dd		GDT0

		alignb	16
bootpack:



