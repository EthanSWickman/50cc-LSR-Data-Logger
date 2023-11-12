#ifndef LL_H
#define LL_H

typedef struct node{
    unsigned int val;
    struct node* next;
} node;

typedef struct ll{
    node* head;
    unsigned int len;
} ll;

node* buildNode(unsigned int val, node* next);
ll* buildll();
void appendNode(ll* ll, unsigned int val);
void printll(ll* ll);
void removeNode(ll* ll, unsigned int index);
void deletell(ll* ll);
void insertNode(ll* ll, unsigned int val, unsigned int index);

#endif