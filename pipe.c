#include "header.h"

void executar_pipe(char **comandos, Comando *lista_comandos){
    int cont = 0;
    char** temp_comandos = comandos;

    while (temp_comandos[cont] != NULL) {
        cont++;
    }

    cont--; // Descarta primeiro valor de comando, que é "pipe"

    // Acabei de perceber que não foi a melhor ideia colocar fd no plural
    int** lista_fds = (int**) malloc(20 * sizeof(int*));
    
    // Cont-1 já que é preciso um pipe a menos do que a quantidade total de comandos
    for (int i = 0; i < cont-1; i++) {
        lista_fds[i] = (int*) malloc(2 * sizeof(int));
        pipe(lista_fds[i]);
        lista_fds[i+1] = NULL;
    }

    int amount_pipes = cont-1;

    if (amount_pipes < 1) {
        printf("fewer commands than expected\n");
        free(lista_fds);
        return;
    }

    cont = 1; // Primeiro valor de comando é "pipe"
    
    pid_t** lista_pids = (pid_t**) malloc(20 * sizeof(pid_t*));

    while (comandos[cont] != NULL) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");

        }else if (pid == 0) {
            if (cont == 1) {
                dup2(lista_fds[cont-1][1], STDOUT_FILENO);
                for (int i = 0; i < amount_pipes; i++) {
                    close(lista_fds[i][0]);
                    close(lista_fds[i][1]);
                }
            }else if (comandos[cont+1] == NULL) {
                dup2(lista_fds[cont-2][0], STDIN_FILENO);
                for (int i = 0; i < amount_pipes; i++) {
                    close(lista_fds[i][0]);
                    close(lista_fds[i][1]);
                }
            }else {
                dup2(lista_fds[cont-2][0], STDIN_FILENO);
                dup2(lista_fds[cont-1][1], STDOUT_FILENO);
                for (int i = 0; i < amount_pipes; i++) {
                    close(lista_fds[i][0]);
                    close(lista_fds[i][1]);
                }
            }
            Comando* temp = lista_comandos;
            while (temp != NULL) {
                if (strcmp(temp->nome, comandos[cont]) == 0) {
                    aplicar_redirecionamentos(temp);
                    execvp(temp->args[0], temp->args);
                    perror("program not found");
                    _exit(1);
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
    
    for (int i = 0; i < amount_pipes; i++) {
        close(lista_fds[i][0]);
        close(lista_fds[i][1]);
    }

    lista_pids[cont-1] = NULL;
    cont = 0;

    while (lista_pids[cont] != NULL) {
        int status;
        waitpid(*(lista_pids[cont]), &status, 0);
        free(lista_pids[cont]);

        cont++;
    }

    cont = 0;
    while (lista_fds[cont] != NULL) {
        free(lista_fds[cont]);
        cont++;
    }
    free(lista_fds);
}