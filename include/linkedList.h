#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct node Node;

// TODO: Adicionar ID, dessa forma será possível ordenar por prioridade e manter o ID dos processos
struct node
{
    int value;
    int id;
    Node *next;
};

typedef struct
{
    int length;
    Node *first;
    Node *last;
} LinkedList;

void initializeLinkedList(LinkedList *list);
void printLinkedList(LinkedList *list);
int insertLinkedList(LinkedList *list, int id, int value);
void destroyLinkedList(LinkedList *list);
int pollLinkedList(LinkedList *list);
int firstLinkedList(LinkedList *list);
int *linkedListToArray(LinkedList *list, int *array);
void clearLinkedList(LinkedList *list);

#endif
