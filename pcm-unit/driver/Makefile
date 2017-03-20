obj-m += pcm_test.o
pcm_test-objs := pcm_test_mod.o tsc.o msr.o setjmp.o setjmp64.o kvmclock_test.o #setjmp32.o
all:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	make -C /root/cicada/linux-cicada M=$(PWD) modules

clean:
	make -C /root/cicada/linux-cicada M=$(PWD) clean
