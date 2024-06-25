#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue* createQueue(int capacity) {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->data = (int *)malloc(queue->capacity * sizeof(int));
    return queue;
}

int isQueueFull(Queue *queue) {
    return (queue->size == queue->capacity);
}

int isQueueEmpty(Queue *queue) {
    return (queue->size == 0);
}

void enqueue(Queue *queue, int item) {
    if (isQueueFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->data[queue->rear] = item;
    queue->size++;
}

int dequeue(Queue *queue) {
    if (isQueueEmpty(queue))
        return -1;
    int item = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}

void destroyQueue(Queue *queue) {
    free(queue->data);
    free(queue);
}


//test