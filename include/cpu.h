#ifndef CPU_H
#define CPU_H

#include "pcb.h"

#define INSTRUCTION_EXECUTED 0
#define FORK_PROCESS 1
#define CHANGE_PROCESS 2
#define TERMINATE_PROCESS 3
#define BLOCK_PROCESS 4
#define UNKNOWN_COMMAND 5

/**
 * @brief Representa a CPU do simulador. É responsável por executar de
 * fato as instruções dos programas, modifica o valor da variável interna
 * de [pcb] e altera seu contador de programa [pc].
 */
typedef struct
{
    int lastTime;    /* Unidades de tempo utilizadas pelo processo anteriormente. */
    int processTime; /* Unidades de tempo utilizadas no processamento atual.      */
    PCB *pcb;        /* Ponteiro para o processo na CPU.                          */
} CPU;

void initializeCPU(CPU *cpu);
int executeCPU(CPU *cpu, PCBList *list, int nextID, int systemTime);
void printCPU(CPU *cpu);

#endif