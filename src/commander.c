#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main()
{
    char input[256];
    int pipefd[2];
    pid_t cpid;
    size_t i;

    printf("[Commander] Iniciado!\n");

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

        execlp("./manager", "./manager", NULL);
    }

    /* PROCESSO PAI */
    else
    {
        close(pipefd[0]); /* Fecha o terminal de leitura */

        do
        {
            input[0] = '\0';
            sleep(1);
            printf("Insira um ou mais comandos: ");
            scanf("%[^\n]s", input);
            getchar();

            for (i = 0; i < strlen(input); i++)
            {
                input[i] = toupper(input[i]);
                write(pipefd[1], input + i, 1);
            }
        } while (strstr(input, "T") == NULL);

        close(pipefd[1]); /* Fecha o terminal de escrita */
        printf("[Commander] Encerrado com sucesso!\n");
    }

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
 * Fazer testes com [time] no terminal do linux.
 * 
 * 
 * 
 *

 * 

 */
