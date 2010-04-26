

[INSTRSET "i486p"]
VBEMODE	equ		0x0115	;標準は0x0115
; （画面モード一覧）
;0x0100  640x400  256  
;0x0101  640x480  256  
;0x0102  800x600  16  *
;0x0103  800x600  256  
;0x0104  1024x768  16  *
;0x0105  1024x768  256  
;0x0106  1280x1024  16  *
;0x0107  1280x1024  256  *
;0x010D  320x200  約32000色(1:5:5:5)  *
;0x010E  320x200  約65000色(5:6:5)  *
;0x010F  320x200  約1678万色(8:8:8)  *
;0x0110  640x480  約32000色(1:5:5:5)  ?
;0x0111  640x480  約65000色(5:6:5)  
;0x0112  640x480  約1678万色(8:8:8)  
;0x0113  800x600  約32000色(1:5:5:5) ? 
;0x0114  800x600  約65000色(5:6:5)  
;0x0115  800x600  約1678万色(8:8:8)  
;0x0116  1024x768  約32000色(1:5:5:5)?  
;0x0117  1024x768  約65000色(5:6:5)  
;0x0118  1024x768  約1678万色(8:8:8)  
;0x0119  1280x1024  約32000色(1:5:5:5)* 
;0x011A  1280x1024  約65000色(5:6:5) * 
;0x011B  1280x1024  約1678万色(8:8:8)  *


BOTPAK	equ		0x00280000		;bootpackのロード先
DSKCAC	equ		0x00100000		;ディスクキャッシュの場所
DSKCAC0	equ		0x00008000		;ディスクキャッシュの場所（リアルモード）

; BOOT_INFO関係
CYLS	equ		0x0ff0			;ブートセクタが設定する
LEDS	equ		0x0ff1			;キーボードの状態を記録
VMODE	equ		0x0ff2			;色数に関する情報。何ビットカラーか？（VESAINFOを使用することを推奨）
SCRNX	equ		0x0ff4			;解像度のX（VESAINFOを使用することを推奨）
SCRNY	equ		0x0ff6			;解像度のY（VESAINFOを使用することを推奨）
VRAM	equ		0x0ff8			;グラフィックバッファの開始番地（VESAINFOを使用することを推奨）


[BITS 16]

	org	0xc200				;このプログラムは、0xc200番地から始まる
asmhead:					
	mov	ax, 0				;レジスタ初期化。axを0に。
	mov	ss, ax				;レジスタ初期化。ssをaxに。つまり、ssを0に。
	mov	sp, 0xc200			;スタックポインタを初期化。spを0xc200に。
	mov	ds, ax				;レジスタ初期化。dsをaxに。つまり、dsを0に。
	mov	es, ax				;レジスタ初期化。esをaxに。つまり、esを0に。

	call	backc				;テキストモード背景の初期化。
	lea	esi, [msg001]    		;msg001のアドレスをesiに代入する。
	mov	ax, 0xB800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es, ax		    		;esに0xB800を入れる。esに直接代入はできないので。
	mov	edi, 0			    	;文字の位置指定。ゼロ行目は0。一行目は80*2。
	call	printf				;printfを呼び出す。

	call	a20_try_loop			;A20GATEをonにする。失敗すると、戻ってこない。
	lea	esi, [msg002]			;msg002のアドレスをesiに代入する。
	mov	ax, 0xB800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es, ax				;esに0xB800を入れる。esに直接代入はできないので。
	mov	edi, 80*2			;画面の一行目から始める。
	call	printf				;printfを呼び出す。

	call	vbecheck			;VesaBiosExtentionで画面モード切替。失敗すると、戻ってこない。
	call	keyled				;BIOSを利用して、キーボードのLEDなどの情報を取得。
	call	pmode				;プロテクテッド・バーチャルアドレスモードに移行。戻ってこない。


;サブルーチン

halthalt:
	lea	esi, [msg005]			;msg003のアドレスをesiに代入する。
	mov	ax, 0xB800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es, ax		    		;esに0xB800を入れる。esに直接代入はできないので。
	mov	edi, 80*2*2*2*2			;画面の四行目から始める。
	call	printf				;printfを呼び出す。
	call	entkeywait			;エンターキーを押すまで戻ってこない。
	call	shutdown			;シャットダウンする。

printf:						;テキストモード文字表示ルーチン
	push	eax		 		;eaxをスタックに保存。
printf_loop:
	mov	al, byte [esi]			;esi番地の文字をalに代入。
	mov	byte [es:edi], al		;alを画面の指定した番地に代入。
	or	al, al				;alが0なのかを調べる。
	jz	printf_end			;0であれば、printf_endにジャンプする。
	inc	edi				;0でなければ、ediを1つ増やす
	mov	byte [es:edi], 0x06		;文字の色と背景色の値を入れる。
	inc	esi				;次の文字を取り出すためにesiを1つ増やす。
	inc	edi				;画面に次の文字を表示するためにediを増やす。
	jmp	printf_loop			;printf_loopに行く。これを文字数分繰り返す。
printf_end:
	pop	eax				;スタックに保存しておいたeaxをeaxに戻す。
	ret					;呼び出し元に戻る。

backc:						;テキストモード背景色描画ルーチン
	mov	ax,0xb800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es,ax				;esに0xB800を入れる。esに直接代入はできないので。
	mov	di,0				;画面の一番初めから始める。
	mov	ax,word[backcc]			;axに書く文字を代入。
	mov	cx,0x7ff			;0x7ff回繰り返す。
	
paint:						;テキストモード背景色描画ルーチンループ
	mov	word[es:di],ax			;画面の番地にaxを代入。
	add	di,2				;画面の番地を増やす。
	dec	cx				;cxを減らして、回数を数える。
	jnz	paint				;cxがゼロではなかったら、また繰り返す。
	ret					;cxがゼロだったら、呼び出し元に戻る。

