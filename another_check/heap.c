#include <stdio.h>
#include <stdlib.h>
typedef struct s_heap{
    int *item;
    int len;
    int cap;
}t_heap;

void heap_init(t_heap *heap){
    heap->item = malloc(sizeof(*(heap->item))*heap->cap);
    printf("item allocated: %zu\n", sizeof(*(heap->item))*heap->cap);
    heap->len = 0;
}

void heap_push(t_heap *heap, int num){
    int index;
    int temp;
    int parent;
    
    index = heap->len;
    heap->item[index] = num;
    heap->len++;
    while (index > 0)
    {
        parent = (index-1)/2;
        printf("index = %i\n", index);
        if (heap->item[index] < heap->item[parent]){
            temp = heap->item[index];
            heap->item[index] = heap->item[parent];
            heap->item[parent] = temp;
        }
        else
            break;
        index = parent;
    }
}

int *heap_pop(t_heap *heap){
    int *top;
    int index;
    int child;
    int temp;

    if (heap->len == 0)
        return NULL;
    index = 0;
    top = malloc(sizeof(top));
    *top = heap->item[0];
    heap->item[0] = heap->item[heap->len-1];
    heap->len--;
    while (1)
    {
        child = (index+1)*2;
        if (heap->item[child] >= heap->len)
            break;
        if (heap->item[child] > heap->item[child+1])
            child++;
        if (heap->item[child] < heap->item[index]){
            temp = heap->item[child];
            heap->item[child] = heap->item[index];
            heap->item[index] = temp;
        }
        else
            break;
        index = child;
    }
    return top;
}

int main(){
    t_heap heap;
    heap.cap = 4;
    heap_init(&heap);
    heap_push(&heap, 4);
    heap_push(&heap, 9);
    heap_push(&heap, 3);
    heap_push(&heap, 1);
    printf("HEAP ITEMS\n");
    for (int i = 0; i < heap.cap; i++)
    {
        printf("%i->", heap.item[i]);
    }
    printf("NULL\n");
    for (int i = 0; i < heap.cap+5; i++){
        int *popped = heap_pop(&heap);
        if (!popped)
        {
            printf("the heap is empty\n");
            continue;
        }
        printf("popped = %i\n", *popped);
        for (int i = 0; i < heap.len; i++)
        {
            printf("%i->", heap.item[i]);
        }
        printf("NULL\n");
    }
}

