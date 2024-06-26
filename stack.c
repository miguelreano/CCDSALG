#include "stack.h"
#include <stdio.h>

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
