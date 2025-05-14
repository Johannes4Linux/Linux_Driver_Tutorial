#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MEMSIZE 64

int main() {
	int fd;
	char *text;

	text = malloc(MEMSIZE);
	if (!text) {
		perror("malloc");
		return 1;
	}

	fd = open("/dev/hello0", O_RDWR);
	if (fd < 0) {
		perror("open");
		free(text);
		return 1;
	}

	sprintf(text, "Hello World");
	write(fd, text, strlen(text));

	lseek(fd, 0, SEEK_SET);

	memset(text, 0, MEMSIZE);

	read(fd, text, MEMSIZE);
	printf("%s\n", text);

	free(text);
	return 0;
}
