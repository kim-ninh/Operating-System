#include <linux/module.h>  /* Needed by all kernel modules */
#include <linux/kernel.h>  /* Needed for loglevels (KERN_WARNING, KERN_EMERG, KERN_INFO, etc.) */
#include <linux/init.h>    /* Needed for __init and __exit macros. */
#include <linux/unistd.h>  /* sys_call_table __NR_* system call function indices */
#include <linux/fs.h>      /* filp_open */
#include <asm/paravirt.h> /* write_cr0 */
#include <linux/sched.h>	/* need for current_id  */
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/fdtable.h>	/*need for files_struct: current_files*/

//Get this address from bash: sudo /proc/kallsyms  | grep sys_call_table
//or: sudo cat /boot/System.map-4.14.81 | grep sys_call_table
unsigned long *syscall_table = (unsigned long *)0xffffffffb1a00180;	// <= Replace your correct address here

char *tmp = NULL;
char *pathname = NULL;

asmlinkage int (*original_write)(unsigned int, const char __user *, size_t);

asmlinkage int (*original_open)(const char __user *, int);

int getPathName(unsigned int fd,struct files_struct *files){
	
	struct file *file;
	struct path *path;

	spin_lock(&files->file_lock);
	file = fcheck_files(files, fd);
	if (!file) {
    	spin_unlock(&files->file_lock);
    	return -ENOENT;
	}

	path = &file->f_path;
	path_get(path);
	spin_unlock(&files->file_lock);

	tmp = (char *)__get_free_page(GFP_KERNEL);

	if (!tmp) {
    	path_put(path);
    	return -ENOMEM;
}

	pathname = d_path(path, tmp, PAGE_SIZE);
	path_put(path);

	if (IS_ERR(pathname)) {
    	free_page((unsigned long)tmp);
    	return PTR_ERR(pathname);
	}

	/* do something here with pathname */
	return 0;
	
}

asmlinkage int new_open(const char __user *pathname, int flags) {
	printk(KERN_INFO "[+] open() hooked.");

	char buf[32];

	strncpy_from_user(buf, pathname, sizeof(buf));	
	printk(KERN_INFO "Process name: %s", current->comm);    
	printk(KERN_INFO "File open: %s", buf);
	printk(KERN_INFO " ");
    
	return original_open(pathname, flags);
}


asmlinkage int new_write (unsigned int x, const char __user *y, size_t size) {
    printk(KERN_INFO "[+] write() hooked.");
		
	int ret = getPathName(x, current->files);
	int writtenByte;	
	
	writtenByte = original_write(x, y, size);
	if (ret == 0)
	{
		printk(KERN_INFO "Process name: %s", current->comm);
		printk(KERN_INFO "File name written: %s", pathname);
		printk(KERN_INFO "Written byte: %d", writtenByte); 
	}
	 
	printk(KERN_INFO " ");    
	return writtenByte;
}

static int __init onload(void) {
    printk(KERN_WARNING "Hello world!\n");
    printk(KERN_INFO "Syscall table address: %p\n", syscall_table);
  	

    if (syscall_table != NULL) {
        write_cr0 (read_cr0 () & (~ 0x10000));
        //original_write = (void *)syscall_table[__NR_write];
        //syscall_table[__NR_write] = &new_write;
		original_open = (void *)syscall_table[__NR_open];
		syscall_table[__NR_open] = &new_open;
        write_cr0 (read_cr0 () | 0x10000);
        printk(KERN_INFO "[+] onload: sys_call_table hooked\n");
		printk(KERN_INFO " ");
    } else {
        printk(KERN_INFO "[-] onload: syscall_table is NULL\n");
    }
  
    /*
     * A non 0 return means init_module failed; module can't be loaded.
     */
    return 0;
}

static void __exit onunload(void) {
    if (syscall_table != NULL) {
        write_cr0 (read_cr0 () & (~ 0x10000));
        //syscall_table[__NR_write] = original_write;
		syscall_table[__NR_open] = original_open;
        write_cr0 (read_cr0 () | 0x10000);		
		free_page((unsigned long)tmp);
        printk(KERN_INFO "[+] onunload: sys_call_table unhooked\n");
    } else {
        printk(KERN_INFO "[-] onunload: syscall_table is NULL\n");
    }
	
    printk(KERN_INFO "Goodbye world!\n");
	printk(KERN_INFO " ");
}

module_init(onload);
module_exit(onunload);
