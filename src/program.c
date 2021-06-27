#include "../include/program.h"

/* 
 * =======================================================================================
 * INSTRUÇÃO
 * =======================================================================================
 */

Instruction initializeInstruction()
{
    Instruction instruction;

    instruction.command = '\0';
    instruction.integer = 0;
    instruction.string = NULL;

    return instruction;
}

void printInstruction(Instruction *instruction)
{
    if (instruction->command == '\0')
    {
        printf("Vazio\n");
        return;
    }

    printf("%c ", instruction->command);

    if (instruction->string != NULL)
        printf("%s\n", instruction->string);
    else
        printf("%d\n", instruction->integer);
}

/* 
 * =======================================================================================
 * PROGRAMA
 * =======================================================================================
 */

/**
 * @brief Inicialize um [Program] vazio.
 */
Program initializeProgram()
{
    Program program;

    program.size = 0;
    program.instructions = NULL;

    return program;
}

void destroyProgram(Program *program)
{
    for (int i = 0; i < program->size; i++)
        free(program->instructions[i].string);

    free(program->instructions);
}

void printProgram(Program *program)
{
    printf("Número de Instruções: %d\n", program->size);
    printf("Instruções:\n");
    for (int i = 0; i < program->size; i++)
    {
        printf("  %4d - ", i);
        printInstruction(&(program->instructions[i]));
    }
}

Instruction parseLine(char *line, int length)
{
    Instruction instruction;

    instruction = initializeInstruction();

    if (line[0] == '\0')
        return instruction;

    instruction.command = toupper(line[0]);

    if (line[1] == '\0')
        return instruction;

    char *argument = &(line[2]);

    if (instruction.command == 'R')
    {
        instruction.string = (char *)malloc(sizeof(char) * (length - 2));
        strcpy(instruction.string, argument);
    }
    else
    {
        instruction.integer = atoi(argument);
    }

    return instruction;
}

Instruction copyInstruction(Instruction *original)
{
    Instruction copy;

    copy.command = original->command;
    copy.integer = original->integer;
    copy.string = NULL;

    // Caso não seja nula
    if (original->string)
    {
        copy.string = (char *)malloc(sizeof(char) * strlen(original->string));
        strcpy(copy.string, original->string);
    }

    return copy;
}

Program copyProgram(Program *original)
{
    Program copy;

    copy.size = original->size;
    copy.instructions = (Instruction *)malloc(sizeof(Instruction) * copy.size);

    for (int i = 0; i < original->size; i++)
        copy.instructions[i] = copyInstruction(&(original->instructions[i]));

    return copy;
}

Program parseFile(char *path)
{
    FILE *file;
    Program program;
    Instruction *instructions;

    int allocated;
    int lastLimit;
    int total;
    char character;
    char line[MAX_INSTRUCTION_LENGTH];
    int i;

    file = fopen(path, "r");

    if (file == NULL)
    {
        printf("PATH: %s\n", path);
        perror("ERRO AO ABRIR O ARQUIVO");
        exit(EXIT_FAILURE);
    }

    instructions = (Instruction *)malloc(sizeof(Instruction) * DEFAULT_PROGRAM_SIZE);

    lastLimit = DEFAULT_PROGRAM_SIZE;
    total = 0;
    i = 0;

    while ((character = fgetc(file)) != EOF)
    {
        if (character == '\n')
        {
            if (line[0] == '\0')
                continue;

            line[i] = '\0';
            instructions[total] = parseLine(line, i);
            total++;
            line[0] = '\0';
            i = 0;

            if (total >= lastLimit)
            {
                lastLimit = DEFAULT_PROGRAM_SIZE + total;
                instructions = (Instruction *)realloc(instructions, sizeof(Instruction) * lastLimit);
            }

            continue;
        }
        line[i] = character;

        if (i < MAX_INSTRUCTION_LENGTH - 1)
            i++;
    }

    if (line[0] != '\0')
    {
        line[i] = '\0';
        instructions[total] = parseLine(line, i);
        total++;
        line[0] = '\0';
    }

    program = initializeProgram();
    program.size = total;
    program.instructions = instructions;

    return program;
}
