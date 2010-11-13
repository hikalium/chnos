
OBJS_BOOTPACK = naskfunc.obj hankaku.obj grap_08.obj grap_16.obj grap_32.obj graphic.obj memory.obj sheet.obj fifo.obj init.obj gdtidt.obj int.obj mouse.obj keyboard.obj timer.obj window.obj io.obj mtask.obj console.obj file.obj api.obj textmode.obj bootpack.obj
TOOLPATH = ../../z_tools/
INCPATH  = ../../z_tools/CHNOSProject/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(INCPATH)CHNOSP.rul
EDIMG    = $(TOOLPATH)edimg.exe bias:9
IMGTOL   = $(TOOLPATH)imgtol.com
MAKEFONT = $(TOOLPATH)makefont.exe
COPY     = copy
DEL      = del
SJISCONV = $(TOOLPATH)sjisconv.exe -s

# デフォルト動作

default :
	$(MAKE) img

# ファイル生成規則

chnipl.bin : chnipl.nas Makefile
	$(NASK) chnipl.nas chnipl.bin chnipl.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

bootpack.gas : bootpack.ca Makefile
	$(CC1) -o bootpack.gas bootpack.ca

bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.nas

bootpack.obj : bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

naskfunc.obj : naskfunc.nas Makefile
	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst

#APPs

a.bim : a.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:a.bim map:a.map a.obj a_nask.obj

a.hrb : a.bim Makefile
	$(BIM2HRB) a.bim a.hrb 0

hello.hrb : hello.nas Makefile
	$(NASK) hello.nas hello.hrb hello.lst

hello2.hrb : hello2.nas Makefile
	$(NASK) hello2.nas hello2.hrb hello2.lst

hello3.bim : hello3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello3.bim map:hello3.map hello3.obj a_nask.obj

hello3.hrb : hello3.bim Makefile
	$(BIM2HRB) hello3.bim hello3.hrb 0

hello4.bim : hello4.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello4.bim stack:1k map:hello4.map hello4.obj a_nask.obj

hello4.hrb : hello4.bim Makefile
	$(BIM2HRB) hello4.bim hello4.hrb 0

hello5.obj : hello5.nas Makefile
	$(NASK) hello5.nas hello5.obj hello5.lst

hello5.bim : hello5.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello5.bim map:hello5.map hello5.obj a_nask.obj

hello5.hrb : hello5.bim Makefile
	$(BIM2HRB) hello5.bim hello5.hrb 0

uuid.bim : uuid.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:uuid.bim map:uuid.map uuid.obj a_nask.obj

uuid.hrb : uuid.bim Makefile
	$(BIM2HRB) uuid.bim uuid.hrb 0

star1.bim : star1.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:star1.bim map:star1.map star1.obj a_nask.obj

star1.hrb : star1.bim Makefile
	$(BIM2HRB) star1.bim star1.hrb 0

stars.bim : stars.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars.bim map:stars.map stars.obj a_nask.obj

stars.hrb : stars.bim Makefile
	$(BIM2HRB) stars.bim stars.hrb 0

lines.bim : lines.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:lines.bim map:lines.map lines.obj a_nask.obj

lines.hrb : lines.bim Makefile
	$(BIM2HRB) lines.bim lines.hrb 0

winhelo.bim : winhelo.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo.bim stack:1k map:winhelo.map winhelo.obj a_nask.obj

winhelo.hrb : winhelo.bim Makefile
	$(BIM2HRB) winhelo.bim winhelo.hrb 0

winhelo2.bim : winhelo2.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo2.bim stack:1k map:winhelo2.map winhelo2.obj a_nask.obj

winhelo2.hrb : winhelo2.bim Makefile
	$(BIM2HRB) winhelo2.bim winhelo2.hrb 0

winhelo3.bim : winhelo3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo3.bim stack:64k map:winhelo3.map winhelo3.obj a_nask.obj

winhelo3.hrb : winhelo3.bim Makefile
	$(BIM2HRB) winhelo3.bim winhelo3.hrb 0

crack1.bim : crack1.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:crack1.bim map:crack1.map crack1.obj a_nask.obj

