cmd_/home/cpu_cap/test_module/testmod.o := ld -m elf_x86_64   -r -o /home/cpu_cap/test_module/testmod.o /home/cpu_cap/test_module/test_mod.o /home/cpu_cap/test_module/test_rdpmc.o /home/cpu_cap/test_module/test_pt.o /home/cpu_cap/test_module/test_vmx.o /home/cpu_cap/test_module/test_mpx.o /home/cpu_cap/test_module/test_xsave.o ; scripts/mod/modpost /home/cpu_cap/test_module/testmod.o
