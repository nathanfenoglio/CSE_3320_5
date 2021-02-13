#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_helloworld 333

int main(int argc, char *argv[]){
	int ret = syscall(__NR_helloworld);
	printf("%d\n", ret); //just printing return value of the system call

	return 0;
}

