
#OBJS_BOOTPACK = naskfunc.obj hankaku.obj graphic.obj grap_08.obj grap_16.obj grap_32.obj gdtidt.obj int.obj io.obj fifo.obj keyboard.obj timer.obj mouse.obj memory.obj sheet.obj window.obj system.obj bootpack.obj
OBJS_BOOTPACK = naskfunc.obj hankaku.obj grap_08.obj grap_16.obj grap_32.obj graphic.obj memory.obj sheet.obj fifo.obj init.obj gdtidt.obj int.obj mouse.obj keyboard.obj timer.obj window.obj io.obj mtask.obj console.obj file.obj bootpack.obj
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

hello.hrb : hello.nas Makefile
	$(NASK) hello.nas hello.hrb hello.lst

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

chnos.sys : asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb chnos.sys

chnos.img : chnipl.bin chnos.sys hello.hrb Makefile
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
		copy from:hello.hrb to:@: \
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

