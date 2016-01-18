/*
 * 读者写者问题(写者优先)
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int read_cnt = 0, write_cnt = 0, total_cnt = 0;
pthread_mutex_t read_mutex, write_mutex, resource_mutex, read_try_mutex;

/*
 * 写者优先体现在: 一旦写者出现, 他将设置读者的尝试锁read_try_mutex, 然后等待该读者读完后接管resource的访问权.
 * 此时只有写者有权竞争critical section, 其余所有读者(即使在写者前出现)均被锁住
 */
void *write_handler(void *arg) {
    pthread_detach(pthread_self());  // joinalbe --> unjoinable

    /* Entry Section */
    pthread_mutex_lock(&write_mutex);  // avoid race condition with other writers
    ++write_cnt;
    ++total_cnt;
    if(write_cnt == 1)
        pthread_mutex_lock(&read_try_mutex);  // "first writer" -> lock the readers out
    printf("A writer comes in! Total readers: %d  Total writers: %d  Total persons: %d\n", read_cnt, write_cnt, total_cnt);
    pthread_mutex_unlock(&write_mutex);

    /* Critical Section */
    pthread_mutex_lock(&resource_mutex);  // Prevents other writers from modifying simultaneously
    sleep(2);  // do some writing
    pthread_mutex_unlock(&resource_mutex);

    /* Exit Section */
    pthread_mutex_lock(&write_mutex);
    --write_cnt;
    --total_cnt;
    if(write_cnt == 0)
        pthread_mutex_unlock(&read_try_mutex);  // "last writer" -> unlock the readers
    printf("A writer leaves! Total readers: %d  Total writers: %d  Total persons: %d\n", read_cnt, write_cnt, total_cnt);
    pthread_mutex_unlock(&write_mutex);
}

void *read_handler(void *arg) {
    pthread_detach(pthread_self());

    /* Entry Section */
    pthread_mutex_lock(&read_try_mutex);  // a reader is trying to enter
    pthread_mutex_lock(&read_mutex);  // avoid race condition with other readers
    ++read_cnt;
    ++total_cnt;
    if(read_cnt == 1)
        pthread_mutex_lock(&resource_mutex);  // "first reader" -> lock the resource
    printf("A reader comes in! Total readers: %d  Total writers: %d  Total persons: %d\n", read_cnt, write_cnt, total_cnt);
    pthread_mutex_unlock(&read_mutex);
    pthread_mutex_unlock(&read_try_mutex);

    /* Critical Section */
    sleep(4);  // do some reading

    /* Exit Section */
    pthread_mutex_lock(&read_mutex);
    --read_cnt;
    --total_cnt;
    if(read_cnt == 0)
        pthread_mutex_unlock(&resource_mutex);  // "last reader" -> unlock the resource
    printf("A reader leaves! Total readers: %d  Total Writers: %d  Total persons: %d\n", read_cnt, write_cnt, total_cnt);
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
    if((res = pthread_mutex_init(&write_mutex, NULL)) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
    if((res = pthread_mutex_init(&resource_mutex, NULL)) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
    if((res = pthread_mutex_init(&read_try_mutex, NULL)) != 0) {
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
    pthread_mutex_destroy(&write_mutex);
    pthread_mutex_destroy(&resource_mutex);
    pthread_mutex_destroy(&read_try_mutex);
    return 0;
}
