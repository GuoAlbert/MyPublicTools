#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <strings.h>
#include <sys/time.h>
void check_stat(int fd)
{
	struct stat sbuf_fd={};
	fstat(fd, &sbuf_fd);
	printf("fstat: atime.sec=%ld, atime.nsec=%ld, mtime.sec=%ld, mtime.nsec=%ld, ctime.sec=%ld, ctime.nsec=%ld, blksize=%lu.\n",
			sbuf_fd.st_atim.tv_sec, sbuf_fd.st_atim.tv_nsec, sbuf_fd.st_mtim.tv_sec, sbuf_fd.st_mtim.tv_nsec,
			sbuf_fd.st_ctim.tv_sec, sbuf_fd.st_ctim.tv_nsec, sbuf_fd.st_blksize);
}

int main(int argc, char *argv[])
{

	struct stat sbuf;
	char *filePath = argv[1];
	char buf[256] = {0};
	struct timespec times[2] = {};
	struct timeval now;
	int fd = open(filePath, O_RDWR|O_TRUNC);
	if (fd < 0) {
		printf("Failed to open %s. error:%s.\n", filePath, strerror(errno));
		return 1;
	}
	write(fd, buf, sizeof(buf));
	gettimeofday(&now, NULL);
	printf("now.sec = %ld, now.usec = %ld\n", now.tv_sec, now.tv_usec);
	times[0].tv_sec = now.tv_sec - 1;
	times[0].tv_nsec = now.tv_usec * 1000;
	times[1].tv_sec = now.tv_sec - 1;
	times[1].tv_nsec = now.tv_usec * 1000;
	printf("getattr from cache:\n");
	check_stat(fd);
	int ret = futimens(fd, times);
	if (ret != 0) {
		printf("Failed to set time on %d, error:%s.\n", fd, strerror(errno));
		return 1;
	}
	printf("getattr from 9p server:\n");
	check_stat(fd);
	printf("getattr from cache:\n");
	check_stat(fd);
	close(fd);
	return 0;
}
