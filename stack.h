#ifndef STACK_H
#define STACK_H

typedef struct {
    int *data;
    int top;
    int capacity;
} Stack;

Stack* createStack(int capacity);
int isStackFull(Stack *stack);
int isStackEmpty(Stack *stack);
void push(Stack *stack, int item);
int pop(Stack *stack);
void destroyStack(Stack *stack);

#endif // STACK_H
