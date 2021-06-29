#include "../include/pcb.h"

/* 
 * =======================================================================
 * PCB: PROCESS CONTROL BLOCK
 * =======================================================================
 */

/**
 * @brief Inicializa um [PCB] a partir das informações do arquivo
 * correpondente ao [path].
 */
PCB *initializePCBFromFile(char *path)
{
    PCB *pcb;

    pcb = (PCB *)malloc(sizeof(PCB));
    pcb->program = parseFile(path);

    pcb->priority = pcb->program.size;

    return pcb;
}

/**
 * @brief Imprime [pcb] formatado para um humano ler.
 */
void printPCB(PCB *pcb)
{
    if (pcb)
        printf("PID: %4d | PPID: %4d | Prioridade: %4d | Valor: %4d | Início: %4d | Tempo CPU: %4d | PC: %4d\n",
               pcb->pid, pcb->ppid, pcb->priority, pcb->value, pcb->startTime, pcb->cpuTime, pcb->pc);
}

/**
 * @brief Destrói [pcb] liberando a memória alocada.
 */
void destroyPCB(PCB *pcb)
{
    if (pcb)
    {
        destroyProgram(&(pcb->program));
        free(pcb);
    }
}

/* 
 * =======================================================================
 * PCBList: PCB List or PCB Table
 * =======================================================================
 */

/**
 * @brief Inicializa uma [PCBList] vazia.
 */
void initializePCBList(PCBList *list)
{
    list->length = 0;
    list->expanded = 1;
    list->pcbs = (PCB **)malloc(sizeof(PCB *) * DEFAULT_LIST_LENGTH);
}

/**
 * @brief Insere um processo na tabela de processos na posição
 * correspondente ao seu próprio ID.
 * 
 * @details Incrementa [length] na estrutura de [PCBList]. Caso não haja
 * mais espaço no array de [list], a memória é realocada dinamicamente
 * para comportar mais [DEFAULT_LIST_LENGTH] itens.
 * 
 * @param list Tabela PCB de destino.
 * @param pcb Processo adicionado.
 * 
 * @return Caso a operação seja bem sucedida, retorna 0, caso contrário,
 * retorna -1.
 */
int insertPCB(PCBList *list, PCB *pcb)
{
    // PID inválido
    if (pcb->pid < 0)
        return -1;

    // Expande a lista caso a mesma não consiga armazenar [pcb]
    if (pcb->pid >= list->expanded * DEFAULT_LIST_LENGTH)
    {
        int total = ++(list->expanded) * DEFAULT_LIST_LENGTH;
        list->pcbs = (PCB **)realloc(list->pcbs, sizeof(PCB *) * total);
    }

    list->pcbs[pcb->pid] = pcb;
    (list->length)++;
    return 0;
}

/**
 * @brief Imprime [list] formatado para um humano ler.
 */
void printPCBList(PCBList *list)
{
    for (int i = 0; i < list->expanded * DEFAULT_LIST_LENGTH; i++)
        printPCB(list->pcbs[i]);
}

/**
 * @brief Destrói [list] liberando a memória alocada.
 */
void destroyPCBList(PCBList *list)
{
    // Itera sobre todos os índices da lista
    for (int i = 0; i < list->expanded * DEFAULT_LIST_LENGTH; i++)
        destroyPCB(list->pcbs[i]);

    free(list->pcbs);
}
