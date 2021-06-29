#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    char input;
    int pipefd[2];
    pid_t cpid;

    printf("Commander: Begin\n");

    if (pipe(pipefd) == -1)
    {
        perror("ERRO NO PIPE");
        exit(EXIT_FAILURE);
    }

    if ((cpid = fork()) == -1)
    {
        perror("ERRO NO FORK");
        exit(EXIT_FAILURE);
    }

    /* PROCESSO FILHO */
    if (cpid == 0)
    {
        close(pipefd[1]);              /* Fecha o terminal de escrita */
        dup2(pipefd[0], STDIN_FILENO); /* O filho lerá de [pipefd[0]] quando tentar ler do teclado */
        close(pipefd[0]);              /* Fecha o terminal de leitura */

        execlp("./manager.exe", "./manager.exe", NULL);
    }

    /* PROCESSO PAI */
    else
    {
        close(pipefd[0]); /* Fecha o terminal de leitura */

        do
        {
            sleep(1);
            printf("Insira um comando: ");
            scanf("%c", &input);
            input = toupper(input);
            write(pipefd[1], &input, 1);
        } while (input != 'T');

        wait(NULL);
        close(pipefd[1]); /* Fecha o terminal de escrita */
    }

    printf("Commander: End\n");
    return 0;
}

/*
 * ANOTAÇÕES SOBRE O TRABALHO
 * 
 *    - Após ler uma instrução F <n>, o processo cria um filho no estado Pronto, e adiciona-o
 * na Tabela PCB. O pai continua a execução <n> linhas depois. Por exemplo:
 * 
 * F 1
 * R filename
 * A 10
 * 
 * O processo cria um novo processo, adiciona-o em pronto na tabela PCB, o primeiro comando
 * dele será R, que vai trocar seu código pelo do arquivo [filename]. O pai continua na CPU
 * e executa A 10.
 * 
 *    - O estado de um processo é composto pelo valor da variável e o PC.
 *
 * 
 * 
 * 
 * 
 *
 * ESCALONAMENTO
 *  
 * O Algoritmo de Escalonamento escolhido foi o SRTN (Shortest Remaining Time Next), como as instruções
 * que um processo simulado é capaz de realizar são triviais, não envolvem entrada de dados, e possuem
 * tempos de execução iguais (em relação ao tempo simulado, pois levando em consideração o tempo real os
 * comandos R e B podem variar dependendo do tamanho do programa escolhido para a troca de imagem e do tamanho
 * da lista de bloqueados, respectivamente), então o cálculo para o tempo de execução restante não é nada complexo.
 * 
 * As prioridades dos processos só irão mudar quando os mesmos estiverem dentro da CPU, dessa forma, a lista de 
 * processos prontos se manterá ordenada sem muitas complicações. Lembrando que a ordem de prioridade é inversa,
 * ou seja, quanto menor o valor, maior a prioridade sobre outros processos, e vice-versa.
 * 
 * Os processos bloqueados são armazenados utilizando o conceito de FIFO (First In First Out).
 * 
 * Pelo o que está descrito nos requisitos, o escalonamento deve ocorrer após a execução de uma instrução do
 * processo na CPU.
 * 
 * 
 * 
 * O processo [init] não termina enquanto os processos filhos ainda não terminaram.
 * 
 * No. If the parent is killed, children become children of the init process (that has the process id 1 and is launched as the first user process by the kernel).
 * The init process checks periodically for new children, and waits for them (thus freeing resources that are allocated by their return value).
 * 
 * 
 * Uma dificuldade foi deixar o armazenamento dos programas eficiente. No código atual cada processo possui uma
 * uma cópia da estrutura de dados [Program]. Creio que o ideal seria deixar uma lista de programas já abertos
 * armazenados na memória do [Manager], e cada processo referenciar com um ponteiro seus respectivos programas.
 * Dessa forma, poderia haver múltiplas instâncias de um mesmo programa utilizando uma quantidade mínima de
 * memória.
 * 
 * Outro problema foi sincronizar a execução dos processos Commander, Process Manager e Reporter. Como o computador
 * possui sua própria medida de escalonamento, por vezes é difícil controlar o fluxo de execução de maneira que
 * as instruções sejam executadas na ordem desejada.
 */
