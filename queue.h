#ifndef QUEUE_H
#define QUEUE_H

#include "transaction.h"

// Define constants for maximum sizes and the number of tellers
#define MAX_QUEUE_SIZE 40
#define NUM_TELLERS 5
#define MAX_PENDING_QUEUE 40

// Define account types
#define NEW 0
#define GOVERNMENT 1
#define CHECKING 2
#define SAVINGS 3

extern const char *accountTypeStr[];

// Define maximum customers per queue
#define MAX_NEW_QUEUE 3
#define MAX_GOV_QUEUE 4
#define MAX_CHECKING_QUEUE 5
#define MAX_SAVINGS_QUEUE 5

// Define a Queue data structure
typedef struct {
    Transaction transactions[MAX_QUEUE_SIZE]; 
    int front; 
    int rear;  
    int size;  
} Queue;

// Function declarations
void initQueue(Queue *q);
int isQueueFull(Queue *q, int accountType);
int isQueueEmpty(Queue *q);
void enqueue(Queue *q, Transaction transaction);
Transaction dequeue(Queue *q);
void printQueueContents(Queue *q, const char *queueName);

#endif // QUEUE_H
