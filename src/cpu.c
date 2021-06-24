#include "../include/cpu.h"

void initializeCPU(CPU *cpu)
{
    cpu->lastTime = 0;
    cpu->processTime = 0;
    cpu->pcb = NULL;
}

int executeCPU(CPU *cpu, PCBList *list, int nextID, int systemTime)
{
    PCB *forked;
    Instruction current;
    char path[MAX_PATH_LENGTH];

    if (cpu->pcb->pc < 0 || cpu->pcb->pc >= cpu->pcb->program.size)
        return INSTRUCTION_OUT_OF_RANGE;

    // Incrementa o tempo de uso da CPU após uma tentativa de execução de uma instrução.
    (cpu->processTime)++;
    (cpu->pcb->cpuTime)++;

    current = cpu->pcb->program.instructions[cpu->pcb->pc];

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
        forked = copyPCB(cpu->pcb);

        forked->pid = nextID;
        forked->startTime = systemTime + 1;
        forked->ppid = cpu->pcb->pid;
        forked->cpuTime = 0;
        forked->pc++;

        cpu->pcb->pc += 1 + current.integer;

        list->pcbs[nextID] = forked;
        list->length++;

        return FORK_PROCESS;

    case 'R':
        strcpy(path, DEFAULT_PATH);
        strcat(path, current.string);

        cpu->pcb->program = parseFile(path);
        cpu->pcb->pc = 0;
        cpu->pcb->value = 0;

        return CHANGE_PROCESS;

    case 'E':

        return TERMINATE_PROCESS;

    case 'B':
        // Bloqueia esse processo
        cpu->pcb->pc++;
        return BLOCK_PROCESS;

    default:
        return UNKNOWN_COMMAND;
    }

    // Incrementa o Contador de Programa e o Tempo de Sistema após uma instrução
    (cpu->pcb->pc)++;

    return EXIT_SUCCESS;
}

void printCPU(CPU *cpu)
{
    if (!cpu->pcb)
    {
        printf("CPU VAZIA!\n");
        return;
    }

    printf("PID: %4d | Tempo Anterior: %4d | Tempo Atual: %4d | Valor: %4d | PC: %4d\n",
           cpu->pcb->pid, cpu->lastTime, cpu->processTime, cpu->pcb->value, cpu->pcb->pc);

    printProgram(&(cpu->pcb->program));
}