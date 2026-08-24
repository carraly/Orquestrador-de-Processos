#include "header.h"
#include <stdio.h>

void executar_sequencial(char **comandos, Comando *lista_comandos) {
    int cont = 1; // Primeiro valor de comando é "sequential"

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
                    exit(4);
                }
                temp = temp->next;
            }
            printf("invalid command");
            exit(2);
        }else {
            int status;

            waitpid(pid, &status, 0);

            if (WIFEXITED(status)){
                int codigo = WEXITSTATUS(status);
                printf("Task exited with code %d\n", codigo);
            }
        }

        cont++;
    }
    if (cont < 2) {
        printf("fewer commands than expected");
        return;
    }
}