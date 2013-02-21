#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define MAXLINE	256

const int backlog = 4;

void *clientHandler(void *arg)
{

    char str[MAXLINE];
    int  i, n;

    int  fd = *(int*)(arg);

    while (1) {
        if ((n = read(fd, str, MAXLINE)) == 0) {
            close (fd);
            return;
        }

        for (i = 0; i < strlen(str); i++) {
            if (str[i] >= 97 && str[i] <= 122) {
                str[i] = str[i] - 32;
            }
        }

        write(fd, str, strlen(str));
    }

}

int main(int argc, char *argv[])
{

	int	listenfd, connfd;
    pthread_t tid;
	int     clilen;
	struct 	sockaddr_in cliaddr, servaddr;

	if (argc != 3) {
		printf("Usage: caseServer <address> <port> \n");
        return -1;
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
                errno, strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family 	   = AF_INET;
	servaddr.sin_addr.s_addr   = inet_addr(argv[1]);
	servaddr.sin_port          = htons(atoi(argv[2]));

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        fprintf(stderr, "Error binding to socket, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;

	}

	if (listen(listenfd, backlog) == -1) {
        fprintf(stderr, "Error listening for connection request, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;
	}

	
	while (1) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0 ) {
			if (errno == EINTR)
				continue;
			else {
                fprintf(stderr, "Error connection request refused, errno = %d (%s) \n",
                        errno, strerror(errno));
			}
		}

        if (pthread_create(&tid, NULL, clientHandler, (void *)&connfd) != 0) {
           fprintf(stderr, "Error unable to create thread, errno = %d (%s) \n",
                   errno, strerror(errno));
        }

	}
}

