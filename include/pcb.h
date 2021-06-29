#ifndef PCB_H
#define PCB_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "program.h"

#define DEFAULT_LIST_LENGTH 128

/* 
 * =======================================================================
 * PCB: PROCESS CONTROL BLOCK
 * =======================================================================
 */

/**
 * @brief Estrutura de dados que armazena as informações críticas para
 * gerenciar um processo. Um processo é uma abstração de um programa em
 * execução.
 */
typedef struct
{
    int pid;         /* Identificador do Processo.     */
    int ppid;        /* PID do processo pai.           */
    int pc;          /* Contador de Programa.          */
    int value;       /* Valor da variável do processo. */
    int priority;    /* Nível de prioridade.           */
    int startTime;   /* Tempo de início do processo.   */
    int cpuTime;     /* Tempo de CPU utilizado.        */
    Program program; /* Ponteiro para o programa.      */
} PCB;

PCB *initializePCBFromFile(char *path);
void printPCB(PCB *pcb);
void destroyPCB(PCB *pcb);

/* 
 * =======================================================================
 * PCBList: PCB List or PCB Table
 * =======================================================================
 */

/** 
 * @brief Tipo de dado abstrato que gerencia os PCBs como uma lista.
 */
typedef struct
{
    int length;   /* Tamanho da lista.                           */
    int expanded; /* Número que vezes que a lista foi expandida. */
    PCB **pcbs;   /* Array com os PCB's.                         */
} PCBList;

void initializePCBList(PCBList *list);
int insertPCB(PCBList *list, PCB *pcb);
void printPCBList(PCBList *list);
void destroyPCBList(PCBList *list);

#endif