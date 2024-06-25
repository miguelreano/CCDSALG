#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    int *data;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

Queue* createQueue(int capacity);
int isQueueFull(Queue *queue);
int isQueueEmpty(Queue *queue);
void enqueue(Queue *queue, int item);
int dequeue(Queue *queue);
void destroyQueue(Queue *queue);

#endif // QUEUE_H