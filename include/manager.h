#ifndef MANAGER_H
#define MANAGER_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cpu.h"
#include "linkedList.h"

/**
 * @brief Função Macro para decidir o próximo ID de um processo. Caso um
 * dos valores seja -1, retorna o maior entre [a] e [b], caso contrário,
 * retorna o menor.
 */
#define NEXT_ID(a, b) ((a == -1) || (b == -1) ? ((b > a) ? b : a) : ((b > a) ? a : b))

#define LINE_LENGTH 99

/**
 * @brief Representa o Process Manager do trabalho de Sistemas
 * Operacionais. Ele é responsável por gerenciar os processos ainda não
 * finalizados.
 */
typedef struct
{
    int executing;       /* Índice do processo executando na CPU.                     */
    int time;            /* Tempo total do sistema.                                   */
    CPU *cpu;            /* Abstração da CPU simulada.                                */
    PCBList *list;       /* Tabela [PCB] com os processos não finalizados.            */
    LinkedList *ready;   /* Lista com os índices dos processos prontos para executar. */
    LinkedList *blocked; /* Lista com os índices dos processos bloqueados.            */
    LinkedList *ids;     /* Armazena os IDs removidos para futura reatribuição.       */
    int nextID;          /* Número ID do próximo processo.                            */
} Manager;

void initializeManager(Manager *manager);
void changeContext(Manager *manager, int pid);
int schedule(Manager *manager);
void adjustPPID(PCBList *pcbList, LinkedList *list, int terminated);
int arithmeticSeries(int n, int start, int end);
void clearIDList(PCBList *list, LinkedList *ids);
void processCommands(Manager *manager);
void printRepeat(char character, int times, bool breakline);
void printTitle(char *title, bool vspace);
void printFromList(PCBList *table, LinkedList *list);
void printHelp();
void printWrap(char *longText, int lineLength);
void printWelcome();
void report(Manager *manager);
void destroyManager(Manager *manager);

void linkedListTest();
void debug(Manager *manager);

#endif