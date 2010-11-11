
[INSTRSET "i486p"]
VBEMODE equ             0x0115  ;0x0115
;Graphic Modes
;0x0100		640x400  	256
;0x0101		640x480  	256
;0x0102		800x600  	16
;0x0103		800x600  	256
;0x0104		1024x768  	16
;0x0105		1024x768  	256
;0x0106		1280x1024  	16
;0x0107		1280x1024  	256
;0x010D		320x200		32768		(1:5:5:5)
;0x010E		320x200		65536		(0:5:6:5)
;0x010F		320x200		16777216	(8:8:8:8)
;0x0110		640x480  	32768		(1:5:5:5)
;0x0111		640x480  	65536		(0:5:6:5)
;0x0112		640x480  	16777216	(8:8:8:8)
;0x0113		800x600  	32768		(1:5:5:5)
;0x0114		800x600  	65536		(0:5:6:5)
;0x0115		800x600  	16777216	(8:8:8:8)
;0x0116		1024x768  	32768		(1:5:5:5)
;0x0117		1024x768  	65536		(0:5:6:5)
;0x0118		1024x768  	16777216	(8:8:8:8)
;0x0119		1280x1024  	32768		(1:5:5:5)
;0x011A		1280x1024  	65536		(0:5:6:5) 
;0x011B		1280x1024  	16777216	(8:8:8:8) 


BOTPAK  equ             0x00280000
DSKCAC  equ             0x00100000
DSKCAC0 equ             0x00008000

; BOOT_INFO
CYLS    equ             0x0ff0
LEDS    equ             0x0ff1
VMODE   equ             0x0ff2
SCRNX   equ             0x0ff4
SCRNY   equ             0x0ff6
VRAM    equ             0x0ff8

[BITS 16]

        org     0xc200
asmhead:
        mov     ax, 0
        mov     ss, ax
        mov     sp, 0xc200
        mov     ds, ax
        mov     es, ax

        call    backc

        lea     esi, [msg001]
        mov     edi, 0
        call    printf

        call    a20_try_loop

        lea     esi, [msg002]
        mov     edi, 80*2
        call    printf

        call    vbecheck
        call    keyled
        call    pmode


;サブルーチン

halthalt:
        lea     esi, [msg005]
        mov     ax, 0xB800
        mov     es, ax
        mov     edi, 80*2*2*2*2
        call    printf
        call    entkeywait
        call    shutdown

printf:
        push    eax
        mov     ax, 0xB800
        mov     es, ax
printf_loop:
        mov     al, byte [esi]
        mov     byte [es:edi], al
        or      al, al
        jz      printf_end
        inc     edi
        mov     byte [es:edi], 0x03
        inc     esi
        inc     edi
        jmp     printf_loop
printf_end:
        pop     eax
        ret

backc:
        mov     ax,0xb800
        mov     es,ax
        mov     di,0
        mov     ax,word[backcc]
        mov     cx,0x7ff

paint:
        mov     word[es:di],ax
        add     di,2
        dec     cx
        jnz     paint
        ret

A20_TEST_LOOPS          equ     32
A20_ENABLE_LOOPS        equ     255
A20_TEST_ADDR           equ     4*0x80


a20_try_loop:
a20_none:
        call    a20_test
        jnz     a20_done
a20_bios:
        mov     ax, 0x2401

        pushfd
        int     0x15
        popfd
        call    a20_test
        jnz     a20_done
a20_kbc:
        call    empty_8042
        call    a20_test
        jnz     a20_done
        mov     al, 0xD1
        out     0x64, al
        call    empty_8042
        mov     al, 0xDF
        out     0x60, al
        call    empty_8042
a20_kbc_wait:
        xor     cx, cx
a20_kbc_wait_loop:
        call    a20_test
        jnz     a20_done
        loop    a20_kbc_wait_loop
a20_fast:
        in      al, 0x92
        or      al, 0x02

        and     al, 0xFE
        out     0x92, al
a20_fast_wait:
        xor     cx, cx
a20_fast_wait_loop:
        call    a20_test
        jnz     a20_done
        loop    a20_fast_wait_loop
        dec     byte [a20_tries]
        jnz     a20_try_loop

a20_die:
        lea     esi, [msg003]    
        mov     edi, 80*2          
        call    printf
        jmp     halthalt

a20_tries:
        db      A20_ENABLE_LOOPS

a20_done:
        ret

a20_test:
        push    cx
        push    ax
        xor     cx, cx
        mov     fs, cx                      
        dec     cx
        mov     gs, cx                      
        mov     cx, A20_TEST_LOOPS
        mov     ax, word [fs:A20_TEST_ADDR]
        push    ax
