#include "../include/manager.h"

int main()
{
    Manager manager;

    printf("[Process Manager] Iniciado!\n");
    initializeManager(&manager);

    printWelcome();

    processCommands(&manager);

    destroyManager(&manager);
    printf("[Process Manager] Encerrado com sucesso!\n");

    return 0;
}

/**
 * @brief Inicializa o Manager do simulador.
 * 
 * @details Inicializa todas as estruturas de dados necessárias, configura
 * o tempo do sistema para 0, modifica o próximo ID para 1, inicia o
 * processo [init], e o coloca na CPU.
 */
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
    printf("[Process Manager] Estruturas de dados inicializadas!\n");

    manager->nextID = 1;
    init = initializePCBFromFile("programs/init");
    printf("[Process Manager] Primeiro processo simulado criado!\n\n");

    insertPCB(manager->list, init);
    changeContext(manager, 0);
}

/**
 * @brief Troca de Contexto do simulador. Retira um processo da CPU e
 * coloca outro no lugar.
 * 
 * @param manager Ponteiro para o Manager do simulador.
 * @param pid PID do processo que irá assumir a CPU.
 */
void changeContext(Manager *manager, int pid)
{
    PCB *target;

    if (pid < 0)
        return;

    printf("(PID = %d) Assumiu a CPU! \n", pid);

    manager->executing = pid;
    target = manager->list->pcbs[pid];

    manager->cpu->processTime = 0;
    manager->cpu->lastTime = target->cpuTime;
    manager->cpu->pcb = target;
}

/**
 * @brief Função de Escalonamento. Retorna o ID do processo que será
 * executado pela CPU.
 * 
 * @details A ordem de escalonamento é inversamente proporcional à
 * [priority], ou seja, quanto menor o valor de [priority] maior serão
 * as chances do processo assumir a CPU, e vice-versa.
 * 
 * @param manager 
 * @return ID do processo que assumirá a CPU. Caso não haja processos
 * prontos, retorna -1. 
 */
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

/**
 * @brief Realiza um ajuste nos PPID's dos processos não finalizados, após
 * o término de um processo.
 * 
 * @details Como há realocação de PID's, para que não haja casos no qual
 * um processo acabe sendo filho e pai, simultaneamente, de um outro 
 * processo, é preciso ajustar os PPID's quando um processo termina.
 * Assim como ocorre em alguns Sistemas Operacionais, o processo [init]
 * se torna o pai dos processos órfãos.
 * 
 * @param pcbList Lista com os PCB's não finalizados.
 * @param list Lista encadeada com os PID's dos processos a serem ajustados.
 * @param terminated PID do processo que terminou.
 */
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

/**
 * @brief Limpa a lista de ID's que serão realocados, caso seja necessário.
 * 
 * @details Verifica processo a processo, ao encontrar um espaço vazio,
 * confere se todos os elemento preenchidos de [list] foram analisados,
 * em caso afirmativo, limpa [ids], caso contrário, há lacunas de PID's
 * na Tabela PCB e, portanto, nada é feito.
 * 
 * @param list Lista com os PCB's não finalizados.
 * @param ids Lista com os ID's a serem realocados.
 */
void clearIDList(PCBList *list, LinkedList *ids)
{
    int i;

    for (i = 0; i < list->expanded * DEFAULT_LIST_LENGTH; i++)
        if (!list->pcbs[i])
            break;

    if (i == list->length)
        clearLinkedList(ids);
}

