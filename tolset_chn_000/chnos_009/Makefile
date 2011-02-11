TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/CHNOSProject/

MAKE     = $(TOOLPATH)make.exe -r
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# デフォルト動作

default :
	$(MAKE) chnos.img

# ファイル生成規則

chnos.img : chnos/chnipl.bin chnos/chnos.sys chnos/chnipl.nas 
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:chnos/chnipl.bin len:512 from:0 to:0 \
		copy from:chnos/chnos.sys to:@: \
		copy from:chnos/chnipl.nas to:@: \
		imgout:chnos.img

# コマンド

run :
	$(MAKE) chnos.img
	$(COPY) chnos.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

run_b :
	$(MAKE) chnos.img
	$(COPY) chnos.img ..\z_tools\bochs\fdimage0.bin
	$(MAKE) -C ../z_tools/bochs

iso :
	$(MAKE) chnos.img
	..\z_tools\mkisofs.exe -v -iso-level 1 -b chnos.img -o ..\z_tools\qemu_iso\chnos.iso .

run_cd :
	$(MAKE) iso
	$(MAKE) -C ../z_tools/qemu_iso

install :
	$(MAKE) chnos.img
	$(IMGTOL) w a: chnos.img

full :
	$(MAKE) -C chnos
	$(MAKE) chnos.img

run_full :
	$(MAKE) full
	$(COPY) chnos.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install_full :
	$(MAKE) full
	$(IMGTOL) w a: chnos.img

run_os :
	$(MAKE) -C chnos
	$(MAKE) run

clean :
# 何もしない

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

clean_full :
	$(MAKE) -C chnos		clean

src_only_full :
	$(MAKE) -C chnos		src_only
	-$(DEL) chnos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) chnos.img