a20_test_wait:
        inc     ax
        mov     word [fs:A20_TEST_ADDR], ax
        call    delay
        cmp     ax, word [gs:A20_TEST_ADDR+0x10]
        loop    a20_test_wait

        pop     word [fs:A20_TEST_ADDR]
        pop     ax
        pop     cx
        ret

empty_8042:
        push    ecx
        mov     ecx, 100000

empty_8042_loop:
        dec     ecx
        jz      empty_8042_end_loop

        call    delay

        in      al, 0x64
        test    al, 1           
        jz      no_output

        call    delay
;       in      al, 0x60
        jmp     empty_8042_loop

no_output:
        test    al, 2
        jnz     empty_8042_loop

empty_8042_end_loop:
        pop     ecx
        ret

delay:
        out     0x80, al
        ret

getc:
        mov     ah,0x00
        int     0x16
        ret

vbecheck:
        lea     esi, [msg007]
        mov     edi, 80*2*3             
        call    printf

        lea     esi, [msg008]
        mov     edi, 80*2*4
        call    printf

        lea     esi, [msg009]
        mov     edi, 80*2*5
        call    printf

        lea     esi, [msg010]
        mov     edi, 80*2*6
        call    printf

        lea     esi, [msg011]
        mov     edi, 80*2*7
        call    printf

        lea     esi, [msg012]
        mov     edi, 80*2*8
        call    printf

        call    getc

        cmp     ah,0x01
        je      vbe00

        cmp     ah,0x02
        je      vbe01
        cmp     ah,0x03
        je      vbe02
        cmp     ah,0x04
        je      vbe03
        cmp     ah,0x05
        je      vbe04
        cmp     ah,0x06
        je      vbe05
        cmp     ah,0x07
        je      vbe06
        cmp     ah,0x08
        je      vbe07
        cmp     ah,0x09
        je      vbe08
        cmp     ah,0x0a
        je      vbe09

        cmp     ah,0x1e
        je      vbe0a
        cmp     ah,0x30
        je      vbe0b
        cmp     ah,0x2e
        je      vbe0c
        cmp     ah,0x20
        je      vbe0d
        cmp     ah,0x12
        je      vbe0e
        cmp     ah,0x21
        je      vbe0f
        cmp     ah,0x22
        je      vbe0g
        cmp     ah,0x23
        je      vbe0h
        cmp     ah,0x17
        je      vbe0i

        cmp     ah,0x2d
        je      vbetext

        jmp     vbecheck

vbe00:
        mov     al,0x13
        mov     ah,0x00
        int     0x10
        mov     ax,0xe0
        mov     es,ax
        mov     di,0
        mov     word[es:di+0x12],320
        mov     word[es:di+0x14],200
        mov     byte[es:di+0x19],8
        mov     dword[es:di+0x28],0x000a0000
        mov     byte[VMODE],8
        mov     word[SCRNX],320
        mov     word[SCRNY],200
        mov     dword[VRAM],0x000a0000
        ret

vbe01:
        mov     word[videomode],0x010f
        jmp     vbesub
vbe02:
        mov     word[videomode],0x0112
        jmp     vbesub
vbe03:
        mov     word[videomode],0x0115
        jmp     vbesub
vbe04:
        mov     word[videomode],0x0118
        jmp     vbesub
vbe05:
        mov     word[videomode],0x011b
        jmp     vbesub
vbe06:
        mov     word[videomode],0x011f
        jmp     vbesub
vbe07:
        mov     word[videomode],0x010e
        jmp     vbesub
vbe08:
        mov     word[videomode],0x0111
        jmp     vbesub
vbe09:
        mov     word[videomode],0x0114
        jmp     vbesub
vbe0a:
        mov     word[videomode],0x0117
        jmp     vbesub
vbe0b:
        mov     word[videomode],0x011a
        jmp     vbesub
vbe0c:
        mov     word[videomode],0x011e
        jmp     vbesub
vbe0d:
        mov     word[videomode],0x0100
        jmp     vbesub
vbe0e:
        mov     word[videomode],0x0101
        jmp     vbesub
vbe0f:
        mov     word[videomode],0x0103
        jmp     vbesub
vbe0g:
        mov     word[videomode],0x0105
        jmp     vbesub
vbe0h:
        mov     word[videomode],0x0107
        jmp     vbesub
vbe0i:
        mov     word[videomode],0x011c
        jmp     vbesub

vbesub:
        mov     ax,0xe0
        mov     es,ax
        mov     di,0
        mov     cx,[videomode]
        mov     ax,0x4f01
        int     0x10

        mov     ax,0x9000
        mov     es,ax
        mov     di,0
        mov     ax,0x4f00
        int     0x10
        cmp     ax,0x004f
        jne     scrn320
        mov     ax,[es:di+4]
        cmp     ax,0x0200
        jb      scrn320
        mov     cx,[videomode]
        mov     ax,0x4f01
        int     0x10
        cmp     ax,0x004f
        jne     scrn320

        mov     bx,[videomode]
        add     bx,0x4000
        mov     ax,0x4f02
        int     0x10
        mov     byte[VMODE],16
        mov     ax,[es:di+0x12]
        mov     [SCRNX],ax
        mov     ax,[es:di+0x14]
        mov     [SCRNY],ax
        mov     eax,[es:di+0x28]
        mov     [VRAM],eax
        ret

