#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

#define MAX_SIZE 32
 
int main(){
 
    char name[32];
    puts("Enter process to find");
    scanf("%s",name);
    //strtok(name, "\n");
    long int status = syscall(333, name);
    printf("sys_pnametoid returned %ld\n", status);
	
	int pid;
	char buff[MAX_SIZE];
	puts("Enter process ID: ");
	scanf("%d", &pid);
	long int status334 = syscall(334, pid, buff, MAX_SIZE);
	printf("sys_pidtoname returned %ld, %s\n", status334, buff); 

    return 0;
}
