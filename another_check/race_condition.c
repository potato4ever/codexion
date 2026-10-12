#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
typedef struct s_st{
    pthread_t   thread;
    pthread_mutex_t mutex;
    int i;
}t_st;

void    *thread_routine(void *arg){
    t_st *st = (t_st *) arg;
    for (int j = 0; j < 1000000; j++)
    {
        pthread_mutex_lock(&st->mutex);
        if (st->i == 100000)
        {
            st->i++;
            printf("i entered the condition\n");
            pthread_mutex_unlock(&st->mutex);
            break;
        }
        st->i++;
        pthread_mutex_unlock(&st->mutex);
    }
    return NULL; 
}

int main(){
    t_st    st[2];
    st[0].i = 0;
    pthread_mutex_init(&st[0].mutex, NULL);
    for (int i = 0; i < 2; i++)
    {
        pthread_create(&st[i].thread, NULL, thread_routine, &st[0]);
    }
    for (int i = 0; i < 2; i++)
        pthread_join(st[i].thread, NULL);
    pthread_mutex_destroy(&st[0].mutex);
    
}
