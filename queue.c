#include "queue.h"
#include <stdio.h>

const char *accountTypeStr[] = { "New", "Government", "Checking", "Savings" };

// Initialize a queue
void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1; // Set rear to -1 to indicate the queue is initially empty
    q->size = 0;
}

// Check if a queue is full based on teller type
int isQueueFull(Queue *q, int accountType) {
    switch (accountType) {
        case NEW:
            return q->size == MAX_NEW_QUEUE;
        case GOVERNMENT:
            return q->size == MAX_GOV_QUEUE;
        case CHECKING:
            return q->size == MAX_CHECKING_QUEUE;
        case SAVINGS:
            return q->size == MAX_SAVINGS_QUEUE;
        default:
            return 1; // Should never happen
    }
}

// Check if a queue is empty
int isQueueEmpty(Queue *q) {
    return q->size == 0;
}

// Add a transaction to the queue
void enqueue(Queue *q, Transaction transaction) {
    if (!isQueueFull(q, transaction.accountType)) {
        q->rear = (q->rear + 1) % MAX_QUEUE_SIZE; // Update rear index, wrap around using modulus
        q->transactions[q->rear] = transaction; // Add transaction at the rear index
        q->size++; // Increment size of the queue
    } else {
        printf("Queue is full. Cannot enqueue transaction %d\n", transaction.amount); // Print error if queue is full
    }
}

// Remove a transaction from the queue
Transaction dequeue(Queue *q) {
    Transaction transaction = { -1, -1, -1 }; // Default invalid transaction
    if (!isQueueEmpty(q)) {
        transaction = q->transactions[q->front];
        q->front = (q->front + 1) % MAX_QUEUE_SIZE; // Increment front and wrap around if necessary
        q->size--;
    }
    return transaction; // Return the dequeued transaction
}

// Print the contents of the queue
void printQueueContents(Queue *q, const char *queueName) {
    printf("Contents of %s queue:\n", queueName);
    if (isQueueEmpty(q)) {
        printf("Queue is empty.\n");
    } else {
        int i = q->front;
        for (int count = 0; count < q->size; count++) {
            Transaction trans = q->transactions[i];
            printf("Stub: %d, Amount: %d, Account Type: %s, Duration: %d\n",
                   trans.stubNumber, trans.amount, accountTypeStr[trans.accountType], trans.duration);
            i = (i + 1) % MAX_QUEUE_SIZE;
        }
    }
}