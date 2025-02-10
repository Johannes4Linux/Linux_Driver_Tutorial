#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() 
{
	int fd;
	char c;

	fd = open("/dev/hello0", O_RDWR);

	if (fd < 0) {
		perror("open");
		return fd;
	}

	while (read(fd, &c, 1)) 
		putchar(c);

	close(fd);
	return 0;
}
