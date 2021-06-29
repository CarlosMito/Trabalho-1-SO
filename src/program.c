#include "../include/program.h"

/**
 * @brief Inicializa uma [Instruction] vazia.
 */
Instruction initializeInstruction()
{
    Instruction instruction;

    instruction.command = '\0';
    instruction.integer = 0;
    instruction.string = NULL;

    return instruction;
}

/**
 * @brief Inicializa um [Program] vazio.
 */
Program initializeProgram()
{
    Program program;

    program.size = 0;
    program.instructions = NULL;

    return program;
}

/**
 * @brief Processa uma linha de comando de um programa, e armazena suas
 * propriedades na estrutura de dados [Instruction].
 * 
 * @details As linhas de comandos dos programas simulados são escritas no
 * seguinte formato:
 * 
 * <comando> <parâmetro do comando>
 * 
 * Sendo que <parâmetro do comando> pode ser uma [string] ou um [int]
 * dependendo de <comando>.
 * 
 * @param line Linha de comando de um programa.
 * @param length Quantidade de caracteres em [line].
 * @return Uma [Instruction] contendo as informações retiradas da linha
 * de comando.
 */
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

/**
 * @brief Processa um arquivo contendo um conjunto de linhas de comandos,
 * e armazena suas informações na estrutura de dados [Program].
 * 
 * @details Caso não seja possível abrir o arquivo especificado, o
 * programa termina com status [EXIT_FAILURE]. A quebra de linha determina
 * o fim de cada instrução. A estrutura [Program] armazena as instruções
 * em blocos de tamanho [DEFAULT_PROGRAM_SIZE].
 * 
 * @param path Caminho (relativo à pasta "programs") do arquivo com o
 * programa escrito.
 * 
 * @return Um [Program] contendo as informações retiradas do arquivo
 * referente ao [path].
 */
Program parseFile(char *path)
{
    FILE *file;
    Program program;
    Instruction *instructions;

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
                instructions = (Instruction *)realloc(
                    instructions,
                    sizeof(Instruction) * lastLimit);
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

/**
 * @brief Retorna uma [Instruction] que é cópia de [original].
 */
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

/**
 * @brief Retorna um [Program] que é cópia de [original].
 */
Program copyProgram(Program *original)
{
    Program copy;

    copy.size = original->size;
    copy.instructions = (Instruction *)malloc(sizeof(Instruction) * copy.size);

    for (int i = 0; i < original->size; i++)
        copy.instructions[i] = copyInstruction(&(original->instructions[i]));

    return copy;
}

/**
 * @brief Imprime [instruction] formatada para um humano ler.
 */
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

/**
 * @brief Imprime [program] formatado para um humano ler.
 */
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

/**
 * @brief Destrói [program] liberando a memória alocada para as instruções.
 */
void destroyProgram(Program *program)
{
    for (int i = 0; i < program->size; i++)
        free(program->instructions[i].string);

    free(program->instructions);
}