/**
 * @brief Processa os comandos recebidos do Commander.
 * 
 * @details Não há diferenciação entre caracteres maiúsculos e minúsculos.
 * Consiste em um loop que não se encerra enquanto o comando lido for
 * diferente de T. O comando Q aciona a execução da CPU e dependendo de
 * seu status de retorno, são realizadas medidas adicionais de ajuste no
 * [manager]. O comando U desbloqueia o primeiro processo bloqueado.
 * O comando P cria o processo Reporter que imprime o estado atual de
 * [manager].
 * 
 * @param manager Ponteiro para o Manager do simulador.
 */
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

        switch (input)
        {
        case 'Q':
            (manager->time)++;

            // CPU vazia
            if (!manager->cpu->pcb)
            {
                // Nenhum processo pronto
                if (firstLinkedList(manager->ready) == -1)
                {
                    printf("CPU vazia! ");

                    if (firstLinkedList(manager->blocked) == -1)
                    {
                        // Todos os processos foram finalizados
                        printf("Não há nenhum processo restante no sistema!\n\n");
                        manager->time--;
                    }
                    else
                        // Há somente processos bloqueados
                        printf("Não há processos prontos no momento!\n\n");

                    continue;
                }

                // Troca de contexto para o processo selecionado pela função de escalonamento
                auxiliarID = schedule(manager);
                changeContext(manager, auxiliarID);
            }

            // Executa a próxima instrução do processo na CPU
            status = executeCPU(manager->cpu, manager->list, manager->nextID, manager->time);

            switch (status)
            {
            case EXIT_SUCCESS:
                break;

            // Insere o ID da cópia instantânea na lista de processos prontos
            case FORK_PROCESS:
                printf("(PID = %d) Foi criado!\n", manager->nextID);

                // Houve uma realocação de PIDs cujos processos já foram terminados
                if (manager->nextID != manager->list->length)
                    pollLinkedList(manager->ids);

                insertLinkedList(manager->ready, manager->nextID, manager->list->pcbs[manager->nextID]->priority);
                clearIDList(manager->list, manager->ids);

                manager->nextID = NEXT_ID(manager->list->length, firstLinkedList(manager->ids));
                break;

            // Insere o processo atual na lista de processos bloqueados
            case BLOCK_PROCESS:
                printf("(PID = %d) Processo bloqueado!\n", manager->cpu->pcb->pid);
                insertLinkedList(manager->blocked, manager->cpu->pcb->pid, 0);
                initializeCPU(manager->cpu);
                break;

            // Insere o processo atual na lista de processos bloqueados
            case TERMINATE_PROCESS:
                // Caso o ID do processo finalizado seja diferente de 0,
                if (manager->executing)
                {
                    printf("(PID = %d) Processo finalizado!\n", manager->cpu->pcb->pid);

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
                clearIDList(manager->list, manager->ids);

                manager->nextID = NEXT_ID(manager->list->length, firstLinkedList(manager->ids));
                break;

            case UNKNOWN_COMMAND:
                printf("Comando não reconhecido!\n");
                break;
            }

            auxiliarID = schedule(manager);
            changeContext(manager, auxiliarID);

            break;

        case 'U':
            // Altera o estado do primeiro processo na fila de bloqueados para pronto
            auxiliarID = pollLinkedList(manager->blocked);

            if (auxiliarID == -1)
            {
                printf("Não há processos bloqueados no momento!\n");
            }
            else
            {
                printf("(PID = %d) Processo desbloqueado!\n", auxiliarID);
                insertLinkedList(manager->ready, auxiliarID, manager->list->pcbs[auxiliarID]->priority);
            }

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

        case 'H':
            printHelp();
            break;

        default:
            printf("Comando não reconhecido! Mande H para obter ajuda!\n");
            break;
        }

        printf("\n");
    } while (input != 'T');
}

/**
 * @brief Imprime [character] repetidamente, [times] vezes.
 * 
 * @param character Caractere que será impresso.
 * @param times Quantidade de caracteres impressos.
 * @param breakline Quebra de linha após a repetição.
 */
void printRepeat(char character, int times, bool breakline)
{
    int i;

    for (i = 0; i < times; i++)
        printf("%c", character);

    if (breakline)
        printf("\n");
}

