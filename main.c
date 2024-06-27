#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue.h"
#include "stack.h"

#define MAX_EXTRA_QUEUE_TRANSACTIONS 10
#define NUM_TELLERS 5

// Generate a random duration for the transaction based on the account type
int getRandomDuration(int accountType) {
    int min, max;
    switch (accountType) {
        case NEW:
            min = 8; max = 10;
            break;
        case GOVERNMENT:
            min = 10; max = 15;
            break;
        case CHECKING:
            min = 5; max = 8;
            break;
        case SAVINGS:
            min = 5; max = 7;
            break;
        default:
            min = 0; max = 0; // Should never happen
    }
    return min + rand() % (max - min + 1);
}

// Process a transaction and update the total time elapsed
void processTransaction(Queue *q, Stack *s, int *totalTimeElapsed) {
    while (!isQueueEmpty(q)) {
        Transaction trans = dequeue(q);
        push(s, trans);
        *totalTimeElapsed += trans.duration;
        printf("Processed transaction stub %d, amount %d, account type %s, duration %d minutes\n",
               trans.stubNumber, trans.amount, accountTypeStr[trans.accountType], trans.duration);
    }
}

// Convert total minutes into hours, minutes, and seconds format
void convertTime(int totalTimeElapsed, int *hours, int *minutes, int *seconds) {
    *hours = totalTimeElapsed / 60;
    *minutes = totalTimeElapsed % 60;
    *seconds = 0; // No need to compute seconds from minutes
}

// Activate the 5th queue if all regular queues (savings and checking) are full and pending queue is at least 50% capacity
void OpenNewQueue(Queue *tellers, Queue *pendingQueue, int pendingQueueCondition) {
    int allFull = 1;
    // Check only checking and savings tellers (indices 2 and 3)
    for (int i = 2; i <= 3; i++) {
        if (!isQueueFull(&tellers[i], CHECKING) || !isQueueFull(&tellers[i], SAVINGS)) {
            allFull = 0;
            break;
        }
    }

    if (allFull && pendingQueue->size >= (pendingQueueCondition / 2)) {
        printf("Opening 5th queue due to high pending queue and full regular queues.\n");
        initQueue(&tellers[4]);
    }
}

// Consolidate transactions from all stacks into a single stack and display them
void ConsolidateTransactions(Stack *completedTransactions, int numTellers) {
    Stack consolidatedStack;
    initStack(&consolidatedStack);

    for (int i = 0; i < numTellers; i++) {
        while (!isStackEmpty(&completedTransactions[i])) {
            push(&consolidatedStack, pop(&completedTransactions[i]));
        }
    }

    printf("Consolidated transactions:\n");
    while (!isStackEmpty(&consolidatedStack)) {
        Transaction trans = pop(&consolidatedStack);
        printf("Transaction stub %d, amount %d, account type %s, duration %d minutes\n",
               trans.stubNumber, trans.amount, accountTypeStr[trans.accountType], trans.duration);
    }
}

int main() {
    srand(time(NULL));

    Queue tellers[NUM_TELLERS];
    Stack completedTransactions[NUM_TELLERS];
    Queue pendingQueue;

    // Initialize all queues and stacks
    for (int i = 0; i < NUM_TELLERS; i++) {
        initQueue(&tellers[i]);
        initStack(&completedTransactions[i]);
    }
    initQueue(&pendingQueue);

    int totalTimeElapsed = 0;
    int stubNumber = 1; // Initialize the stub number

    // Main loop
    while (1) {
        int choice;
        int hours, minutes, seconds;
        convertTime(totalTimeElapsed, &hours, &minutes, &seconds);
        printf("\nBank Teller Simulation Menu:\n");
        printf("1. Add customer to queue\n");
        printf("2. Consolidate and display transactions\n");
        printf("3. Exit\n");
        printf("Total time elapsed: %02d:%02d:%02d (HH:MM:SS)\n", hours, minutes, seconds);
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                Transaction transaction;
                transaction.stubNumber = stubNumber++; // Automatically assign a stub number
                printf("Enter transaction details (amount, account type): ");
                scanf("%d %d", &transaction.amount, &transaction.accountType);
                transaction.duration = getRandomDuration(transaction.accountType);

                int tellerIndex;
                if (transaction.accountType == NEW || transaction.accountType == GOVERNMENT) {
                    tellerIndex = transaction.accountType; // New and Government accounts have dedicated queues
                } else if (transaction.accountType == CHECKING || transaction.accountType == SAVINGS) {
                    // Distribute evenly among the available tellers
                    tellerIndex = 2 + (transaction.accountType == CHECKING ? 0 : 1);
                } else {
                    printf("Invalid account type. Transaction ignored.\n");
                    continue;
                }

                // Check if teller queue is full
                if (isQueueFull(&tellers[tellerIndex], transaction.accountType)) {
                    // Check if pending queue is full
                    if (isQueueFull(&pendingQueue, NEW) && isQueueFull(&pendingQueue, GOVERNMENT) &&
                        isQueueFull(&pendingQueue, CHECKING) && isQueueFull(&pendingQueue, SAVINGS)) {
                        OpenNewQueue(tellers, &pendingQueue, MAX_PENDING_QUEUE);
                        if (tellers[4].size < MAX_EXTRA_QUEUE_TRANSACTIONS) {
                            enqueue(&tellers[4], transaction);
                        } else {
                            printf("Extra queue is full. Cannot enqueue transaction.\n");
                        }
                    } else {
                        enqueue(&pendingQueue, transaction);
                        printf("Transaction enqueued to pending queue.\n");
                    }
                } else {
                    enqueue(&tellers[tellerIndex], transaction);
                }
                break;
            }

            case 2:
                // Consolidate and display all completed transactions without processing pending and queued transactions
                ConsolidateTransactions(completedTransactions, NUM_TELLERS);
                break;

            case 3:
                // Consolidate and display all completed transactions before exiting
                ConsolidateTransactions(completedTransactions, NUM_TELLERS);
                printf("Exiting...\n");
                exit(0);

            default:
                // Handle invalid menu choice
                printf("Invalid choice. Try again.\n");
        }

        // Print contents of each teller queue
        for (int i = 0; i < NUM_TELLERS; i++) {
            char queueName[20];
            snprintf(queueName, sizeof(queueName), "Teller %d", i + 1);
            printQueueContents(&tellers[i], queueName);
        }
        printQueueContents(&pendingQueue, "Pending");
    }

    return 0;
}
