
TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/CHNOSProject/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)CHNOSProject/CHNOSP.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
MAKEFONT = $(TOOLPATH)makefont.exe
COPY     = copy
DEL      = del
SJISCONV = $(TOOLPATH)sjisconv.exe -s

default :
	$(MAKE) img

# ファイル生成規則

chnos.img : files/chnipl.bin files/chnos.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:files/chnipl.bin len:512 from:0 to:0 \
		copy from:files/chnos.sys to:@: \
		imgout:files/chnos.img

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
	$(COPY) chnos.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

run_cd :
	$(MAKE) iso
	$(MAKE) -C ../z_tools/qemu_iso

install :
	$(MAKE) img
	$(IMGTOL) w a: chnos.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) chnos.sys

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

iso :
	$(MAKE)
	..\z_tools\mkisofs.exe -v -iso-level 1 -b chnos.img -o ..\z_tools\qemu_iso\chnos.iso .

