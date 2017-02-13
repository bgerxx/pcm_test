obj-m += pcm_test.o
pcm_test-objs := pcm_test_mod.o tsc.o
all:
	make -C /home/linux-cicada M=$(PWD) modules
clean:
	make -C /home/linux-cicada M=$(PWD) clean
