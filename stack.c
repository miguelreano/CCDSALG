#include "stack.h"
#include <stdio.h>

/**
 * Function name: initStack
 * Description: Initialize a stack.
 * Parameters:
 *** Stack *s: Pointer to the stack to be initialized.
 */
void initStack(Stack *s) {
    s->top = -1;
}

/**
 * Function name: isStackFull
 * Description: Check if a stack is full.
 * Parameters:
 *** Stack *s: Pointer to the stack.
 * Return value:
 *** int: Returns 1 if the stack is full, otherwise returns 0.
 */
int isStackFull(Stack *s) {
    return s->top == MAX_STACK_SIZE - 1;
}

/**
 * Function name: isStackEmpty
 * Description: Check if a stack is empty.
 * Parameters:
 *** Stack *s: Pointer to the stack.
 * Return value:
 *** int: Returns 1 if the stack is empty, otherwise returns 0.
 */
int isStackEmpty(Stack *s) {
    return s->top == -1;
}

/**
 * Function name: push
 * Description: Add a transaction to the stack.
 * Parameters:
 *** Stack *s: Pointer to the stack.
 *** Transaction transaction: The transaction to be added.
 */
void push(Stack *s, Transaction transaction) {
    if (!isStackFull(s)) {
        s->transactions[++s->top] = transaction;
    } else {
        printf("|-[ ! ]- [ Stack is full. Cannot push transaction %d\n", transaction.amount);
    }
}

/**
 * Function name: pop
 * Description: Remove a transaction from the stack.
 * Parameters:
 *** Stack *s: Pointer to the stack.
 * Return value:
 *** Transaction: The popped transaction. Returns a default invalid transaction if the stack is empty.
 */
Transaction pop(Stack *s) {
    Transaction transaction = { -1, -1, -1 }; 
    if (!isStackEmpty(s)) {
        transaction = s->transactions[s->top--];
    }
    return transaction;
}
