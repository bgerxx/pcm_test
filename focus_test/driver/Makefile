obj-m += pcm_test.o
pcm_test-objs := pcm_test_mod.o cpuid.o invd.o rdseed.o rdrand.o rdpmc.o rdtsc.o rdtscp.o
all:
	make -C /home/linux-cicada M=$(PWD) modules
clean:
	make -C /home/linux-cicada M=$(PWD) clean
