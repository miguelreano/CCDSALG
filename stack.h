#ifndef STACK_H
#define STACK_H

#include "transaction.h"

// Define constants for maximum sizes
#define MAX_STACK_SIZE 100

// Define a Stack data structure
typedef struct {
    Transaction transactions[MAX_STACK_SIZE]; // Array to hold transactions
    int top; // Index of the top of the stack
} Stack;

// Function declarations
void initStack(Stack *s);
int isStackFull(Stack *s);
int isStackEmpty(Stack *s);
void push(Stack *s, Transaction transaction);
Transaction pop(Stack *s);

#endif // STACK_H
