#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *message_printer(void *ptr)
{

	char	*msg;

	msg = (char *) ptr;
	printf("%x: %s \n", pthread_self(), msg);

    pthread_exit(0);
}

int main(int argc, char *argv[])
{

    char *msg = NULL;
    pthread_t   tid[5];
    int i;

    if (argc != 2 ) {
       printf("Usage: %s <msg> \n", argv[0]);
       exit( -1);
    }

    msg = argv[1];

    for (i = 0; i < 5; i++) {
	printf("Creating thread %d ... \n", i);
	pthread_create(&tid[i], NULL, &message_printer,
				(void *)msg);
    }

    printf("Main program \n");

    for (i = 0; i < 5; i++) {
	pthread_join(tid[i], NULL);
	printf("Completed join with thread %d \n", i);
    }

    exit(0);

}