A20_TEST_LOOPS		equ	32 	  	;A20GATEがONになったかテストするループの回数。
A20_ENABLE_LOOPS	equ	255 	 	;A20GATEがONになるまでがんばる全体のループの回数。 
A20_TEST_ADDR		equ	4*0x80		;A20GATEテスト対象のアドレス。


a20_try_loop:					;A20GATE有効化ルーチン
a20_none:
	call	a20_test			;A20GATEがすでにONになっていないか調べる。
	jnz	a20_done			;すでにONだったら必要ないので、終了。
a20_bios:					;BIOS (INT 0x15, AX=0x2401)を使用したA20GATE有効化を試す。
	mov	ax, 0x2401			;機能番号0x2401=A20をオンにする。
						;機能番号0x2400=A20をオフにする。
	pushfd					;FLAGSをスタックに保存。
	int	0x15				;BIOS INT 0x15
	popfd					;FLAGSをスタックから復元。
	call	a20_test			;ONになったか調べる。
	jnz	a20_done			;ONになっていたら終了。そうじゃなかったら続く。
a20_kbc:					;キーボードコントローラー（KBC）を使用したA20GATE有効化を試す。
	call	empty_8042			;KBCのステータスレジスタを読んで、データポートが空になるまで待つ。
	call	a20_test			;なんとなくもう一回、ONになったか調べる。
	jnz	a20_done			;ONになっていたら終了。そうじゃなかったら続く。
	mov	al, 0xD1			;alに0xd1を代入。
	out	0x64, al			;KBCのコマンド出力（ポート0x64）に0xd1を代入。直接代入できないので。
	call	empty_8042			;KBCのステータスレジスタを読んで、データポートが空になるまで待つ。
	mov	al, 0xDF			;alに0xdfを代入。
	out	0x60, al			;KBCのデータ出力（ポート0x60）に0x60(A20GATE)
	call	empty_8042			;KBCのステータスレジスタを読んで、データポートが空になるまで待つ。
a20_kbc_wait:					;とにかく待つ。
	xor	cx, cx				;loopの回数を指定。
a20_kbc_wait_loop:
	call	a20_test			;ONになったか調べる。
	jnz	a20_done			;ONになっていたら終了。
	loop	a20_kbc_wait_loop		;loop(dec cxをして、ゼロになるまで何度もループ)。
a20_fast:					;最終手段　system control port（ポート0x92）。
	in	al, 0x92			;system control portをalに代入。
	or	al, 0x02			;alに0x02をor計算する。つまり、bit1を1にしている。
						;bit1=1=A20GATEをONにする。bit1=0=A20GATEをOFFにする。
	and	al, 0xFE			;さらに、alに0xfeをand計算する。
	out	0x92, al			;system control portにalを戻す。
a20_fast_wait:					;とにかく待つ。
	xor	cx, cx				;loopの回数を指定。
a20_fast_wait_loop:
	call	a20_test			;ONになったか調べる。
	jnz	a20_done			;ONになっていたら終了。
	loop	a20_fast_wait_loop		;loop(dec cxをして、ゼロになるまで何度もループ)。
	dec	byte [a20_tries]		;全体の繰り返し回数を一回減らす。
	jnz	a20_try_loop			;ゼロになるまであきらめず、最初から。。。

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
;	in	al, 0x60			    ; 読む
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

getc:
	mov	ah,0x00
	int	0x16
	ret

vbecheck:
	lea	esi, [msg007]			;msg003のアドレスをesiに代入する。
	mov	ax, 0xB800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es, ax		    		;esに0xB800を入れる。esに直接代入はできないので。
	mov	edi, 80*2*3		;画面の四行目から始める。
	call	printf				;printfを呼び出す。

	lea	esi, [msg008]			;msg003のアドレスをesiに代入する。
	mov	ax, 0xB800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es, ax		    		;esに0xB800を入れる。esに直接代入はできないので。
	mov	edi, 80*2*4		;画面の四行目から始める。
	call	printf				;printfを呼び出す。

	lea	esi, [msg009]			;msg003のアドレスをesiに代入する。
	mov	ax, 0xB800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es, ax		    		;esに0xB800を入れる。esに直接代入はできないので。
	mov	edi, 80*2*5		;画面の四行目から始める。
	call	printf				;printfを呼び出す。

	lea	esi, [msg010]			;msg003のアドレスをesiに代入する。
	mov	ax, 0xB800			;アドレスの指定。テキストモードは、0xB800から始まる。
	mov	es, ax		    		;esに0xB800を入れる。esに直接代入はできないので。
	mov	edi, 80*2*6		;画面の四行目から始める。
	call	printf				;printfを呼び出す。

	call	getc

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

	lea	esi, [msg004]    ; 文字列があるところのアドレスをesiに代入する。
	mov	ax, 0xB800
	mov	es, ax		    ; esに0xB800を入れる。
	mov	edi, 80*2*2*2	    ; 画面の二行目から始める。
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
msg006:	db	"Sorry . Shutdown filed . Press power button .",0
msg007:	db	"Video Mode List. Please select the mode number.",0
msg008:	db	"32bit--1:320x200--2:640x480--3:800x600--4:1024x768--5:1280x1024--6:1600x1200",0
msg009:	db	"16bit--7:320x200--8:640x480--9:800x600--a:1024x768--b:1280x1024--c:1600x1200",0
msg010:	db	" 8bit--d:640x400--e:640x480--f:800x600--g:1024x768--h:1280x1024--i:1600x1200",0
backcc:	db	".",0

videomode:	dw 0

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



