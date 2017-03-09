obj-m += testmod.o
testmod-objs := test_mod.o test_rdpmc.o test_pt.o test_vmx.o test_mpx.o test_xsave.o
all:
	make -C /home/linux M=$(PWD) modules	 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
