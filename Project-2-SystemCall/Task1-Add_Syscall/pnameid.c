#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/string.h>
#include "pnameid.h"
asmlinkage int sys_pnametoid(char* name){
	char buf[32];
	strncpy_from_user(buf, name, sizeof(buf));
	
    struct task_struct *task;
   
	int pid = -1;
	
    for_each_process(task){
        if(strcmp(task->comm,buf) == 0){
			pid = task->pid;
        }
    }
    return pid;
}

asmlinkage long sys_pidtoname(int pid, char* buf, int len){
    struct task_struct *task;
	int n = -1;
	
    for_each_process(task){
        if(task->pid == pid){
			copy_to_user(buf, task->comm, len);
            n = strlen(task->comm);
        }
    }
	
	if (n == -1)
		return -1;
	
	if (len >= n)
		return 0;
	
	if (len < n)
		return n;
}