crack1.hrb : crack1.bim Makefile
	$(BIM2HRB) crack1.bim crack1.hrb 0

crack2.hrb : crack2.nas Makefile
	$(NASK) crack2.nas crack2.hrb crack2.lst

bug1.bim : bug1.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug1.bim map:bug1.map bug1.obj a_nask.obj

bug1.hrb : bug1.bim Makefile
	$(BIM2HRB) bug1.bim bug1.hrb 0

bug2.bim : bug2.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug2.bim map:bug2.map bug2.obj a_nask.obj

bug2.hrb : bug2.bim Makefile
	$(BIM2HRB) bug2.bim bug2.hrb 0

bug3.bim : bug3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug3.bim map:bug3.map bug3.obj a_nask.obj

bug3.hrb : bug3.bim Makefile
	$(BIM2HRB) bug3.bim bug3.hrb 0

#End APPs

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

chnos.sys : asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb chnos.sys

chnos.img : chnipl.bin chnos.sys a.hrb hello.hrb hello2.hrb hello3.hrb hello4.hrb hello5.hrb uuid.hrb star1.hrb lines.hrb stars.hrb winhelo.hrb winhelo2.hrb winhelo3.hrb crack1.hrb crack2.hrb bug1.hrb bug2.hrb bug3.hrb Makefile
	$(EDIMG)   imgin:../../z_tools/fdimg0at.tek \
		wbinimg src:chnipl.bin len:512 from:0 to:0 \
		copy from:chnos.sys to:@: \
		copy from:chnipl.nas to:@: \
		copy from:make.bat to:@: \
		copy from:bootpack.c to:@: \
		copy from:core.h to:@: \
		copy from:console.c to:@: \
		copy from:fifo.c to:@: \
		copy from:gdtidt.c to:@: \
		copy from:grap_08.c to:@: \
		copy from:grap_16.c to:@: \
		copy from:grap_32.c to:@: \
		copy from:graphic.c to:@: \
		copy from:init.c to:@: \
		copy from:int.c to:@: \
		copy from:io.c to:@: \
		copy from:keyboard.c to:@: \
		copy from:memory.c to:@: \
		copy from:mouse.c to:@: \
		copy from:mtask.c to:@: \
		copy from:sheet.c to:@: \
		copy from:timer.c to:@: \
		copy from:window.c to:@: \
		copy from:file.c to:@: \
		copy from:a.hrb to:@: \
		copy from:hello.hrb to:@: \
		copy from:hello2.hrb to:@: \
		copy from:hello3.hrb to:@: \
		copy from:hello4.hrb to:@: \
		copy from:hello5.hrb to:@: \
		copy from:winhelo.hrb to:@: \
		copy from:winhelo2.hrb to:@: \
		copy from:winhelo3.hrb to:@: \
		copy from:crack1.hrb to:@: \
		copy from:crack2.hrb to:@: \
		copy from:bug1.hrb to:@: \
		copy from:bug2.hrb to:@: \
		copy from:bug3.hrb to:@: \
		copy from:uuid.hrb to:@: \
		copy from:star1.hrb to:@: \
		copy from:stars.hrb to:@: \
		copy from:lines.hrb to:@: \
		imgout:chnos.img 

#一般規則

%.ca : %.c Makefile
	$(SJISCONV) $*.c $*.ca

%.gas : %.ca core.h Makefile
	$(CC1) -o $*.gas $*.ca

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst



# コマンド

img :
	$(MAKE) chnos.img

run :
	$(MAKE) img
	$(COPY) chnos.img ..\..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../../z_tools/qemu
run_b :
	$(MAKE) img
	$(COPY) chnos.img ..\..\z_tools\bochs\fdimage0.bin
	$(MAKE) -C ../../z_tools/bochs


run_cd :
	$(MAKE) iso
	$(MAKE) -C ../../z_tools/qemu_iso

install :
	$(MAKE) img
	$(IMGTOL) w a: chnos.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) *.ca
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) chnos.sys

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

iso :
	..\..\z_tools\mkisofs.exe -v -iso-level 1 -b chnos.img -o ..\..\z_tools\qemu_iso\chnos.iso .

