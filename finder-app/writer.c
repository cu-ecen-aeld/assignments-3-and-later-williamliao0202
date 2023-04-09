#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


int main(int argc, char *argv[])
{
	ssize_t nr;
	int fd;
	const int paranum = argc;
	const char *writefile = argv[1];
	const char *writestr = argv[2];

	openlog(NULL, 0, LOG_USER);
	
	if (paranum != 3)
	{
		printf("Invalid Number of arguments: %d\n", paranum);
		syslog(LOG_ERR, "Invalid Number of arguments: %d\n", paranum);
		return 1;	
	}
	
	fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		fprintf(stderr, "Value of errno attempting to open/create file %s: %d\n", writefile, errno);
		perror("perror returned");
		fprintf(stderr, "Error opening file %s: %s\n", writefile, strerror(errno));
		syslog(LOG_ERR, "The file could not be create.");
		return 1;
	}
	
	nr = write(fd, writestr, strlen(writestr));
	if (nr == -1)
	{
		syslog(LOG_ERR,"Error happened during writting.");
	}

	printf("Writing %s to %s\n", writestr, argv[1]);
	syslog(LOG_DEBUG, "Writing %s to %s\n", writestr, writefile);
	
	return 0;
}
