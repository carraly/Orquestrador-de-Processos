#include "header.h"

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
                    aplicar_redirecionamentos(temp);
                    execvp(temp->args[0], temp->args);
                    perror("program not found");
                    _exit(4);
                }
                temp = temp->next;
            }
            printf("invalid command\n");
            _exit(2);
        }else {
            int status;

            waitpid(pid, &status, 0);
        }

        cont++;
    }
    if (cont < 2) {
        printf("fewer commands than expected\n");
        return;
    }
}