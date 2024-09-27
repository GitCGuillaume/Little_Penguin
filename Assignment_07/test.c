#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define PATH "/sys/kernel/debug/fortytwo/foo"

void	*routine_read(void *ptr)
{
	char str[20];
	int fd = open(PATH, O_RDONLY);

	(void)ptr;
	if (fd < 0) {
		printf("err open fd\n");
		return (NULL);
	}
	memset(str, 0, 20);
	read(fd, str, 20);
	printf("str: %s", str);
	close(fd);
	return (NULL);
}

/*
 * need write permission though
 */
void	*routine_write(void *ptr)
{
	char str[20];
	int fd = open(PATH, O_WRONLY);

	if (fd < 0) {
		printf("err open fd\n");
		return (NULL);
	}
	memset(str, 0, 20);
	write(fd, (char *)ptr, strlen((char *)ptr));
	close(fd);
	return (NULL);
}

int	main(void)
{
	char *str[] = {"0000000\n", "1111\n",
		"2222\n", "3333\n",
		"4444\n"
	};
	pthread_t thr1[5];
	pthread_t thr2[5];

	for (int i = 0; i < 5; i++) {
		pthread_create(&thr1[i], NULL, routine_write, str[i]);
		pthread_create(&thr2[i], NULL, routine_read, NULL);
	}
	for (int i = 0; i < 5; i++) {
		pthread_join(thr1[i], NULL);
		pthread_join(thr2[i], NULL);
	}
	return 0;
}
