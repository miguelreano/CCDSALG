#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue.h"
#include "stack.h"

#define MAX_EXTRA_QUEUE_TRANSACTIONS 10

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
    *seconds = (*minutes * 60) % 60;
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
        initQueue(&tellers[4]);
        printf("Activated the 5th queue due to all regular queues being full and pending queue conditions met.\n");

        while (!isQueueEmpty(pendingQueue) && tellers[4].size < MAX_EXTRA_QUEUE_TRANSACTIONS) {
            Transaction transaction = dequeue(pendingQueue);
            enqueue(&tellers[4], transaction);
        }
    }
}

// Consolidate and sort all transactions completed by tellers in ascending order
void ConsolidateTransactions(Stack *completedTransactions, int numStacks) {
    Stack allTransactions;
    initStack(&allTransactions);

    // Collect all transactions from each stack into a single stack
    for (int i = 0; i < numStacks; i++) {
        while (!isStackEmpty(&completedTransactions[i])) {
            push(&allTransactions, pop(&completedTransactions[i]));
        }
    }

    // Sort transactions in ascending order by stub number
    for (int i = 0; i < allTransactions.top; i++) {
        for (int j = i + 1; j <= allTransactions.top; j++) {
            if (allTransactions.transactions[i].stubNumber > allTransactions.transactions[j].stubNumber) {
                Transaction temp = allTransactions.transactions[i];
                allTransactions.transactions[i] = allTransactions.transactions[j];
                allTransactions.transactions[j] = temp;
            }
        }
    }

    // Display sorted transactions
    printf("Consolidated Transactions:\n");
    for (int i = 0; i <= allTransactions.top; i++) {
        printf("Stub Number: %d, Amount: %d, Account Type: %s, Duration: %d minutes\n",
               allTransactions.transactions[i].stubNumber,
               allTransactions.transactions[i].amount,
               accountTypeStr[allTransactions.transactions[i].accountType],
               allTransactions.transactions[i].duration);
    }
}

// Display the main menu for user interaction
void displayMenu(int totalTimeElapsed) {
    int hours, minutes, seconds;
    convertTime(totalTimeElapsed, &hours, &minutes, &seconds);
    printf("\nBank Teller Simulation Menu:\n");
    printf("1. Add customer to queue\n");
    printf("2. Consolidate and display transactions\n");
    printf("3. Exit\n");
    printf("Total time elapsed: %02d:%02d:%02d (HH:MM:SS)\n", hours, minutes, seconds);
    printf("Enter your choice: ");
}

int main() {
    srand(time(NULL)); // Initialize random seed

    Queue tellers[NUM_TELLERS + 1]; // Array of queues for each teller including the 5th queue
    Stack completedTransactions[NUM_TELLERS]; // Array of stacks for completed transactions
    Queue pendingQueue; // Pending queue for overflow customers

    // Initialize queues and stacks
    for (int i = 0; i < NUM_TELLERS + 1; i++) {
        initQueue(&tellers[i]);
        if (i < NUM_TELLERS) {
            initStack(&completedTransactions[i]);
        }
    }
    initQueue(&pendingQueue);

    int choice;
    Transaction transaction;
    int stubNumber = 1; // Stub number counter
    int totalTimeElapsed = 0; // Initialize total time elapsed

    while (1) {
        displayMenu(totalTimeElapsed);
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                // Add customer to queue
                printf("Enter transaction amount: ");
                scanf("%d", &transaction.amount);
                printf("Select account type (0: New, 1: Government, 2: Checking, 3: Savings): ");
                scanf("%d", &transaction.accountType);
                transaction.duration = getRandomDuration(transaction.accountType);
                transaction.stubNumber = stubNumber++;

                // Determine the correct teller index based on account type
                int tellerIndex;
                if (transaction.accountType == NEW) {
                    tellerIndex = 0; // Teller 1 handles new accounts
                } else if (transaction.accountType == GOVERNMENT) {
                    tellerIndex = 1; // Teller 2 handles government accounts
                } else if (transaction.accountType == CHECKING || transaction.accountType == SAVINGS) {
                    // Tellers 3 and 4 handle checking and savings accounts
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
                    processTransaction(&tellers[tellerIndex], &completedTransactions[tellerIndex], &totalTimeElapsed);
                }
                break;

            case 2:
                // Consolidate and display all completed transactions
                ConsolidateTransactions(completedTransactions, NUM_TELLERS);
                break;

            case 3:
                // Exit the program
                printf("Exiting...\n");
                exit(0);

            default:
                // Handle invalid menu choice
                printf("Invalid choice. Try again.\n");
        }

        for (int i = 0; i < 5; i++) {
            char queueName[20];
            snprintf(queueName, sizeof(queueName), "Teller %d", i + 1);
            printQueueContents(&tellers[i], queueName);
        }
        printQueueContents(&pendingQueue, "Pending");
    }

    return 0;
}

