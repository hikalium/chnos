
#アプリ追加時に書き足すべき箇所	
# ファイル生成規則 2箇所
# コマンド
#  full, clan_full, src_only_full

TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/CHNOSProject/

MAKE     = $(TOOLPATH)make.exe -r
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)RWFD.EXE
COPY     = copy
DEL      = del

# デフォルト動作

default :
	$(MAKE) chnos.img

# ファイル生成規則

chnos.img : chnos/chnipl.bin chnos/chnos.sys chnos/chnipl.nas \
		asmtest/asmtest.hrb \
		crack1/crack1.chn \
		ctest/ctest.chn
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:chnos/chnipl.bin len:512 from:0 to:0 \
		copy from:chnos/chnos.sys to:@: \
		copy from:chnos/chnipl.nas to:@: \
		copy from:asmtest/asmtest.hrb to:@: \
		copy from:crack1/crack1.chn to:@: \
		copy from:ctest/ctest.chn to:@: \
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
	$(IMGTOL) chnos.img a:

full :
	$(MAKE) -C chnos
	$(MAKE) -C asmtest
	$(MAKE) -C crack1
	$(MAKE) -C ctest
	$(MAKE) chnos.img

run_full :
	$(MAKE) full
	$(COPY) chnos.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install_full :
	$(MAKE) full
	$(IMGTOL) chnos.img a:

run_os :
	$(MAKE) -C chnos
	$(MAKE) run

clean :
# 何もしない

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

clean_full :
	$(MAKE) -C asmtest		clean
	$(MAKE) -C crack1		clean
	$(MAKE) -C ctest		clean
	$(MAKE) -C chnos		clean

src_only_full :
	$(MAKE) -C asmtest		src_only
	$(MAKE) -C crack1		src_only
	$(MAKE) -C ctest		src_only
	$(MAKE) -C chnos		src_only
	-$(DEL) chnos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) chnos.img
