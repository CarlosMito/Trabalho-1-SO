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

typedef struct
{
    char command; // Comando da instrução
    int integer;  // Argumento em número inteiro
    char *string; // Argumento em string
} Instruction;

typedef struct
{
    int size;                  // Número de instruções
    Instruction *instructions; // Array de instruções do programa
} Program;

// Instruction createInstruction(char command, int number, char *filename);
void printInstruction(Instruction *instruction);

Program initializeProgram(int totalInstructions);
Program emptyProgram();

void destroyProgram(Program *program);
void printProgram(Program *program);

Program parseFile(char *path);
Instruction parseLine(char *line, int length);

#endif