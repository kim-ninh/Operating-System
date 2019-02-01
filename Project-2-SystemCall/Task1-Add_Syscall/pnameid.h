asmlinkage int sys_pnametoid(char* name);
asmlinkage long sys_pidtoname(int pid, char* buf, int len);