#include "../include/linkedList.h"

void initializeLinkedList(LinkedList *list)
{
    list->length = 0;
    list->first = NULL;
    list->last = NULL;
}

// Retorna o [id] do primeiro item na lista ordenada crescentemente por [value].
// Caso a lista esteja vazia, retorna -1.
int pollLinkedList(LinkedList *list)
{
    Node *auxiliar;
    int id;

    // Caso a lista esteja vazia
    if (!list->length)
        return -1;

    // Caso a lista tenha apenas 1 elemento
    if (list->length == 1)
    {
        id = list->first->id;

        free(list->first);

        list->first = NULL;
        list->last = NULL;
        list->length = 0;

        return id;
    }

    // A lista possui 2 ou mais elementos
    id = list->first->id;
    auxiliar = list->first;
    list->first = list->first->next;

    free(auxiliar);

    list->length--;

    return id;
}

// Apenas retorna o ID do primeiro item da lista, não remove nenhum elemento.
// Caso a lista esteja vazia, retorna -1.
int firstLinkedList(LinkedList *list)
{
    // Caso a lista esteja vazia
    if (!list->length)
        return -1;

    return list->first->id;
}

int insertLinkedList(LinkedList *list, int id, int value)
{
    Node *header, *lastHeader, *node;

    node = (Node *)malloc(sizeof(Node));
    node->value = value;
    node->id = id;
    node->next = NULL;

    // Caso a lista esteja vazia
    if (!list->length)
    {
        list->first = node;
        list->last = node;
        list->length = 1;
        return 0;
    }

    header = list->first;
    lastHeader = header;

    while (header)
    {
        // [node] é inserido antes de [header]
        if (header->value > node->value)
        {
            node->next = header;
            list->length++;

            // [node] é inserido no começo da lista
            if (header == list->first)
            {
                list->first = node;
                return 0;
            }

            // [node] é inserido entre 2 elementos
            lastHeader->next = node;
            return 0;
        }

        lastHeader = header;
        header = header->next;
    }

    // [node] é inserido na última posição da lista
    list->last->next = node;
    list->last = node;
    list->length++;
    return 0;
}

void printLinkedList(LinkedList *list)
{
    Node *header;

    // Caso a lista esteja vazia
    if (!list->length)
        return;

    header = list->first;

    printf("(%d: %d)", header->id, header->value);
    header = header->next;

    while (header)
    {
        printf(" -> (%d: %d)", header->id, header->value);
        header = header->next;
    }

    printf("\nLength: %d | First: (%d: %d) and Last: (%d: %d)\n",
           list->length, list->first->id, list->first->value, list->last->id, list->last->value);
}

void clearLinkedList(LinkedList *list)
{
    Node *auxiliar;

    if (list->first)
    {
        auxiliar = list->first;
        list->first = list->first->next;

        clearLinkedList(list);

        free(auxiliar);
        return;
    }

    list->last = NULL;
    list->length = 0;
}

void destroyLinkedList(LinkedList *list)
{
    Node *auxiliar;

    if (list->first)
    {
        auxiliar = list->first;
        list->first = list->first->next;
        free(auxiliar);
        destroyLinkedList(list);
    }
}

int *linkedListToArray(LinkedList *list, int *array)
{
    Node *current;
    int i;

    current = list->first;
    i = 0;

    while (current)
    {
        array[i++] = current->id;
        current = current->next;
    }

    return array;
}