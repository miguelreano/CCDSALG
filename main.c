#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue.h"
#include "stack.h"
#include "transaction.h"

#define MAX_EXTRA_QUEUE_TRANSACTIONS 10
#define NUM_TELLERS 5
#define MAX_TRANSACTIONS 60

typedef struct {
    Transaction currentTransaction;
    int isBusy;
    int remainingTime;
} TellerStatus;

/**
 * Function name: getRandomDuration
 * Description: Generate a random duration for the transaction based on the account type.
 * Parameters:
 *** int accountType: The type of account for the transaction.
 * Return value:
 *** int: The random duration for the transaction.
 */
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
/**
 * Function name: processTransaction
 * Description: Process a single transaction for a given teller.
 * Parameters:
 *** Queue *q: Pointer to the queue of transactions.
 *** Stack *s: Pointer to the stack of completed transactions.
 *** TellerStatus *tellerStatus: Pointer to the teller's status.
 *** int *totalTimeElapsed: Pointer to the total time elapsed.
 */
void processTransaction(Queue *q, Stack *s, TellerStatus *tellerStatus, int *totalTimeElapsed) {
    if (!tellerStatus->isBusy && !isQueueEmpty(q)) {
        tellerStatus->currentTransaction = dequeue(q);
        tellerStatus->remainingTime = tellerStatus->currentTransaction.duration;
        tellerStatus->isBusy = 1;
    }

    if (tellerStatus->isBusy) {
        tellerStatus->remainingTime--;
        *totalTimeElapsed += 1;
        if (tellerStatus->remainingTime == 0) {
            push(s, tellerStatus->currentTransaction);
            printf("Completed transaction stub %d, amount %d, account type %s, duration %d minutes\n",
                   tellerStatus->currentTransaction.stubNumber, tellerStatus->currentTransaction.amount,
                   accountTypeStr[tellerStatus->currentTransaction.accountType],
                   tellerStatus->currentTransaction.duration);
            tellerStatus->isBusy = 0;
        }
    }
}

/**
 * Function name: convertTime
 * Description: Convert total minutes into hours, minutes, and seconds format.
 * Parameters:
 *** int totalTimeElapsed: The total time elapsed in minutes.
 *** int *hours: Pointer to the hours component.
 *** int *minutes: Pointer to the minutes component.
 *** int *seconds: Pointer to the seconds component.
 */
void convertTime(int totalTimeElapsed, int *hours, int *minutes, int *seconds) {
    *hours = totalTimeElapsed / 60;
    *minutes = totalTimeElapsed % 60;
    *seconds = 0; // No need to compute seconds from minutes
}

/**
 * Function name: OpenNewQueue
 * Description: Activate the 5th queue if all regular queues (savings and checking) 
 *              are full and pending queue is at least 50% capacity.
 * Parameters:
 *** Queue *tellers: Array of teller queues.
 *** Queue *pendingQueue: Pointer to the pending queue.
 *** int pendingQueueCondition: The condition for the pending queue to trigger opening a new queue.
 */
void OpenNewQueue(Queue *tellers, Queue *pendingQueue, int pendingQueueCondition) {
    int allFull = 1;
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

/**
 * Function name: ConsolidateTransactions
 * Description: Consolidate transactions from all stacks into a single stack and display them.
 * Parameters:
 *** Stack *completedTransactions: Array of completed transaction stacks for each teller.
 *** int numTellers: The number of tellers.
 */
void ConsolidateTransactions(Stack *completedTransactions, int numTellers) {
    Stack consolidatedStack;
    initStack(&consolidatedStack);

    // Temporary array to store transactions
    Transaction *transactions = (Transaction *)malloc(MAX_TRANSACTIONS * sizeof(Transaction));
    int count = 0;
    int totalTransactions[NUM_TELLERS] = {0}; // Array to store total transactions for each teller

    for (int i = 0; i < numTellers; i++) {
        while (!isStackEmpty(&completedTransactions[i])) {
            Transaction transaction = pop(&completedTransactions[i]);
            transactions[count++] = transaction;
            totalTransactions[i]++; // Increment the transaction count for each teller
        }
    }

    // Sort transactions by stub number
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (transactions[i].stubNumber < transactions[j].stubNumber) {
                Transaction temp = transactions[i];
                transactions[i] = transactions[j];
                transactions[j] = temp;
            }
        }
    }

    // Push sorted transactions back into the stack
    for (int i = 0; i < count; i++) {
        push(&consolidatedStack, transactions[i]);
    }

    // Display sorted transactions
    printf("Consolidated transactions:\n");
    while (!isStackEmpty(&consolidatedStack)) {
        Transaction trans = pop(&consolidatedStack);
        printf("Transaction stub %d, amount %d, account type %s, duration %d minutes\n",
               trans.stubNumber, trans.amount, accountTypeStr[trans.accountType], trans.duration);
    }

    // Display total number of transactions for each teller
    printf("Total number of transactions completed by each teller:\n");
    for (int i = 0; i < numTellers; i++) {
        printf("Teller %d: Total Transactions %d\n", i + 1, totalTransactions[i]);
    }

    free(transactions);
}

