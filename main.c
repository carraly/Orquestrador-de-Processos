#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    char comando[50] = "";
    char* token;
    while ((strcmp(comando, "exit") != 0)) {
        printf("processflow> ");
        fgets(comando, sizeof(comando), stdin);

        token = strtok(comando, " ");

        while (token != NULL) {
            if (strcmp(token, "task") == 0) {
                pid_t pid = fork();

                if (pid < 0) {
                    perror("fork failed");
                }else if (pid == 0) {
                    int cont = 0;

                    while (token != NULL) {
                        cont++;
                        token = strtok(NULL, " ");
                    }

                    char *args[cont+1];
                    args[cont+1] = NULL;
                    cont = 0;

                    token = strtok(comando, " ");
                    token = strtok(NULL, " ");
                    // Descartar primeiro

                    while (token != NULL) {
                        token = strtok(NULL, " ");
                        args[cont] = token;
                        cont++;
                    }
                    execvp(args[0], args);
                }else {
                    int status;

                    waitpid(pid, &status, 0);

                    if (WIFEXITED(status)){
                        int codigo = WEXITSTATUS(status);
                        printf("Tarefa terminou com código %d\n", codigo);
                    }
                }
            }
            token = strtok(NULL, " ");
        }
    }
    return 0;
}