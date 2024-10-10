#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

#define SIZE 4096
#define DEVICE_PATH "/dev/reverse"

void	*routine_write_a(void *ptr)
{
	if (!ptr)
		return NULL;
	char *str = ptr;
	int fd = open(DEVICE_PATH, O_WRONLY);
	int ret;

	if (fd < 0) {
		printf("%s\n", "Couldn't write to device");
		return NULL;
	}
	usleep(6);
	ret = write(fd, str, strlen(str));
	printf("ret:%d\n", ret);
	if (fd >= 0)
		close(fd);
	return NULL;
}
void	*routine_write_b(void *ptr)
{
	if (!ptr)
		return NULL;
	char *str = ptr;
	int fd = open(DEVICE_PATH, O_WRONLY);
	int ret;

	if (fd < 0) {
		printf("%s\n", "Couldn't write to device");
		return NULL;
	}
	ret = write(fd, str, strlen(str));
	printf("ret:%d\n", ret);
	if (fd >= 0)
		close(fd);
	return NULL;
}
void	*routine_read(void *ptr)
{
	char str[4096] = { 0 };
	int fd = open(DEVICE_PATH, O_RDONLY);

	(void)ptr;
	if (fd < 0) {
		printf("%s\n", "Couldn't read to device");
		return NULL;
	}
	read(fd, str, 4096);
	printf("%s\n", str);
	if (fd >= 0)
		close(fd);
	return NULL;
}

int	main(void)
{
	pthread_t thr1_write[2];
	pthread_t thr2_read;
	char str_a[SIZE];
	char str_b[SIZE];
	int fd_a;
	int fd_b;

	fd_a = open("a.txt", O_RDONLY);
	if (fd_a < 0)
		return EXIT_FAILURE;
	fd_b = open("b.txt", O_RDONLY);
	if (fd_b < 0) {
		close(fd_a);
		return EXIT_FAILURE;
	}
	memset(str_a, 0, sizeof(str_a));
	memset(str_b, 0, sizeof(str_b));
	read(fd_a, str_a, sizeof(str_a));
	read(fd_b, str_b, sizeof(str_b));
	str_a[4095] = 0;
	str_b[4095] = 0;
	/* WRITE */
	pthread_create(&thr1_write[0], NULL, routine_write_a, str_a);
	pthread_create(&thr1_write[1], NULL, routine_write_b, str_b);
	/* READ */
	pthread_create(&thr2_read, NULL, routine_read, NULL);
	/* CLEAN */
	pthread_join(thr1_write[0], NULL);
	pthread_join(thr1_write[1], NULL);
	pthread_join(thr2_read, NULL);
	/* WRITE */
	usleep(100);
	pthread_create(&thr1_write[0], NULL, routine_write_b, "abc");
	/* READ */
	usleep(100);
	pthread_create(&thr2_read, NULL, routine_read, NULL);
	/* CLEAN */
	pthread_join(thr1_write[0], NULL);
	pthread_join(thr2_read, NULL);
	close(fd_a);
	close(fd_b);
	return 0;
}
