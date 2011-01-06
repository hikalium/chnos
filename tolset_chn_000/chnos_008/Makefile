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

chnos.img : chnos/chnipl.bin chnos/chnos.sys chnos/chnipl.nas \
		a/a.hrb \
		bug1/bug1.hrb \
		bug2/bug2.hrb \
		bug3/bug3.hrb \
		crack1/crack1.hrb \
		crack2/crack2.hrb \
		crack3/crack3.hrb \
		crack4/crack4.hrb \
		hello/hello.hrb \
		winhelo/winhelo.hrb \
		winhelo2/winhelo2.hrb \
		winhelo3/winhelo3.hrb 
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:chnos/chnipl.bin len:512 from:0 to:0 \
		copy from:chnos/chnos.sys to:@: \
		copy from:chnos/chnipl.nas to:@: \
		copy from:a/a.hrb to:@: \
		copy from:bug1/bug1.hrb to:@: \
		copy from:bug2/bug2.hrb to:@: \
		copy from:bug3/bug3.hrb to:@: \
		copy from:crack1/crack1.hrb to:@: \
		copy from:crack2/crack2.hrb to:@: \
		copy from:crack3/crack3.hrb to:@: \
		copy from:crack4/crack4.hrb to:@: \
		copy from:hello/hello.hrb to:@: \
		copy from:winhelo/winhelo.hrb to:@: \
		copy from:winhelo2/winhelo2.hrb to:@: \
		copy from:winhelo3/winhelo3.hrb to:@: \
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
	$(MAKE) -C a
	$(MAKE) -C bug1
	$(MAKE) -C bug2
	$(MAKE) -C bug3
	$(MAKE) -C crack1
	$(MAKE) -C crack2
	$(MAKE) -C crack3
	$(MAKE) -C crack4
	$(MAKE) -C hello
	$(MAKE) -C winhelo
	$(MAKE) -C winhelo2
	$(MAKE) -C winhelo3
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
	$(MAKE) -C bug1			clean
	$(MAKE) -C bug2			clean
	$(MAKE) -C bug3			clean
	$(MAKE) -C crack1		clean
	$(MAKE) -C crack2		clean
	$(MAKE) -C crack3		clean
	$(MAKE) -C crack4		clean
	$(MAKE) -C hello		clean
	$(MAKE) -C winhelo		clean
	$(MAKE) -C winhelo2		clean
	$(MAKE) -C winhelo3		clean

src_only_full :
	$(MAKE) -C chnos		src_only
	$(MAKE) -C bug1			src_only
	$(MAKE) -C bug2			src_only
	$(MAKE) -C bug3			src_only
	$(MAKE) -C crack1		src_only
	$(MAKE) -C crack2		src_only
	$(MAKE) -C crack3		src_only
	$(MAKE) -C crack4		src_only
	$(MAKE) -C hello		src_only
	$(MAKE) -C winhelo		src_only
	$(MAKE) -C winhelo2		src_only
	$(MAKE) -C winhelo3		src_only
	-$(DEL) chnos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) chnos.img
