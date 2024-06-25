#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack* createStack(int capacity) {
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->data = (int *)malloc(stack->capacity * sizeof(int));
    return stack;
}

int isStackFull(Stack *stack) {
    return (stack->top == stack->capacity - 1);
}

int isStackEmpty(Stack *stack) {
    return (stack->top == -1);
}

void push(Stack *stack, int item) {
    if (isStackFull(stack))
        return;
    stack->data[++stack->top] = item;
}

int pop(Stack *stack) {
    if (isStackEmpty(stack))
        return -1;
    return stack->data[stack->top--];
}

void destroyStack(Stack *stack) {
    free(stack->data);
    free(stack);
}
