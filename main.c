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


	struct timer_params t_params = {.pid = 470, .expires = -1, 
					.head = NULL, .elist = NULL, 
					.firing = -1};
	
	struct mmap_params m_params = {.pid = 470, .vm_start = -1, 
					.vm_end = -1, .vm_flags = -1,
					.vm_next = NULL, .vm_prev = NULL,
					.rb_subtree_gap = -1, .vm_mm = NULL,
					.vm_file = NULL};
	
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
	printf("\tnode.expires: %lu\n\thead: %x\n\tpid: %zu\n\telist: %x\n\tfiring: %d\n}\n",
		 t_params.expires, t_params.head, t_params.pid, t_params.elist, t_params.firing);
	
	ioctl(dev, RD_MMAP, &m_params);
	printf("struct vm_area_struct{\n");
	printf("\tvm_start: %lu\n\tvm_end: %lu\n\tvm_flags: %lu\n\tvm_prev: %x\n\tvm_next: %x\n\trb_subtree_gap: %lu\n\tvm_mm: %x\n\tvm_file: %x\n}\n", 
		m_params.vm_start, m_params.vm_end, m_params.vm_flags, m_params.vm_prev, m_params.vm_next, m_params.rb_subtree_gap, m_params.vm_mm, m_params.vm_file);
	
	close(dev);
	return 0;
}
