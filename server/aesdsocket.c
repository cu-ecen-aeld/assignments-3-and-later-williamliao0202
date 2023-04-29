#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

#define PORT "9000"
#define BACKLOG 10
#define FILENAME "/var/tmp/aesdsocketdata"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool caught_signal = false;
static void signal_handler(int signal_number)
{
    if( signal_number == SIGINT || signal_number == SIGTERM)
    {
        caught_signal = true;
        syslog(LOG_INFO, "Caught signal, exiting\n"); 
    }
}

int main(int argc, char *argv[])
{
    int sockfd, new_fd, pid;
    struct addrinfo hints;
    struct addrinfo *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction new_action;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int buffer_size = 1024;
    int read_size = 0;
    char* buffer = (char*)malloc(buffer_size);
	ssize_t nr;
	int fd;
    
    printf("argv[1] : %s\n", argv[1]);

	openlog(NULL, 0, LOG_USER);

	memset(&new_action,0, sizeof(struct sigaction));
    new_action.sa_handler=signal_handler;
    if (sigaction(SIGTERM, &new_action, NULL) !=0)
    {
        printf("Error %d (%s) registering for SIGTERM\n", errno, strerror(errno));
    }
    if (sigaction(SIGINT, &new_action, NULL))
    {
        printf("Error %d (%s) registering for SIGTINT\n", errno, strerror(errno));
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }
    
    freeaddrinfo(servinfo);

    if (p == NULL)  
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) 
    {
        perror("listen");
        exit(1);
    }
    
    if (argc == 2 && (strcmp(argv[1], "-d") == 0))
    {
        syslog(LOG_DEBUG, "Create a Daemon");
        pid  = fork();
        if (pid == -1)
        {
            exit(1);
        }
        else if (pid != 0)
        {
            exit(EXIT_SUCCESS);
        }
        setsid();
        chdir("/");
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        open("/dev/null", O_RDWR);
        dup(0);
        dup(0);

    }
    while(1)
    {
        if (caught_signal == true)
        {
            close(sockfd);
            remove(FILENAME);
            free(buffer);
            break;
        }
        
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, 
                get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);

        printf("Accepted connection from %s\n", s);
	    syslog(LOG_INFO, "Accepted connection from %s\n", s);
	
    	fd = open(FILENAME, O_RDWR | O_CREAT | O_APPEND, 0644);
    	if (fd == -1)
	    {
		    fprintf(stderr, "Value of errno attempting to open/create file %s: %d\n", FILENAME, errno);
		    perror("perror returned");
		    fprintf(stderr, "Error opening file %s: %s\n", FILENAME, strerror(errno));
		    syslog(LOG_ERR, "The file could not be create.");
            exit(1);
	    }
        
        while((read_size = recv(new_fd, buffer, buffer_size, 0))>0)
        {
 	        nr = write(fd, buffer, read_size);
        	if (nr == -1)
	        {
		        syslog(LOG_ERR,"Error happened during writting.");
	        }
            if (buffer[read_size-1] == '\n')
            {
                break;
            }   
        }
        
        // set file descriptor to the start of the file
        if (lseek(fd, 0, SEEK_SET) == -1) {
            perror("lseek");
            exit(1);
        }

        while ((read_size = read(fd, buffer, buffer_size)) > 0)
        {
            if ((send(new_fd, buffer, read_size, 0)) == -1) 
            {
                perror("send");
                exit(1);
            }
        }
        
        if(read_size < 0)
        {
            perror("read error");
        }

        if (close(fd) == -1) 
        {
            perror("close server socket");
            exit(1);
        }
        if (close(new_fd) == -1)
        {
            perror("close client socket");
            exit(1);
        }
	    syslog(LOG_INFO, "Closed connection from %s\n", s);

    }
    return 0;
}
