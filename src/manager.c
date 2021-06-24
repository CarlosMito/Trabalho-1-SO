#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../include/cpu.h"
#include "../include/linkedList.h"

#define NEXT_ID(a, b) ((a == -1) || (b == -1) ? ((b > a) ? b : a) : ((b > a) ? a : b))
#define LINE_LENGTH 99

typedef struct
{
    int executing;       /* Índice do processo executando na CPU                     */
    int time;            /* Tempo total do sistema                                   */
    CPU *cpu;            /* Abstração da CPU simulada                                */
    PCBList *list;       /* Tabela [PCB] com os processos não finalizados            */
    LinkedList *ready;   /* Lista com os índices dos processos prontos para executar */
    LinkedList *blocked; /* Lista com os índices dos processos bloqueados            */
    LinkedList *ids;     /* Armazena os IDs removidos para futura reatribuição       */
    int nextID;          /* Número ID do próximo processo                            */
} Manager;

void printFromList(PCBList *table, LinkedList *list)
{
    const int length = list->length;
    int test[length];

    linkedListToArray(list, test);

    for (int i = 0; i < length; i++)
        printPCB(table->pcbs[test[i]]);
}

// @brief tTes
// @param pid: PID do processo que irá assumir a CPU
void changeContext(Manager *manager, int pid)
{
    if (pid < 0)
    {
        manager->cpu->processTime = 0;
        manager->cpu->lastTime = 0;
        manager->cpu->pcb = NULL;
        return;
    }

    PCB *target = manager->list->pcbs[pid];

    manager->cpu->processTime = 0;
    manager->cpu->lastTime = target->cpuTime;
    manager->cpu->pcb = target;
}

// Função de Escalonamento: Retorna o ID do processo que será executado pela CPU.
int schedule(LinkedList *ready)
{
    // Basta arrumar os valores de prioridade ao salvar um nó salvo
    return pollLinkedList(ready);
}

void initializeManager(Manager *manager)
{
    PCB *init;

    manager->cpu = (CPU *)malloc(sizeof(CPU));
    manager->list = (PCBList *)malloc(sizeof(PCBList));
    manager->ready = (LinkedList *)malloc(sizeof(LinkedList));
    manager->blocked = (LinkedList *)malloc(sizeof(LinkedList));
    manager->ids = (LinkedList *)malloc(sizeof(LinkedList));

    manager->executing = 0;
    manager->time = 0;

    initializeCPU(manager->cpu);
    initializePCBList(manager->list);
    initializeLinkedList(manager->ready);
    initializeLinkedList(manager->blocked);
    initializeLinkedList(manager->ids);

    manager->nextID = 1;

    init = initializePCBFromFile("programs/init");

    insertPCB(manager->list, init);
    changeContext(manager, 0);
}

void destroyManager(Manager *manager)
{
    destroyPCBList(manager->list);
    destroyLinkedList(manager->ready);
    destroyLinkedList(manager->blocked);
    destroyLinkedList(manager->ids);

    free(manager->cpu);
    free(manager->list);
    free(manager->ready);
    free(manager->blocked);
    free(manager->ids);
}

void printRepeat(char character, int times, bool breakline)
{
    int i;

    for (i = 0; i < times; i++)
        printf("%c", character);

    if (breakline)
        printf("\n");
}

void printTitle(char *title, bool vspace)
{
    if (vspace)
        printf("\n");

    // O +1 serve para "arredondar para cima"
    int freeSpace = LINE_LENGTH - strlen(title) + 1;
    printRepeat(' ', freeSpace / 2, false);
    printf("%s\n", title);
}

void report(Manager *manager)
{
    pid_t cpid;

    if ((cpid = fork()) == -1)
    {
        perror("ERRO NO FORK");
        exit(EXIT_FAILURE);
    }

    /* PROCESSO FILHO */
    if (cpid == 0)
    {
        printRepeat('=', LINE_LENGTH, true);
        printTitle("ESTADO DO SISTEMA", false);
        printRepeat('=', LINE_LENGTH, true);

        printf("\nTEMPO ATUAL: %d\n", manager->time);

        printTitle("PROCESSO EXECUTANDO", true);
        printPCB(manager->cpu->pcb);
        printf("\n");

        printTitle("PROCESSOS PRONTOS", true);
        printFromList(manager->list, manager->ready);
        printf("\n");

        printTitle("PROCESSOS BLOQUEADOS", true);
        printFromList(manager->list, manager->blocked);
        printf("\n");

        // Apenas para debugar
        // printTitle("TODOS OS PROCESSOS", true);
        // printPCBList(manager->list);
        // printf("\n");

        printRepeat('=', LINE_LENGTH, true);
        exit(EXIT_SUCCESS);
    }
}

int arithmeticSeries(int n, int start, int end)
{
    return n * (start + end) / 2;
}

void clearIDList(PCBList *list, LinkedList *ids)
{
    int total, i;

    total = 0;

    for (i = 0; i < list->expanded * DEFAULT_LIST_LENGTH; i++)
        if (list->pcbs[i])
            total += list->pcbs[i]->pid;

    if (total == arithmeticSeries(list->length, 0, list->length - 1))
        clearLinkedList(ids);
}