vbetext:
        mov     byte[VMODE],0
        mov     word[SCRNX],80
        mov     word[SCRNY],25
        mov     dword[VRAM],0xb8000
        ret

scrn320:

        lea     esi, [msg004]            
        mov     edi, 80*2*2*2       
        call    printf
	call	entkeywait
        jmp     vbecheck

keyled:
        mov     ah,0x02
        int     0x16
        mov     [LEDS],al
        ret

pmode:
        mov     al,0xff
        out     0x21,al
        nop
        out     0xa1,al
        cli

        lgdt    [GDTR0]
        mov     eax,cr0
        and     eax,0x7fffffff
        or      eax,0x00000001
        mov     cr0,eax
        jmp     pipelineflush
pipelineflush:
        mov     ax,1*8
        mov     ds,ax
        mov     es,ax
        mov     fs,ax
        mov     gs,ax
        mov     ss,ax
        mov     esi,bootpack    
        mov     edi,BOTPAK      
        mov     ecx,512*1024/4
        call    memcpy
        mov     esi,0x7c00
        mov     edi,DSKCAC
        mov     ecx,512/4

        call    memcpy
        mov     esi,DSKCAC0+512
        mov     edi,DSKCAC+512
        mov     ecx,0
        mov     cl,byte[CYLS]
        imul    ecx,512*18*2/4
        sub     ecx,512/4
        call    memcpy

        mov     ebx,BOTPAK
        mov     ecx,[ebx+16]
        add     ecx,3
        shr     ecx,2
        jz      skip
        mov     esi,[ebx+20]
        add     esi,ebx
        mov     edi,[ebx+12]
        call    memcpy
skip:
        mov     esp,[ebx+12]
        jmp     dword 2*8:0x0000001b


memcpy:
        mov     eax,[esi]
        add     esi,4
        mov     [edi],eax
        add     edi,4
        sub     ecx,1
        jnz     memcpy
        ret

entkeywait:
        mov     ah,0
        int     0x16
        cmp     ah,0x1c
        jne     entkeywait
        ret

shutdown:
        mov     ax,0x5300
        mov     bx,0
        int     0x15
        jc      thend
        mov     ax,0x5301
        mov     bx,0
        int     0x15
        mov     ax,0x530e
        mov     bx,0
        mov     cx,0x0101
        int     0x15
        jc      thend
        mov     ax,0x530f
        mov     bx,0x0001
        mov     cx,0x0001
        int     0x15
        jc      thend
        mov     ax,0x5308
        mov     bx,0x0001
        mov     cx,0x0001
        int     0x15
        jc      thend
        mov     ax,0x5307
        mov     bx,0x0001
        mov     cx,0x0003
        int     0x15
        hlt

thend:
        lea     esi, [msg006]
        mov     ax, 0xB800
        mov     es, ax
        mov     edi, 80*2*2*2*2
        call    printf
thend2:
        hlt
        jmp     thend2

;data

msg001: db      "Welcome to chnos project .",0
msg002: db      "A20GATE on .",0
msg003: db      "A20GATE filed .",0
msg004: db      "Video mode is not supported or invalid.",0
msg005: db      "Press the Enter key to shut down ...",0
msg006: db      "Sorry . Shutdown filed . Press power button .",0
msg007: db      "Video Mode List. Please select the mode number.",0
msg008: db      "32bit--1:320x200--2:640x480--3:800x600--4:1024x768--5:1280x1024--6:1600x1200",0
msg009: db      "16bit--7:320x200--8:640x480--9:800x600--a:1024x768--b:1280x1024--c:1600x1200",0
msg010: db      " 8bit--d:640x400--e:640x480--f:800x600--g:1024x768--h:1280x1024--i:1600x1200",0
msg011: db      "Press ESC to start in VGA mode.",0
msg012: db      "Press X to start in TEXT mode.",0
backcc: db      ".",0x03

videomode:      dw 0

                alignb  16
GDT0:
                resb    8                               ; ヌルセレクタ
                dw              0xffff,0x0000,0x9200,0x00cf     ; 読み書き可能セグメント32bit
                dw              0xffff,0x0000,0x9a28,0x0047     ; 実行可能セグメント32bit（bootpack用）

                dw              0
GDTR0:
                dw              8*3-1
                dd              GDT0

                alignb  16
bootpack:

















































































