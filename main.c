#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct Comando{
    char nome[20];
    char programa[20];
    char** args;
    struct Comando *next;
} Comando;

void adicionar_comando(Comando** lista, char nome[], char programa[], char** args){
    if (lista == NULL){
        (*lista) = (Comando*) malloc(sizeof(Comando));
    }else {
        while ((*lista)->next != NULL) {
            (*lista) = (*lista)->next;
        }

        (*lista)->next = (Comando*) malloc(sizeof(Comando));
        (*lista) = (*lista)->next;
    }

    strcpy((*lista)->nome, nome);
    strcpy((*lista)->programa, programa);
    (*lista)->args = args;
    (*lista)->next = NULL;
}

int main(){
    Comando* lista_comandos = NULL;
    char comando[50] = "";
    char* token;
    while ((strcmp(comando, "exit") != 0)) {
        printf("processflow> ");
        fgets(comando, sizeof(comando), stdin);

        token = strtok(comando, " \n");

        while (token != NULL) {
            if (strcmp(token, "task") == 0) {
                
                int cont = 0;
                char nome[20];
                char programa[20];
                char **args = (char**) malloc(20 * sizeof(char*));

                token = strtok(NULL, " ");
                // Agora sim descarta só o primeiro

                while (token != NULL) {
                    if (cont == 0){
                        strcpy(nome, token);
                    }else if (cont == 1) {
                        strcpy(programa, token);
                    }else{
                        args[cont-2] = (char*) malloc(20 * sizeof(char));
                        strcpy(args[cont-2], token);
                    }
                    token = strtok(NULL, " ");
                    cont++;
                }

                if (cont >= 2) {
                    args[cont-2] = NULL;
                }else {
                    args[0] = NULL;
                }

                adicionar_comando(&lista_comandos, nome, programa, args);

            }else if (strcmp(token, "run") == 0) {
                pid_t pid = fork();

                if (pid < 0) {
                    perror("fork failed");
                }else if (pid == 0) {
                    int cont = 0;

                    while (token != NULL) {
                        cont++;
                        token = strtok(NULL, " \n");
                    }

                    char *args[cont+1];
                    args[cont] = NULL;
                    cont = 0;

                    token = strtok(comando, "\0");
                    // Separações na string original são substituidas por \0 pelo strtok

                    while (token != NULL) {
                        token = strtok(NULL, "\0");
                        // Agora sim descarta só o primeiro
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