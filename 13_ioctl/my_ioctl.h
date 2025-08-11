#ifndef MY_IOCTL_H_
#define MY_IOCTL_H_

struct mystruct {
	int repeat;
	char name[64];
};

#define WR_VAL _IOW('a', 'b', int *)
#define RD_VAL _IOR('a', 'c', int *)
#define GREET _IOW('a', 'd', struct mystruct *)

#endif