/**
 * @brief Imprime [title] centralizado em uma linha de tamanho [LINE_LENGTH].
 * 
 * @param title Texto que será impresso.
 * @param vspace Decide se coloca ou não uma quebra de linha antes do título.
 */
void printTitle(char *title, bool vspace)
{
    if (vspace)
        printf("\n");

    // O +1 serve para "arredondar para cima"
    int freeSpace = LINE_LENGTH - strlen(title) + 1;
    printRepeat(' ', freeSpace / 2, false);
    printf("%s\n", title);
}

/**
 * @brief Imprime os processos de [table] cujos ID's estão em [list].
 * 
 * @param table Lista com os PCB's não finalizados.
 * @param list Lista encadeada com os ID's a serem impressos.
 */
void printFromList(PCBList *table, LinkedList *list)
{
    const int length = list->length;
    int array[length];

    linkedListToArray(list, array);

    for (int i = 0; i < length; i++)
        printPCB(table->pcbs[array[i]]);
}

/**
 * @brief Imprime a lista de comandos aceitos pelo sistema.
 */
void printHelp()
{
    printf("\n");
    printRepeat('=', LINE_LENGTH, true);
    printTitle("LISTA DE COMANDOS", false);
    printRepeat('=', LINE_LENGTH, true);

    printf("Q: Fim de uma unidade de tempo\n");
    printf("U: Desbloqueia um processo\n");
    printf("P: Imprime o estado atual do sistema\n");
    printf("T: Finaliza o simulador\n");

    printRepeat('=', LINE_LENGTH, true);
}

/**
 * @brief Imprime [longText] limitando cada linha em [lineLength]
 * caracteres.
 * 
 * @param longText Texto que será impresso.
 * @param lineLength Quantidade máxima de caracteres por linha.
 */
void printWrap(char *longText, const int lineLength)
{
    const char specialCharacters[] = "ãáàâéêíóõôú";
    char line[lineLength + 1];
    int counter;
    int i, j;

    i = 0;
    j = 0;
    counter = 0;

    while (longText[i] != '\0')
    {
        // Imprime a linha contabilizada
        if (counter >= lineLength)
        {
            line[j] = '\0';
            j = 0;
            counter = 0;
            printf("%s\n", line);
            line[0] = '\0';
            continue;
        }

        // Elimina espaços no começo da linha
        if (!counter && longText[i] == ' ')
            continue;

        // É um caractere acentuado
        if (strchr(specialCharacters, longText[i]))
        {
            line[j++] = longText[i++];
            line[j++] = longText[i++];

            counter++;
            continue;
        }

        line[j++] = longText[i++];
        counter++;
    }

    line[j] = '\0';

    if (line[0] != '\0')
        printf("%s\n", line);
}

/**
 * @brief Imprime uma mensagem apresentando o sistema.
 */
void printWelcome()
{
    char welcomeMessage[] = "Esse programa tem como objetivo dar uma visão geral, através de uma simulação, de como ocorre a execução de processos em uma CPU, bem como as estratégias utilizadas para a atuação de múltiplos processos.\n";
    // char welcomeMessage[] = "ããããã";

    printf("\n");
    printRepeat('=', LINE_LENGTH, true);
    printTitle("SIMULADOR DE GERENCIAMENTO DE PROCESSOS", false);
    printRepeat('=', LINE_LENGTH, true);

    printWrap(welcomeMessage, LINE_LENGTH);
}

/**
 * @brief Cria um novo processo para imprimir o estado atual de [manager],
 * em seguida, o mesmo é finalizado.
 * 
 * @param manager Estrutura que será impressa.
 */
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
        printf("\n");
        printf("[Reporter] Iniciado!\n");
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

        printRepeat('=', LINE_LENGTH, true);
        printf("[Reporter] Encerrado com sucesso!\n");

        exit(EXIT_SUCCESS);
    }
}

/**
 * @brief Destrói [manager] liberando a memória alocada.
 */
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