int main() {
    srand(time(NULL));

    Queue tellers[NUM_TELLERS];
    Stack completedTransactions[NUM_TELLERS];
    TellerStatus tellerStatus[NUM_TELLERS] = {0};
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
        printf("|================================================[ BANK SIMULATOR ]================================================|");
        printf("\n|-[ 1 ]-[ Add Customer to Queue");
        printf("\n|-[ 2 ]-[ Consolidate and Display Transactions");
        printf("\n|-[ 3 ]-[ Exit");
        printf("\n|-[ ! ]-[ Time Elapsed: %02d:%02d:%02d", hours, minutes, seconds);
        printf("\n|-[ ? ]-[ Enter your choice (1/2/3): ");
        scanf("%d", &choice);
        // printf("  |==================================================================================================================|");

        switch (choice) {
            case 1: {
                Transaction transaction;
                transaction.stubNumber = stubNumber++; // Automatically assign a stub number
                printf("\n|==========================================[ Enter Transaction Details: ]==========================================|");
                printf("\n|-[ ? ]-[ Amount: ");
                scanf("%d", &transaction.amount);
                printf("|-[ ! ]-[ New = 0 | Government = 1 | Checking = 2 | Savings = 3");
                printf("\n|-[ ? ]-[ Account Type (0/1/2/3): ");
                scanf("%d", &transaction.accountType);
                transaction.duration = getRandomDuration(transaction.accountType);

                int tellerIndex;
                if (transaction.accountType == NEW || transaction.accountType == GOVERNMENT) {
                    tellerIndex = transaction.accountType; // New and Government accounts have dedicated queues
                } else if (transaction.accountType == CHECKING || transaction.accountType == SAVINGS) {
                    // Distribute evenly among the available tellers
                    tellerIndex = 2 + (transaction.accountType == CHECKING ? 0 : 1);
                } else {
                    printf("|-[ ! ]- [ Invalid account type. Transaction ignored.\n");
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
                            printf("|-[ ! ]- [ Extra queue is full. Cannot enqueue transaction.\n");
                        }
                    } else {
                        enqueue(&pendingQueue, transaction);
                        printf("|-[ ! ]- [ Transaction enqueued to pending queue.\n");
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
                printf("|-[ ! ]-[ Exiting...\n");
                exit(0);

            default:
                // Handle invalid menu choice
                printf("|-[ ! ]-[ Invalid choice. Try again.\n");
        }

        // Process transactions for each teller
        for (int i = 0; i < NUM_TELLERS; i++) {
            processTransaction(&tellers[i], &completedTransactions[i], &tellerStatus[i], &totalTimeElapsed);
        }

        // Print current transactions for each teller
        printf("\n|==================================================================================================================|\n");
        for (int i = 0; i < NUM_TELLERS; i++) {
            if (tellerStatus[i].isBusy) {
                printf("|-[ %d ]-[ Teller %d is processing transaction: Stub %d, Amount: %d, %s Account, %d Minutes Remaining...\n",
                       i + 1, i + 1, tellerStatus[i].currentTransaction.stubNumber, tellerStatus[i].currentTransaction.amount,
                       accountTypeStr[tellerStatus[i].currentTransaction.accountType], tellerStatus[i].remainingTime);
            } else {
                printf("|-[ %d ]-[ Teller %d is idle\n", i + 1, i + 1);
            }
        }

        // Print contents of each teller queue
        printf("|==================================================================================================================|\n");
        for (int i = 0; i < NUM_TELLERS; i++) {
            char queueName[20];
            snprintf(queueName, sizeof(queueName), "Teller %d", i + 1);
            printQueueContents(&tellers[i], queueName);
            printf("|\n");
        }
        printQueueContents(&pendingQueue, "Pending");
    }

    return 0;
}
