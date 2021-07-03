#include "../include/linkedList.h"

/**
 * @brief Inicializa uma [LinkedList] vazia.
 */
void initializeLinkedList(LinkedList *list)
{
    list->length = 0;
    list->first = NULL;
    list->last = NULL;
}

/**
 * @brief Insere um novo [Node] (com ID correspondente à [id] e valor
 * equivalente à [value]) em [list]. A posição que o mesmo ocupará na
 * lista depende de [value].
 * 
 * @param list Ponteiro para a lista encadeada alvo.
 * @param id Identificador do nó.
 * @param value Prioridade do nó na lista.
 */
void insertLinkedList(LinkedList *list, int id, int value)
{
    Node *header, *lastHeader, *node;

    node = (Node *)malloc(sizeof(Node));
    node->value = value;
    node->id = id;
    node->next = NULL;

    /* Caso a lista esteja vazia */
    if (!list->length)
    {
        list->first = node;
        list->last = node;
        list->length = 1;
        return;
    }

    header = list->first;
    lastHeader = header;

    while (header)
    {
        /* [node] é inserido antes de [header] */
        if (header->value > node->value)
        {
            node->next = header;
            list->length++;

            /* [node] é inserido no começo da lista */
            if (header == list->first)
            {
                list->first = node;
                return;
            }

            /* [node] é inserido entre 2 elementos */
            lastHeader->next = node;
            return;
        }

        lastHeader = header;
        header = header->next;
    }

    /* [node] é inserido na última posição da lista */
    list->last->next = node;
    list->last = node;
    list->length++;
}

/**
 * @brief Insere um novo [Node] (com ID correspondente à [id] e valor
 * equivalente à [value]) na última posição de [list].
 * 
 * @param list Ponteiro para a lista encadeada alvo.
 * @param id Identificador do nó.
 * @param value Prioridade do nó na lista.
 */
void appendLinkedList(LinkedList *list, int id, int value)
{
    Node *header, *node;

    node = (Node *)malloc(sizeof(Node));
    node->value = value;
    node->id = id;
    node->next = NULL;

    header = list->last;
    list->last = node;

    /* A lista está vazia */
    if (!header)
    {
        list->first = node;
        list->length = 1;
        return;
    }

    header->next = node;
    list->length++;
}

/**
 * @brief Remove o primeiro item de [list].
 * 
 * @param list Ponteiro para a lista encadeada alvo.
 * @return O [id] do item removido. Caso [list] esteja vazia, retorna -1.
 */
int pollLinkedList(LinkedList *list)
{
    Node *auxiliar;
    int id;

    /* Caso a lista esteja vazia */
    if (!list->length)
        return -1;

    /* Caso a lista tenha apenas 1 elemento */
    if (list->length == 1)
    {
        id = list->first->id;

        free(list->first);

        list->first = NULL;
        list->last = NULL;
        list->length = 0;

        return id;
    }

    /* A lista possui 2 ou mais elementos */
    id = list->first->id;
    auxiliar = list->first;
    list->first = list->first->next;

    free(auxiliar);

    list->length--;

    return id;
}

/**
 * @brief Esvazia [list] liberando a memória alocada por seus nós e
 * resetando os atributos da mesma.
 * 
 * @param list Ponteiro para a lista encadeada alvo.
 */
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

/**
 * @brief Verifica o [íd] do primeiro item de [list].
 * 
 * @param list Ponteiro para a lista encadeada alvo.
 * @return O [id] do primeiro item de [list]. Caso [list] esteja vazia,
 * retorna -1.
 */
int firstLinkedList(LinkedList *list)
{
    /* Caso a lista esteja vazia */
    if (!list->length)
        return -1;

    return list->first->id;
}

/**
 * @brief Cria uma array de inteiros contendo os [id]'s armazenados em
 * uma [LinkedList]. [array] deve ser tão extensa quanto [list] para
 * armazenar todos os seus [id]'s.
 * 
 * @param list Ponteiro para a lista encadeada alvo.
 * @param array Array que armazenará as [id]'s de [list].
 * @return O próprio parâmtro [array] contendo todos os [id]'s de [list].
 */
int *linkedListToArray(LinkedList *list, int *array)
{
    Node *current;
    int i = 0;

    current = list->first;

    while (current)
    {
        array[i++] = current->id;
        current = current->next;
    }

    return array;
}

/**
 * @brief Imprime [list] formatada para um humano ler.
 */
void printLinkedList(LinkedList *list)
{
    Node *header;

    /* Caso a lista esteja vazia */
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
           list->length,
           list->first->id, list->first->value,
           list->last->id, list->last->value);
}

/**
 * @brief Destrói [list] liberando a memória de seus nós.
 */
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
