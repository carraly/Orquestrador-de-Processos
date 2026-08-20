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
        scanf("%s", comando);

        token = strtok(comando, " ");

        while (token != NULL) {
            if (strcmp(token, "task") == 0) {
                pid_t pid = fork();

                if (pid < 0) {
                    perror("fork failed");
                }else if (pid == 0) {
                    execvp(const char *file, char *const *argv);
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