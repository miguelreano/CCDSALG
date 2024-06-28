#include "queue.h"
#include <stdio.h>

const char *accountTypeStr[] = { "New", "Government", "Checking", "Savings" };

/**
 * Function name: initQueue
 * Description: Initialize a queue.
 * Parameters:
 *** Queue *q: Pointer to the queue to be initialized.
 */
void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1; // Set rear to -1 to indicate the queue is initially empty
    q->size = 0;
}
/**
 * Function name: isQueueFull
 * Description: Check if a queue is full based on the teller type.
 * Parameters:
 *** Queue *q: Pointer to the queue.
 *** int accountType: The type of account for the transaction.
 * Return value:
 *** int: Returns 1 if the queue is full, otherwise returns 0.
 */
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

/**
 * Function name: isQueueEmpty
 * Description: Check if a queue is empty.
 * Parameters:
 *** Queue *q: Pointer to the queue.
 * Return value:
 *** int: Returns 1 if the queue is empty, otherwise returns 0.
 */
int isQueueEmpty(Queue *q) {
    return q->size == 0;
}

/**
 * Function name: enqueue
 * Description: Add a transaction to the queue.
 * Parameters:
 *** Queue *q: Pointer to the queue.
 *** Transaction transaction: The transaction to be added.
 */
void enqueue(Queue *q, Transaction transaction) {
    if (!isQueueFull(q, transaction.accountType)) {
        q->rear = (q->rear + 1) % MAX_QUEUE_SIZE; // Update rear index, wrap around using modulus
        q->transactions[q->rear] = transaction; // Add transaction at the rear index
        q->size++; // Increment size of the queue
    } else {
        printf("|-[ ! ]- [ Queue is full. Cannot enqueue transaction %d\n", transaction.amount); // Print error if queue is full
    }
}

/**
 * Function name: dequeue
 * Description: Remove a transaction from the queue.
 * Parameters:
 *** Queue *q: Pointer to the queue.
 * Return value:
 *** Transaction: The dequeued transaction. Returns a default invalid transaction if the queue is empty.
 */
Transaction dequeue(Queue *q) {
    Transaction transaction = { -1, -1, -1 }; // Default invalid transaction
    if (!isQueueEmpty(q)) {
        transaction = q->transactions[q->front];
        q->front = (q->front + 1) % MAX_QUEUE_SIZE; // Increment front and wrap around if necessary
        q->size--;
    }
    return transaction; // Return the dequeued transaction
}

/**
 * Function name: printQueueContents
 * Description: Print the contents of the queue.
 * Parameters:
 *** Queue *q: Pointer to the queue.
 *** const char *queueName: Name of the queue to be printed.
 */
void printQueueContents(Queue *q, const char *queueName) {
    printf("|-[ ! ]-[ %s Queue:\n", queueName);
    if (isQueueEmpty(q)) {
        printf("|-[ ! ]-[ Queue is empty.\n");
    } else {
        int i = q->front;
        for (int count = 0; count < q->size; count++) {
            Transaction trans = q->transactions[i];
            printf("|-[ ! ]-[ Stub %d, Amount: %d, %s Account, Duration: %d Minutes\n",
                   trans.stubNumber, trans.amount, accountTypeStr[trans.accountType], trans.duration);
            i = (i + 1) % MAX_QUEUE_SIZE;
        }
    }
}