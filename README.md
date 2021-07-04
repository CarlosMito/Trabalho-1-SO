# Trabalho Prático 1 (SO)

## Descrição

Simulador de gerenciamento de processos. Executa tarefas como troca de imagem, troca de contexto, escalonamento, criação de processos, entre outros em um microambiente escrito em C.

## Compilação

Primeiramente, é preciso compilar e criar os arquivos objeto `.o` de `linkedList.c`, `program.c`, `pcb.c` e `cpu.c`. Em seguida, compilar os arquivos `commander.c` e `manager.c`, separadamente, criando os programas executáveis de nome `commander` e `manager`, respectivamente. O executável de `manager.c` deve ser linkado com todos os arquivos objetos criados anteriormente.

O script `compile.sh` foi escrito para executar os passos descritos acima. Basta rodar o seguinte comando no terminal:

```
$ sh compile.sh
```

## Execução

Após a compilação, para rodar o programa basta executar:

```
$ ./commander
```

## Comandos

### Simulador

- **Q**: Fim de uma unidade de tempo.
- **U**: Desbloqueia um processo simulado.
- **P**: Imprime o estado atual do sistema.
- **T**: Finaliza o simulador.

### Programa Simulado

- **S *n***: Atualiza o valor da variável inteira para *n*.
- **A *n***: Soma *n* unidades à variável inteira.
- **D *n***: Subtrai *n* unidades da variável inteira.
- **F *n***: Bifurca o processo e pula *n* instruções.
- **B**: Bloqueia o processo simulado.
- **R *filename***: Troca de imagem para o programa *filename*.
- **E**: Encerra o processo simulado.

## Observações
- Os arquivos fonte dos programas simulados devem estar, necessariamente, dentro da pasta `programs/` para que os mesmos possam ser lidos.
- O programa roda somente em Sistemas Operacionais que utilizam o padrão POSIX.