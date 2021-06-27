#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_INSTRUCTION_LENGTH 256
#define MAX_PATH_LENGTH 256
#define DEFAULT_PROGRAM_SIZE 256
#define DEFAULT_PATH "programs/"

/** 
 * @brief Representa uma instrução de um programa.
 */
typedef struct
{
    char command; /* Comando da instrução.        */
    int integer;  /* Argumento em número inteiro. */
    char *string; /* Argumento em string.         */
} Instruction;

/**
 * @brief Conjunto de instruções que formam um programa.
 */
typedef struct
{
    int size;                  /* Número total de instruções.      */
    Instruction *instructions; /* Array de instruções do programa. */
} Program;

Instruction initializeInstruction();
Program initializeProgram();
Instruction parseLine(char *line, int length);
Program parseFile(char *path);
void printInstruction(Instruction *instruction);
void printProgram(Program *program);
void destroyProgram(Program *program);

Instruction copyInstruction(Instruction *original);
Program copyProgram(Program *original);

#endif