# processflow

Interpretador de comandos no estilo shell, escrito em C. Permite cadastrar
tarefas (programa + argumentos), executá-las de forma simples, sequencial,
paralela ou encadeada por pipes, redirecionar entrada/saída, trocar de
diretório de trabalho e controlar processos em segundo plano.

Toda a execução de processos é feita com fork, exec, wait, dup2 e pipe.
Não usa system() nem popen().

## Compilar

```
make clean
make
```

Compila sem warnings com gcc -Wall -Wextra.

## Executar

Modo interativo:

```
./processflow
```

Modo workflow (lê comandos de um arquivo):

```
./processflow caminho/para/arquivo.pf
```

No modo workflow, cada linha do arquivo é impressa antes de ser processada.

## Comandos

- task nome programa [args...]
  Cadastra uma tarefa nomeada, associada a um programa e seus argumentos.

- run nome
  Executa uma tarefa cadastrada.

- run sequential t1 t2 ...
  Executa as tarefas em sequência, aguardando cada uma terminar antes da
  próxima começar.

- run parallel t1 t2 ...
  Executa todas as tarefas em paralelo e aguarda todas terminarem.

- run pipe t1 t2 ...
  Executa as tarefas encadeadas, ligando a saída de cada uma à entrada da
  seguinte.

- input nome arquivo
  Redireciona a entrada padrão da tarefa para o arquivo informado.

- output nome arquivo
  Redireciona a saída padrão da tarefa para o arquivo informado,
  sobrescrevendo o conteúdo.

- append nome arquivo
  Redireciona a saída padrão da tarefa para o arquivo informado,
  acrescentando ao conteúdo existente.

- workdir caminho
  Troca o diretório de trabalho do processo.

- start nome
  Executa uma tarefa em segundo plano e imprime o id do job e o pid.

- jobs
  Lista os processos em segundo plano ativos.

- wait id
  Aguarda o término de um processo em segundo plano específico.

- exit
  Aguarda todos os processos em segundo plano pendentes e encerra o
  programa.

## Exemplo de arquivo de workflow

```
task ordenar /usr/bin/sort
output ordenar saida.txt
input ordenar entrada.txt
run ordenar
exit
```

## Estrutura de arquivos

- main.c — laço principal, leitura e parsing dos comandos, lista de
  tarefas cadastradas e lista de jobs em segundo plano.
- header.h — struct Comando e protótipos das funções de execução.
- sequencial.c — execução sequencial de tarefas.
- paralelo.c — execução paralela de tarefas.
- pipe.c — execução de tarefas encadeadas por pipe.
- files.c — redirecionamento de entrada/saída (aplicado no momento do
  run, não da declaração).
- makefile — alvos de build (processflow) e limpeza (clean).

## Limites

- até 20 argumentos por comando
- até 50 caracteres por nome de tarefa
- até 200 caracteres por argumento ou caminho de arquivo
- até 20 processos em segundo plano simultâneos