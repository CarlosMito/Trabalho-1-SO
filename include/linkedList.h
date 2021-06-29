#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

/* 
 * =======================================================================
 * ESTRUTURAS DE DADOS
 * =======================================================================
 */

typedef struct node Node;

/**
 * @brief Representa um item em uma lista encadeada de números inteiros.
 */
struct node
{
    int value;  /* Valor armazenado.                    */
    int id;     /* Número identificador.                */
    Node *next; /* Ponteiro para o próximo nó na lista. */
};

/**
 * @brief Representa uma lista encadeada de números inteiros ordenada
 * crescentemente com base no [value] de cada [Node].
 */
typedef struct
{
    int length;  /* Quantidade de nós na lista.           */
    Node *first; /* Ponteiro para o primeiro nó da lista. */
    Node *last;  /* Ponteiro para o último nó da lista.   */
} LinkedList;

/* 
 * =======================================================================
 * FUNÇÕES
 * =======================================================================
 */

void initializeLinkedList(LinkedList *list);
void insertLinkedList(LinkedList *list, int id, int value);
int pollLinkedList(LinkedList *list);
void clearLinkedList(LinkedList *list);
int firstLinkedList(LinkedList *list);
int *linkedListToArray(LinkedList *list, int *array);
void printLinkedList(LinkedList *list);
void destroyLinkedList(LinkedList *list);

#endif
