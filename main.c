#include <stdlib.h> /* required for rand() */
#include <pthread.h>
#include <unistd.h> 
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>


#define BUFFER_SIZE     5
typedef int buffer_item;
int in=0;
int out=0;
int count=0;
pthread_t tid; 
pthread_attr_t attr; 
sem_t mutex;
sem_t empty;
sem_t full;

void *producer();
void *consumer();
int insert_item(buffer_item);
int remove_item(buffer_item*);
void initialize_data();

/*Initialize buffer */
buffer_item buffer[BUFFER_SIZE];

void *producer() {
    buffer_item item;
    while (1) {

        /* sleep for a random period of time */
        sleep(rand()%10+3);
        /* generate a random number */
        item = rand()%50;
        if (insert_item(item))
            printf("report error condition");
    }
}

void *consumer() {
    buffer_item item;
    while (1) {
        /* sleep for a random period of time */
        sleep(rand()%10+3);
        if (remove_item(&item))
            printf("report error condition");
    }
}


int insert_item(buffer_item item) {
    sem_wait(&empty);
    sem_wait(&mutex);

    /* add next produced to the buffer */
    if(count==BUFFER_SIZE)
    {
        return -1; //error condition
    }
    else
    {
        ++count;
        buffer[in]= item;
        in = (in+1)%BUFFER_SIZE;
        printf("producer produced %d \n",item);

        sem_post(&mutex);
        sem_post(&full);
        return 0; //successful
    }   
}

int remove_item(buffer_item *item) {
    sem_wait(&full);
    sem_wait(&mutex);

    /* remove an item from buffer to next consumed */
    if(count==0)
    {
        return -1; //error condition
    }
    else
    {   
        --count;
        *item = buffer[out];
        out=(out+1)%BUFFER_SIZE;
	printf("consumer consumed %d \n", *item);
        sem_post(&mutex);
        sem_post(&empty);
        return 0; //successful
    }
    /* consume the item in next consumed */ 
}


void initialize_data() {

    sem_init(&mutex, 0, 1);

    sem_init(&full, 0, 0);

    sem_init(&empty, 0, BUFFER_SIZE);

    pthread_attr_init(&attr);

}



int main(int argc, char *argv[]) {
	
    /*Input Validation*/
    int flag = 0;
    if (argc != 4 || atoi(argv[1])==0 || atoi(argv[2])==0 || atoi(argv[3])==0)
    {
        printf("Invalid Input! \n");
    }
    else
    {
	for (int i=0;i<strlen(argv[1]) && !flag;i++)
		if(!isdigit(argv[1][i]))
			flag=1;
	for (int i=0;i<strlen(argv[2]) && !flag;i++)
		if(!isdigit(argv[2][i]))
			flag=1;
	for (int i=0;i<strlen(argv[3]) && !flag;i++)
		if(!isdigit(argv[3][i]))
			flag=1;
	if(flag)
	{
		printf("Invalid Input! \n");
		printf("Exit the program \n");
    		exit(0);
	} 
        srand(time(0));
        initialize_data();
 
        /* Create producer thread(s) */
        for ( int i = 0; i < atoi(argv[2]); i++) 
            pthread_create(&tid, &attr, producer, NULL); 

        /* Create consumer thread(s) */
        for ( int i = 0; i < atoi(argv[3]); i++) 
            pthread_create(&tid, &attr, consumer, NULL); 

        /* Sleep */
        sleep(atoi(argv[1]));
    }	
    

    /* Exit */
    printf("Exit the program \n");
    exit(0);
}

