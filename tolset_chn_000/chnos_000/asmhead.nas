

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


	call	halthalt



;	サブルーチン

halthalt:
	hlt
	jmp	halthalt

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
	out    0x60, al
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
	xor    cx, cx
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
	db   A20_ENABLE_LOOPS

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

	in     al, 0x64 		    ; 8042状態ポート 
	test   al, 1			    ; 出力バッファをテスト
	jz     no_output

	call   delay
	in    al, 0x60			    ; 読む
	jmp    empty_8042_loop

no_output:
	test   al, 2			    ; 入力バッファがいっぱいになったか 
	jnz    empty_8042_loop		    ; yes ? loopを回る

empty_8042_end_loop:
	pop    ecx
	ret

delay:
	out    0x80, al
	ret


;データ

msg001:	db	"Welcome to chnos project .",0
msg002:	db	"A20GATE on .",0
msg003:	db	"A20GATE filed .",0
backcc:	db	".",0



