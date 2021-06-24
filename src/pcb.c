#include "../include/pcb.h"

void printPCB(PCB *pcb)
{
    if (pcb)
        printf("PID: %4d | PPID: %4d | Prioridade: %4d | Valor: %4d | Início: %4d | Tempo CPU: %4d | PC: %4d\n",
               pcb->pid, pcb->ppid, pcb->priority, pcb->value, pcb->startTime, pcb->cpuTime, pcb->pc);
}

PCB *copyPCB(PCB *pcb)
{
    PCB *copy;

    copy = (PCB *)malloc(sizeof(PCB));

    copy->pid = pcb->pid;
    copy->ppid = pcb->ppid;
    copy->pc = pcb->pc;
    copy->value = pcb->value;
    copy->priority = pcb->priority;
    copy->startTime = pcb->startTime;
    copy->cpuTime = pcb->cpuTime;
    copy->program = pcb->program;

    return copy;
}

void printPReady()
{
}

void printPBlocked()
{
}

// PCB emptyPCB()
// {
//     PCB pcb;

//     pcb.pid = 0;
//     pcb.ppid = 0;
//     pcb.pc = 0;
//     pcb.program = emptyProgram();
//     pcb.startTime = 0;
//     pcb.cpuTime = 0;
//     pcb.priority = 0;

//     return pcb;
// }

PCB *initializePCBFromFile(char *path)
{
    PCB *pcb;

    pcb = (PCB *)malloc(sizeof(PCB));
    pcb->program = parseFile(path);

    return pcb;
}

void destroyPCB(PCB *pcb)
{
    if (pcb)
        free(pcb);
}

/* 
 * =======================================================================================
 * PCBList: PCB List or PCB Table
 * =======================================================================================
 */

void initializePCBList(PCBList *list)
{
    list->length = 0;
    list->expanded = 1;
    list->pcbs = (PCB **)malloc(sizeof(PCB *) * DEFAULT_LIST_LENGTH);
}

/**
 * @brief Insere um processo na tabela de processos na posição correspondente ao seu próprio ID.
 * 
 * @details Incrementa [length] na estrutura de [PCBList]. Caso não haja mais espaço no
 * array de [list], a memória é realocada dinamicamente para comportar mais
 * [DEFAULT_LIST_LENGTH] itens.
 * 
 * @param list Tabela PCB de destino.
 * @param pcb Processo adicionado.
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

// PCB removePCB(PCBList *list, int index)
// {
//     list->pcbs[index].status = TERMINATED;
// }

PCB popPCB(PCBList *list)
{
}

void printPCBList(PCBList *list)
{
    for (int i = 0; i < list->expanded * DEFAULT_LIST_LENGTH; i++)
        printPCB(list->pcbs[i]);
}

void destroyPCBList(PCBList *list)
{
    // Itera sobre todos os índices da lista
    for (int i = 0; i < list->expanded * DEFAULT_LIST_LENGTH; i++)
        destroyPCB(list->pcbs[i]);

    free(list->pcbs);
}
