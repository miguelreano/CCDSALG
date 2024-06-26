#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define constants for maximum sizes and the number of tellers
#define MAX_QUEUE_SIZE 40
#define MAX_STACK_SIZE 100
#define NUM_TELLERS 4

// Define account types
#define NEW 0
#define GOVERNMENT 1
#define CHECKING 2
#define SAVINGS 3

const char *accountTypeStr[] = { "New", "Government", "Checking", "Savings" };

// Define maximum customers per queue
#define MAX_NEW_QUEUE 3
#define MAX_GOV_QUEUE 4
#define MAX_CHECKING_QUEUE 5
#define MAX_SAVINGS_QUEUE 5

// Define a structure to hold transaction details
typedef struct {
    int amount;
    int accountType;
    int duration;
} Transaction;

// Define a Queue data structure
typedef struct {
    Transaction transactions[MAX_QUEUE_SIZE]; // Array to hold transactions
    int front; // Index of the front of the queue
    int rear;  // Index of the rear of the queue
    int size;  // Current size of the queue
} Queue;

// Define a Stack data structure
typedef struct {
    Transaction transactions[MAX_STACK_SIZE]; // Array to hold transactions
    int top; // Index of the top of the stack
} Stack;

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

// Initialize a stack
void initStack(Stack *s) {
    s->top = -1; // Set top to -1 to indicate the stack is initially empty
}

// Check if a stack is full
int isStackFull(Stack *s) {
    return s->top == MAX_STACK_SIZE - 1;
}

// Check if a stack is empty
int isStackEmpty(Stack *s) {
    return s->top == -1;
}

// Add a transaction to the stack
void push(Stack *s, Transaction transaction) {
    if (!isStackFull(s)) {
        s->transactions[++s->top] = transaction;
    } else {
        printf("Stack is full. Cannot push transaction %d\n", transaction.amount);
    }
}

// Remove a transaction from the stack
Transaction pop(Stack *s) {
    Transaction transaction = { -1, -1, -1 }; // Default invalid transaction
    if (!isStackEmpty(s)) {
        transaction = s->transactions[s->top--];
    }
    return transaction; // Return the popped transaction
}

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
        printf("Processed transaction amount %d, account type %s, duration %d minutes\n",
               trans.amount, accountTypeStr[trans.accountType], trans.duration);
    }
}

// Convert total minutes into hours, minutes, and seconds format
void convertTime(int totalTimeElapsed, int *hours, int *minutes, int *seconds) {
    *hours = totalTimeElapsed / 60;
    *minutes = totalTimeElapsed % 60;
    *seconds = (*minutes * 60) % 60;
}

// Consolidate all transactions from tellers' stacks and sort them
void consolidateTransactions(Stack stacks[], int numStacks) {
    Stack allTransactions;
    initStack(&allTransactions);

    // Collect all transactions from each stack into a single stack
    for (int i = 0; i < numStacks; i++) {
        while (!isStackEmpty(&stacks[i])) {
            push(&allTransactions, pop(&stacks[i]));
        }
    }

    // Sort transactions in ascending order by amount
    for (int i = 0; i < allTransactions.top; i++) {
        for (int j = i + 1; j <= allTransactions.top; j++) {
            if (allTransactions.transactions[i].amount > allTransactions.transactions[j].amount) {
                Transaction temp = allTransactions.transactions[i];
                allTransactions.transactions[i] = allTransactions.transactions[j];
                allTransactions.transactions[j] = temp;
            }
        }
    }

    // Display sorted transactions
    printf("Consolidated Transactions:\n");
    for (int i = 0; i <= allTransactions.top; i++) {
        printf("Amount: %d, Account Type: %s, Duration: %d minutes\n", allTransactions.transactions[i].amount, accountTypeStr[allTransactions.transactions[i].accountType], allTransactions.transactions[i].duration);
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

    Queue tellers[NUM_TELLERS]; // Array of queues for each teller
    Stack completedTransactions[NUM_TELLERS]; // Array of stacks for completed transactions
    Queue pendingQueue; // Queue for pending transactions

    // Initialize queues and stacks
    for (int i = 0; i < NUM_TELLERS; i++) {
        initQueue(&tellers[i]);
        initStack(&completedTransactions[i]);
    }
    initQueue(&pendingQueue);

    int choice;
    Transaction transaction;
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

                enqueue(&tellers[tellerIndex], transaction);
                processTransaction(&tellers[tellerIndex], &completedTransactions[tellerIndex], &totalTimeElapsed);
                break;

            case 2:
                // Consolidate and display all completed transactions
                consolidateTransactions(completedTransactions, NUM_TELLERS);
                break;

            case 3:
                // Exit the program
                printf("Exiting...\n");
                exit(0);

            default:
                // Handle invalid menu choice
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
