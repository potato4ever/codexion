#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int ft_atoi(char *s);

typedef struct s_seller{
    int sell_time;
    int selled;
    pthread_t   thread;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
} t_seller;

typedef struct s_producers{
    int id;
    int cooldown;
    t_seller    *seller;
    pthread_t   thread;
    pthread_mutex_t *mutex;
    pthread_cond_t  *cond;
} t_producers;

int closing = 0;
int start = 0;
int gb_product = 0;

void    *producers_routine(void *arg){
    t_producers *producer = (t_producers *) arg;
    pthread_mutex_lock(producer->mutex);
    while (!start)
    {
        printf("producer %i wait until opening\n", producer->id);
        pthread_cond_wait(producer->cond, producer->mutex);
    }
    pthread_mutex_unlock(producer->mutex);
    pthread_mutex_lock(producer->mutex);
    while (producer->seller->selled != producer->seller->sell_time)
    {
        if (gb_product == 100)
        {
            while (gb_product && (producer->seller->selled != producer->seller->sell_time))
            {
                printf("the producer %i wait for the seller to finish selling\n", producer->id);
                pthread_cond_broadcast(producer->cond);
                pthread_cond_wait(producer->cond, producer->mutex);
            }
        }
        pthread_mutex_unlock(producer->mutex);
        usleep(producer->cooldown * 1000);
        pthread_mutex_lock(producer->mutex);
        if (producer->seller->selled == producer->seller->sell_time)
            break;
        gb_product += 20;
        printf("producer %i produces. products: %i\n", producer->id, gb_product);
    }
    printf("producer %i finishes it's work available products: %i\n", producer->id, gb_product);
    pthread_cond_broadcast(producer->cond);
    pthread_mutex_unlock(producer->mutex);
    return NULL;
}

void    *seller_routine(void *arg){
    t_seller *seller = (t_seller *) arg;
    pthread_mutex_lock(seller->mutex);
    while (seller->selled != seller->sell_time)
    {
        while (gb_product != 100)
        {
            printf("seller waits for producers\n");
            pthread_cond_wait(seller->cond, seller->mutex);
        }
        printf("the seller sells the products\n");
        gb_product = 0;
        seller->selled++;
        printf("seller finishes it's work gb_product = %i\n", gb_product);
        pthread_cond_broadcast(seller->cond);
    }
    pthread_cond_broadcast(seller->cond);
    pthread_mutex_unlock(seller->mutex);
    return NULL;
}

int main(int argc, char **argv){
    if (argc != 4)
        return (1);

    int PRODUCERS_NUM = ft_atoi(argv[1]);
    int SELL_TIME = ft_atoi(argv[3]);

    t_producers producers[PRODUCERS_NUM];
    t_seller    seller;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    seller.mutex = &mutex;
    seller.cond = &cond;
    seller.sell_time = SELL_TIME;
    seller.selled = 0;
    pthread_create(&seller.thread, NULL, seller_routine, &seller);
    for (int i = 0; i < PRODUCERS_NUM; i++){
        producers[i].id = i + 1;
        producers[i].cooldown = ft_atoi(argv[2]);
        producers[i].mutex = &mutex;
        producers[i].cond = &cond;
        producers[i].seller = &seller;
        pthread_create(&producers[i].thread, NULL, producers_routine, &producers[i]);
    }
    pthread_mutex_lock(&mutex);
    start = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    for (int i = 0; i < PRODUCERS_NUM; i++)
        pthread_join(producers[i].thread, NULL);
    pthread_mutex_lock(&mutex);
    closing = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    pthread_join(seller.thread, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
