
#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#define WR_VALUE _IOW('a', 'a', int32_t *)
#define RD_VALUE _IOR('a', 'b', int32_t *)
#define RD_TIMER _IOWR('a', 'c', struct timer_params *)
#define RD_MMAP  _IOWR('a', 'd', struct mmap_params *)

struct timer_params{
	size_t pid;
	unsigned long expires;
	struct timerqueue_head* head;
	struct list_head *elist;
	int firing;
};

struct mmap_params{
	size_t pid;
	unsigned long vm_start;
	unsigned long vm_end;
	unsigned long vm_flags;
	struct vm_area_struct *vm_next, *vm_prev;
	unsigned long rb_subtree_gap;
	struct mm_struct *vm_mm;
	struct file * vm_file;	
};

#endif
