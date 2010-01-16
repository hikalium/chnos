TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/CHNOSProject/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)chnos/chnos.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
MAKEFONT = $(TOOLPATH)makefont.exe
COPY     = copy
DEL      = del

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

naskfunc.obj : naskfunc.nas Makefile
	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst


chnos.sys : asmhead.bin Makefile
	copy /B asmhead.bin chnos.sys

chnos.img : chnipl.bin chnos.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:chnipl.bin len:512 from:0 to:0 \
		copy from:chnos.sys to:@: \
		imgout:chnos.img

# コマンド

img :
	$(MAKE) chnos.img

run :
	$(MAKE) img
	$(COPY) chnos.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: chnos.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) chnos.sys

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

iso :
	$(MAKE)
	..\z_tools\mkisofs.exe -v -iso-level 1 -b chnos.img -o ..\z_tools\qemu_iso\chnos.iso .

