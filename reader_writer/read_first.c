/*
 * 读者写者问题(读者优先)
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int read_cnt = 0, total_cnt = 0;
pthread_mutex_t read_mutex, access_mutex;

/*
 * 读者优先体现在: 一旦有一个读者进入critical section, 他将锁住此区域, 所有写者不能访问
 * 但其他的读者(即使出现在写者之后)可以直接访问critical section
 */

void *write_handler(void *arg) {
    pthread_detach(pthread_self());  // joinable --> unjoinable

    pthread_mutex_lock(&access_mutex);  // avoid race condition with other writers
    ++total_cnt;
    printf("A writer comes in! Total persons: %d\n", total_cnt);
    sleep(4);  // do some writing
    --total_cnt;
    printf("A writer leaves! Total persons: %d\n", total_cnt);
    pthread_mutex_unlock(&access_mutex);
}

void *read_handler(void *arg) {
    pthread_detach(pthread_self());  // joinable --> unjoinable

    /* Entry Section */
    pthread_mutex_lock(&read_mutex);  // avoid race condition with other readers
    if(read_cnt == 0)
        pthread_mutex_lock(&access_mutex);  // forbid writer to access
    ++read_cnt;
    ++total_cnt;
    printf("A reader comes in! Total readers: %d  Total persons: %d\n", read_cnt, total_cnt);
    pthread_mutex_unlock(&read_mutex);

    /* Critical Section */
    sleep(2);  // do some reading

    /* Exit Section */
    pthread_mutex_lock(&read_mutex);
    --read_cnt;
    --total_cnt;
    printf("A reader leaves! Total readers: %d  Total persons: %d\n", read_cnt, total_cnt);
    if(read_cnt == 0)
        pthread_mutex_unlock(&access_mutex);  // allow writer to access
    pthread_mutex_unlock(&read_mutex);
}

int main()
{
    int type, res;
    srand((unsigned)time(NULL));
    if((res = pthread_mutex_init(&read_mutex, NULL)) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
    if((res = pthread_mutex_init(&access_mutex, NULL)) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    while(1) {
        type = rand();  // type: even -> write  odd -> read
        if(type % 2 == 0) {
            pthread_t tid;
            if((res = pthread_create(&tid, NULL, write_handler, NULL)) != 0) {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }
        } else {
            pthread_t tid;
            if((res = pthread_create(&tid, NULL, read_handler, NULL)) != 0) {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }
        }
        sleep(1);
    }
    pthread_mutex_destroy(&read_mutex);
    pthread_mutex_destroy(&access_mutex);
    return 0;
}
