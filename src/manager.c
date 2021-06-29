#include "../include/manager.h"

int main()
{
    Manager manager;

    initializeManager(&manager);
    printf("Process Manager: Begin\n");

    processCommands(&manager);

    // linkedListTest();

    destroyManager(&manager);
    printf("Process Manager: End\n");

    return 0;
}

void printFromList(PCBList *table, LinkedList *list)
{
    const int length = list->length;
    int array[length];

    linkedListToArray(list, array);

    for (int i = 0; i < length; i++)
        printPCB(table->pcbs[array[i]]);
}

void debug(Manager *manager)
{
    printf("READY: ");
    printLinkedList(manager->ready);
    printf("BLOCKED: ");
    printLinkedList(manager->blocked);
    printf("IDS: ");
    printLinkedList(manager->ids);

    printf("NEXT ID: %d", manager->nextID);

    printf("\n");
}

// @brief tTes
// @param pid: PID do processo que irá assumir a CPU
void changeContext(Manager *manager, int pid)
{
    PCB *target;

    if (pid < 0)
        return;

    printf("Colocando processo na CPU: (PID = %d)\n", pid);

    manager->executing = pid;
    target = manager->list->pcbs[pid];

    manager->cpu->processTime = 0;
    manager->cpu->lastTime = target->cpuTime;
    manager->cpu->pcb = target;
}

// Função de Escalonamento: Retorna o ID do processo que será executado pela CPU.
int schedule(Manager *manager)
{
    int idSRT, cpuPriority, readyPriority;

    // Caso a CPU esteja vazia, retorna o ID do primeiro processo pronto
    if (!manager->cpu->pcb)
        return pollLinkedList(manager->ready);

    idSRT = firstLinkedList(manager->ready);

    // Caso a lista de processos prontos não esteja vazia, realiza um escalonamento
    // preemptivo com base no tempo de execução restante dos processos
    if (idSRT != -1)
    {
        cpuPriority = manager->cpu->pcb->priority;
        readyPriority = manager->list->pcbs[idSRT]->priority;

        if (readyPriority < cpuPriority)
        {
            insertLinkedList(manager->ready, manager->cpu->pcb->pid, manager->cpu->pcb->priority);
            return pollLinkedList(manager->ready);
        }
    }

    return -1;
}

void linkedListTest()
{
    LinkedList test;
    int i;

    initializeLinkedList(&test);

    for (i = 0; i < 100; i++)
        insertLinkedList(&test, i, i);

    printLinkedList(&test);

    destroyLinkedList(&test);
}

void initializeManager(Manager *manager)
{
    PCB *init;

    manager->cpu = (CPU *)malloc(sizeof(CPU));
    manager->list = (PCBList *)malloc(sizeof(PCBList));
    manager->ready = (LinkedList *)malloc(sizeof(LinkedList));
    manager->blocked = (LinkedList *)malloc(sizeof(LinkedList));
    manager->ids = (LinkedList *)malloc(sizeof(LinkedList));

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
        printf("Reporter: Begin\n");
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
        printf("Reporter: End\n");
        exit(EXIT_SUCCESS);
    }
}

void adjustPPID(PCBList *pcbList, LinkedList *list, int terminated)
{
    Node *header;

    header = list->first;

    while (header)
    {
        if (pcbList->pcbs[header->id]->ppid == terminated)
            pcbList->pcbs[header->id]->ppid = 0;

        header = header->next;
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

void processCommands(Manager *manager)
{
    char input;
    int status;
    int auxiliarID;

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
            if (!manager->cpu->pcb)
            {
                // Nenhum processo pronto
                if (firstLinkedList(manager->ready) == -1)
                {
                    // Nenhum processo no sistema
                    if (firstLinkedList(manager->blocked) == -1)
                        printf("Não há nenhum processo no sistema!\n");
                    // Há somente processos bloqueados
                    else
                        printf("Não há processos prontos no momento!\n");

                    continue;
                }

                // Troca de contexto para o processo selecionado pela função de escalonamento
                auxiliarID = schedule(manager);
                changeContext(manager, auxiliarID);
            }

            // Executa a próxima instrução do processo na CPU
            status = executeCPU(manager->cpu, manager->list, manager->nextID, manager->time);

            (manager->time)++;

            switch (status)
            {
            case EXIT_SUCCESS:
                break;

            // Insere o ID da cópia instantânea na lista de processos prontos
            case FORK_PROCESS:
                // Houve uma realocação de PIDs cujos processos já foram terminados
                if (manager->nextID != manager->list->length)
                    pollLinkedList(manager->ids);

                insertLinkedList(manager->ready, manager->nextID, manager->list->pcbs[manager->nextID]->priority);
                manager->nextID = NEXT_ID(manager->list->length, firstLinkedList(manager->ids));
                clearIDList(manager->list, manager->ids);
                break;

            // Insere o processo atual na lista de processos bloqueados
            case BLOCK_PROCESS:
                insertLinkedList(manager->blocked, manager->cpu->pcb->pid, 0);
                initializeCPU(manager->cpu);
                break;

            // Insere o processo atual na lista de processos bloqueados
            case TERMINATE_PROCESS:
                // Caso o ID do processo finalizado seja diferente de 0,
                if (manager->executing)
                {
                    // O PID é adicionado na lista dos IDs que serão realocados
                    insertLinkedList(manager->ids, manager->executing, manager->executing);

                    // O [ppid] dos processos que eram filhos do processo que acabou de terminar
                    // é alterado para o PID do processo [init], que é 0.
                    adjustPPID(manager->list, manager->ready, manager->executing);
                    adjustPPID(manager->list, manager->blocked, manager->executing);

                    // Desaloca a memória do processo finalizado
                    destroyPCB(manager->list->pcbs[manager->executing]);
                    manager->list->pcbs[manager->executing] = NULL;
                    (manager->list->length)--;
                }

                initializeCPU(manager->cpu);

                manager->nextID = NEXT_ID(manager->list->length, firstLinkedList(manager->ids));

                clearIDList(manager->list, manager->ids);
                break;

            case UNKNOWN_COMMAND:
                printf("Comando não reconhecido!\n");
                break;

            case INSTRUCTION_OUT_OF_RANGE:
                printf("Instrução fora de alcance!\n");
                break;
            }

            auxiliarID = schedule(manager);
            changeContext(manager, auxiliarID);

            // debug(manager);

            break;

        case 'U':
            // Altera o estado do primeiro processo na fila de bloqueados para pronto
            auxiliarID = pollLinkedList(manager->blocked);

            if (auxiliarID == -1)
            {
                printf("Não há processos bloqueados no momento!\n");
                continue;
            }

            insertLinkedList(manager->ready, auxiliarID, manager->list->pcbs[auxiliarID]->priority);

            break;

        case 'P':
            // Bifurca esse processo e executa [reporter]
            report(manager);
            wait(NULL);
            break;

        case 'T':
            // Bifurca, executa [reporter] e termina [manager] após [reporter] terminar
            report(manager);
            wait(NULL);
            input = 'T';
            break;

        default:
            break;
        }
    } while (input != 'T');
}