void automaticTest(Manager *manager)
{
    int status;

    for (int i = 0; i < 13; i++)
    {
        status = executeCPU(manager->cpu, manager->list, manager->nextID, manager->time);

        if (status == FORK_PROCESS)
        {
            // Insere o novo processo na Lista de Prontos
            insertLinkedList(manager->ready, manager->nextID, manager->list->pcbs[manager->nextID]->priority);

            manager->nextID = NEXT_ID(manager->list->length, firstLinkedList(manager->ids));
        }

        else if (status == BLOCK_PROCESS)
        {
            // Insere o processo atual na Lista de Bloqueados
            insertLinkedList(manager->blocked, manager->cpu->pcb->pid, manager->cpu->pcb->priority);

            // Troca de contexto para o processo selecionado
            // pela função de escalonamento.
            manager->executing = schedule(manager->ready);
            changeContext(manager, manager->executing);
        }

        else if (status == TERMINATE_PROCESS)
        {
            // Adição na lista dos IDs que serão realocados
            insertLinkedList(manager->ids, manager->executing, manager->executing);

            // Desaloca a memória do processo finalizado
            destroyPCB(manager->list->pcbs[manager->executing]);

            manager->list->pcbs[manager->executing] = NULL;
            (manager->list->length)--;

            manager->nextID = NEXT_ID(manager->list->length, firstLinkedList(manager->ids));

            manager->executing = schedule(manager->ready);

            changeContext(manager, manager->executing);
        }
    }

    report(manager);
}

int main()
{
    Manager manager;
    char input;
    int status;
    int id;

    initializeManager(&manager);
    printf("Process Manager: Begin\n");

    // ======================================================================================================
    // PROCESSING COMMANDS
    // ======================================================================================================

    do
    {
        // Lê os comandos do descritor de leitura padrão
        // Equivale ao [pipefd[0]] devido ao comando [dup2] em [commander]
        scanf("%c", &input);
        input = toupper(input);

        switch (input)
        {
        case 'Q':
            // CPU vazia
            if (!manager.cpu->pcb)
            {
                // Nenhum processo pronto
                if (firstLinkedList(manager.ready) == -1)
                {
                    // Nenhum processo no sistema
                    if (firstLinkedList(manager.blocked) == -1)
                        printf("Não há nenhum processo no sistema!\n");
                    else
                        printf("Não há processos prontos no momento!\n");

                    break;
                }

                // Troca de contexto para o processo selecionado
                // pela função de escalonamento.
                manager.executing = schedule(manager.ready);
                printf("Colocando processo na CPU: (PID = %d)\n", manager.executing);
                changeContext(&manager, manager.executing);
            }

            // Executa a próxima instrução do processo na CPU
            status = executeCPU(manager.cpu, manager.list, manager.nextID, manager.time);
            (manager.time)++;

            switch (status)
            {
            case EXIT_SUCCESS:
                break;

            // Insere o ID da cópia instantânea na lista de processos prontos
            case FORK_PROCESS:
                insertLinkedList(manager.ready, manager.nextID, manager.list->pcbs[manager.nextID]->priority);
                manager.nextID = NEXT_ID(manager.list->length, firstLinkedList(manager.ids));
                clearIDList(manager.list, manager.ids);
                break;

            // Insere o processo atual na lista de processos bloqueados
            case BLOCK_PROCESS:
                insertLinkedList(manager.blocked, manager.cpu->pcb->pid, 0);
                manager.executing = schedule(manager.ready);
                changeContext(&manager, manager.executing);
                break;

            // Insere o processo atual na lista de processos bloqueados
            case TERMINATE_PROCESS:

                // Adição na lista dos IDs que serão realocados
                insertLinkedList(manager.ids, manager.executing, manager.executing);

                // Desaloca a memória do processo finalizado
                destroyPCB(manager.list->pcbs[manager.executing]);

                manager.list->pcbs[manager.executing] = NULL;
                (manager.list->length)--;

                manager.nextID = NEXT_ID(manager.list->length, firstLinkedList(manager.ids));

                manager.executing = schedule(manager.ready);
                changeContext(&manager, manager.executing);
                clearIDList(manager.list, manager.ids);
                break;

            case UNKNOWN_COMMAND:
                printf("Comando não reconhecido!\n");
                break;

            case INSTRUCTION_OUT_OF_RANGE:
                printf("Instrução fora de alcance!\n");
                break;
            }

            break;

        case 'U':
            // Altera o estado do primeiro processo na fila de bloqueados para pronto
            id = pollLinkedList(manager.blocked);

            if (id == -1)
            {
                printf("Não há processos bloqueados no momento!\n");
                continue;
            }

            insertLinkedList(manager.ready, id, manager.list->pcbs[id]->priority);

            break;

        case 'P':
            // Bifurca esse processo e executa [reporter]
            // TODO: Criar um processo novo para imprimir [reporter]
            report(&manager);
            wait(NULL);
            break;

        case 'T':
            // Bifurca, executa [reporter] e termina [manager] após [reporter] terminar
            report(&manager);
            wait(NULL);
            input = 'T';
            break;

        case 'R':
            destroyManager(&manager);
            initializeManager(&manager);
            printf("Sistema resetado!\n");
            break;

        default:
            break;
        }
    } while (input != 'T');

    destroyManager(&manager);
    printf("Process Manager: End\n");

    return 0;
}

/**
 * NEXT_ID()
 *  - Função Macro para decidir o próximo ID de um processo. Caso um dos valores seja -1,
 * retorna o maior entre [a] e [b], caso contrário, retorna o menor.
 */