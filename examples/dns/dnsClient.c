/* Simple example on how to get name resolution using DNS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


extern int h_errno;

int main(int argc, char *argv[])
{

    struct hostent *hent;
    struct in_addr addr;
    int i;

    if (argc != 2) {
        printf("Usage:: %s <hostname> \n", argv[0]);
        return -1;
    }

    hent = gethostbyname(argv[1]);

    if (hent == NULL) {
        fprintf(stderr, "gethostbyname() -- error = %d \n", h_errno);
        return -1;
    }

    printf("\nHost Name: '%s' \n", hent->h_name);
    while (hent->h_addr_list[i] != NULL) {
        memcpy((void *)&addr, (void *)hent->h_addr_list[i],sizeof(addr));
        printf("Address: %s \n", inet_ntoa(addr));
        i++;
    } 

    printf("\n");

    return 0;

}

