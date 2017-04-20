obj-m += pcm_test.o
pcm_test-objs := pcm_test_mod.o cpuid.o invd.o rdseed.o rdrand.o rdpmc.o rdtsc.o rdtscp.o pio.o mmio.o icmp.o
all:
	make -C /root/rpmbuild/BUILD/kernel-3.10.0-514.el7/linux-3.10.0-514.el7.x86_64 M=$(PWD) modules
clean:
	make -C /root/rpmbuild/BUILD/kernel-3.10.0-514.el7/linux-3.10.0-514.el7.x86_64 M=$(PWD) clean
