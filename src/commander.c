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
            // Nos requisitos tá especificando 1s, mas para testar fica muito lento
            sleep(1);
            printf("Insira um comando: ");
            fflush(stderr);
            scanf("%c", &input);
            input = toupper(input);
            write(pipefd[1], &input, 1);
        } while (input != 'T');

        wait(NULL);
        close(pipefd[1]);
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
 * Com os programas atuais em [program] o programa deve se comportar da seguinte maneira:
 * 
 * TIME | (PID: INSTRUCTION)

 *  1 | (0: 0) - 1000
 *  2 | (0: 1) - 1019
 *  3 | (0: 2) - 1039
 *  4 | (0: 3) - 1016
 *  5 | (0: 4) - 1071
 *  6 | (0: 5) - Criou PID 1
 *  7 | (0: 6) - Bloqueou
 *  8 | (1: 0) - Trocou para p1
 *  9 | (1: 1) - 64
 * 10 | (1: 2) - 164
 * 11 | (1: 3) - 132
 * 12 | (1: 4) - Criou PID 2
 * 13 | (1: 5) - Bloqueou
 * 14 | (2: 0) - Trocou para p4
 * 15 | (2: 1) - 100
 * 16 | (2: 2) - 120
 * 17 | (2: 3) - 40
 * 18 | (2: 4) - Bloqueou






 */