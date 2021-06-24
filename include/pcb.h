#ifndef PCB_H
#define PCB_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "program.h"

#define DEFAULT_LIST_LENGTH 128

#define EXECUTING 0
#define READY 1
#define BLOCKED 2
#define TERMINATED 3

/* 
 * =======================================================================================
 * PCB: PROCESS CONTROL BLOCK
 * =======================================================================================
 * 
 * Estrutura de dados que armazena as informações críticas para gerenciar um processo.
 * Um processo é a abstração de um programa em execução.
 */
typedef struct
{
    int pid;         // Identificador do Processo
    int ppid;        // PID do Pai
    int pc;          // Contador de Programa
    int value;       /* Valor da variável do processo */
    int priority;    // Nível de prioridade
    int startTime;   // Tempo de início do processo
    int cpuTime;     // Tempo de CPU utilizado
    Program program; // Ponteiro para o programa
} PCB;

void printPCB(PCB *pcb);
void printPCBTable(PCB *table);
void printPReady();
void printPBlocked();
PCB emptyPCB();
PCB *initializePCBFromFile(char *path);
PCB *copyPCB(PCB *pcb);
void destroyPCB(PCB *pcb);

/* 
 * =======================================================================================
 * PCBList: PCB List or PCB Table
 * =======================================================================================
 * 
 * Tipo de dado abstrato que gerencia os PCBs como uma lista.
 */
typedef struct
{
    int length;
    int expanded;
    PCB **pcbs;
} PCBList;

PCBList emptyPCBList();
int isEmptyPCB(PCBList *list);
int insertPCB(PCBList *list, PCB *pcb);
PCB popPCB(PCBList *list);
PCB removePCB(PCBList *list, int index);
void initializePCBList(PCBList *list);
void destroyPCBList(PCBList *list);
void printPCBList(PCBList *list);

#endif