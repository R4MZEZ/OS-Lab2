#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <signal.h>

#include "main.h"

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN


int main() {


	struct timer_params t_params = {.pid = 467, .expires = -1, .head = NULL};
	struct mmap_params m_params = {.pid = 467, .vm_start = -1, .vm_end = -1, .vm_flags = -1 };
	
/*	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = &timerid;
	if (timer_create(CLOCKID, &sev, &timerid) == -1)
		printf("timer_create error\n");

	printf("timer ID is %#jx\n", (uintmax_t) timerid);
*/	
	
	int dev = open("/dev/ramzezdriver", O_WRONLY);
	if(dev == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}
	printf("Opening was successfull!\n");

	ioctl(dev, RD_TIMER, &t_params);
	printf("struct cpu_timer{\n");
	printf("\tnode.expires: %lu\n\thead: %x\n\tpid: %zu\n}\n", t_params.expires, t_params.head, t_params.pid);
	
	ioctl(dev, RD_MMAP, &m_params);
	printf("struct vm_area_struct{\n");
	printf("\tvm_start: %lu\n\tvm_end: %lu\n\tvm_flags: %lu\n}\n", m_params.vm_start, m_params.vm_end, m_params.vm_flags);
	
	close(dev);
	return 0;
}
