#include "header.h"

void executar_paralelo(char **comandos, Comando *lista_comandos){
    pid_t** lista_pids = (pid_t**) malloc(20 * sizeof(pid_t*));
    int cont = 1; // Primeiro valor de comando é "paralel"

    while (comandos[cont] != NULL) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");

        }else if (pid == 0) {
            Comando* temp = lista_comandos;
            while (temp != NULL) {
                if (strcmp(temp->nome, comandos[cont]) == 0) {
                    execvp(temp->args[0], temp->args);
                    perror("program not found");
                    _exit(4);
                }
                temp = temp->next;
            }
            printf("invalid command\n");
            _exit(2);
        }else {
            lista_pids[cont-1] = (pid_t*) malloc(sizeof(pid_t));
            *(lista_pids[cont-1]) = pid;
        }
        cont++;
    }

    if (cont < 2) {
        printf("fewer commands than expected\n");
        free(lista_pids);
        return;
    }

    lista_pids[cont-1] = NULL;
    cont = 0;

    while (lista_pids[cont] != NULL) {
        int status;
        waitpid(*(lista_pids[cont]), &status, 0);

        if (WIFEXITED(status)){
            int codigo = WEXITSTATUS(status);
            printf("task exited with code %d\n", codigo);
        }
        free(lista_pids[cont]);

        cont++;
    }
    free(lista_pids);

    printf("all forks finalized\n");
}