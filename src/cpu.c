#include "../include/cpu.h"

/**
 * @brief Inicializa uma [CPU] vazia.
 */
void initializeCPU(CPU *cpu)
{
    cpu->lastTime = 0;
    cpu->processTime = 0;
    cpu->pcb = NULL;
}

/**
 * @brief Executa a próxima instrução do processo que está na CPU.
 * 
 * @details Sinais de retorno:
 * 
 *     [INSTRUCTION_EXECUTED]: Comando executado normalmente, nenhuma
 *     medida adicional é necessária.
 * 
 *     [FORK_PROCESS]: Processo bifurcado.
 *     [CHANGE_PROCESS]: Troca de imagem para outro processo.
 *     [BLOCK_PROCESS]: Processo bloqueado.
 *     [UNKNOWN_PROCESS]: Comando da instrução não reconhecido.
 * 
 * @param cpu Ponteiro para a CPU.
 * @param list Lista com os PCB's não finalizados.
 * @param nextID Número do próximo ID válido.
 * @param systemTime Tempo atual do sistema.
 * @return Dependendo do comando que foi processado e de seu resultado,
 * diferentes sinais de status são retornados.
 */
int executeCPU(CPU *cpu, PCBList *list, int nextID, int systemTime)
{
    PCB *forked;
    Instruction current;
    char path[MAX_PATH_LENGTH];

    if (cpu->pcb->pc < 0 || cpu->pcb->pc >= cpu->pcb->program.size)
        return TERMINATE_PROCESS;

    // Incrementa o tempo de uso da CPU após uma tentativa de execução de uma instrução.
    (cpu->processTime)++;
    (cpu->pcb->cpuTime)++;

    current = cpu->pcb->program.instructions[cpu->pcb->pc];

    printf("(PID = %d) Comando executado: %c ", cpu->pcb->pid, current.command);
    current.string ? printf("%s\n", current.string) : printf("%d\n", current.integer);

    switch (current.command)
    {
    case 'S':
        cpu->pcb->value = current.integer;
        break;

    case 'A':
        cpu->pcb->value += current.integer;
        break;

    case 'D':
        cpu->pcb->value -= current.integer;
        break;

    case 'F':
        // Bifurca o processo
        forked = (PCB *)malloc(sizeof(PCB));

        forked->pid = nextID;
        forked->startTime = systemTime;
        forked->ppid = cpu->pcb->pid;
        forked->cpuTime = 0;
        forked->pc = cpu->pcb->pc + 1;
        forked->priority = cpu->pcb->priority - 1;
        forked->value = 0;

        forked->program = copyProgram(&(cpu->pcb->program));

        cpu->pcb->pc += 1 + current.integer;
        cpu->pcb->priority = cpu->pcb->program.size - cpu->pcb->pc;

        list->pcbs[nextID] = forked;
        list->length++;

        return FORK_PROCESS;

    case 'R':
        // Troca de imagem
        strcpy(path, DEFAULT_PATH);
        strcat(path, current.string);

        // Destrói o programa anterior
        destroyProgram(&(cpu->pcb->program));

        cpu->pcb->program = parseFile(path);
        cpu->pcb->pc = 0;
        cpu->pcb->value = 0;
        cpu->pcb->priority = cpu->pcb->program.size;

        return CHANGE_PROCESS;

    case 'E':
        return TERMINATE_PROCESS;

    case 'B':
        cpu->pcb->pc++;
        cpu->pcb->priority = cpu->pcb->program.size - cpu->pcb->pc;
        return BLOCK_PROCESS;

    default:
        return UNKNOWN_COMMAND;
    }

    // Incrementa o Contador de Programa e diminui a Prioridade após uma instrução
    cpu->pcb->pc++;
    cpu->pcb->priority = cpu->pcb->program.size - cpu->pcb->pc;

    return INSTRUCTION_EXECUTED;
}

/**
 * @brief Imprime [cpu] formatado para um humano ler.
 */
void printCPU(CPU *cpu)
{
    if (!cpu->pcb)
    {
        printf("CPU vazia!\n");
        return;
    }

    printf("PID: %4d | Tempo Anterior: %4d | Tempo Atual: %4d | Valor: %4d | PC: %4d\n",
           cpu->pcb->pid, cpu->lastTime, cpu->processTime, cpu->pcb->value, cpu->pcb->pc);

    printProgram(&(cpu->pcb->program));
}