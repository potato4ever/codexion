#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int i = 0;
void    *thread_routine(void *arg){
    pthread_mutex_t *num = (pthread_mutex_t *) arg;
    pthread_mutex_lock(num);
    for (int j = 0; j < 100000; j++)
        i++;
    pthread_mutex_unlock(num);
    return NULL;
}

int main()
{
    pthread_t th1;
    pthread_t th2;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&th1, NULL, thread_routine, &mutex);
    pthread_create(&th2, NULL, thread_routine, &mutex);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    printf("num = %i\n", i); 
}
