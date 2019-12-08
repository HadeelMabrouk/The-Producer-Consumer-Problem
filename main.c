#include <stdlib.h> /* required for rand() */
#include <pthread.h>
#include <unistd.h> 
#include <semaphore.h>
#include <stdio.h>
#include <time.h>


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
        else
            printf("producer produced %d \n",item);
    }
}

void *consumer() {
    buffer_item item;
    while (1) {
        /* sleep for a random period of time */
        sleep(rand()%10+3);
        if (remove_item(&item))
            printf("report error condition");
        else
            printf("consumer consumed %d \n",item);
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
 
    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    /*cout << "You have entered " << argc << " arguments:" << "\n"; 
    for (int i = 0; i < argc; ++i) 
        cout << argv[i] << "\n"; */
    srand(time(0));

    initialize_data();
 
    /* 3. Create producer thread(s) */
    for ( int i = 0; i < atoi(argv[2]); i++) 
        pthread_create(&tid, &attr, producer, NULL); 

    /* 4. Create consumer thread(s) */
    for ( int i = 0; i < atoi(argv[3]); i++) 
        pthread_create(&tid, &attr, consumer, NULL); 

    /* 5. Sleep */
    sleep(atoi(argv[1]));

    /* 6. Exit */
    printf("Exit the program \n");
    exit(0);
}

