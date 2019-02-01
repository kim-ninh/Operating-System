INSTRUCTION

// Create Makefile in pnameid/
// obj-y := pnameid


// Add this in the kernel Makefile
// core-y += kernel/ mm/ fs/ ipc/ security/ crypto/ block/ pnameid/


// Add this two line in the bottom: arch/x86/entry/syscalls/syscall_64.tbl
//333	64	pnametoid	sys_pnametoid
//334	64	pidtoname	sys_pidtoname


// Add this two line in the bottom: include/linux/syscalls.h
//asmlinkage int sys_pnametoid(char* name);
//asmlinkage long sys_pidtoname(int pid, char* buf, int len);


//Package required:
//libncurses5-dev bison flex libssl-dev libelf-dev


//Complile
//make menuconfig
//make -j2
//make modules_install install