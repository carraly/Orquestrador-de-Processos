#include "header.h"

void adicionar_comando(Comando** lista, char nome[], char** args){
    if (*lista == NULL){
        (*lista) = (Comando*) malloc(sizeof(Comando));
    }else {
        while ((*lista)->next != NULL) {
            (*lista) = (*lista)->next;
        }

        (*lista)->next = (Comando*) malloc(sizeof(Comando));
        (*lista) = (*lista)->next;
    }

    strcpy((*lista)->nome, nome);
    (*lista)->args = args;
    (*lista)->next = NULL;
}

int main(){
    Comando* lista_comandos = NULL;
    char input_comando[50] = "";
    char* token;
    while ((strcmp(input_comando, "exit") != 0)) {
        printf("processflow> ");
        fgets(input_comando, sizeof(input_comando), stdin);

        token = strtok(input_comando, " \n");

        while (token != NULL) {
            if (strcmp(token, "exit") == 0) {
                break;

            }else if (strcmp(token, "task") == 0) {
                int cont = 0;
                char nome[20];
                char **args = (char**) malloc(20 * sizeof(char*));

                token = strtok(NULL, " ");
                // Agora sim descarta só o primeiro

                while (token != NULL) {
                    if (cont == 0){
                        strcpy(nome, token);
                    }else{
                        args[cont-1] = (char*) malloc(20 * sizeof(char));
                        strcpy(args[cont-1], token);
                    }
                    token = strtok(NULL, " ");
                    cont++;
                }

                if (cont < 2) {
                    perror("invalid task");
                    continue;
                    }

                args[cont-1] = NULL;

                adicionar_comando(&lista_comandos, nome, args);

            }else {
                char modo[20];
                strcpy(modo, token);
                
                int cont = 0;
                char **comandos = (char**) malloc(20 * sizeof(char*));

                token = strtok(NULL, " ");
                // Agora sim descarta só o primeiro

                while (token != NULL) {
                    comandos[cont] = (char*) malloc(20 * sizeof(char));
                    strcpy(comandos[cont], token);
                    token = strtok(NULL, " ");
                    cont++;
                }

                if (cont < 1) {
                    perror("invalid task");
                    continue;
                }

                comandos[cont] = NULL;
                if (strcmp(modo, "run") == 0) {
                    if (strcmp(comandos[0], "sequential") == 0) {
                        executar_sequencial(comandos, lista_comandos);
                        
                    }else if (strcmp(comandos[0], "parallel") == 0) {
                        executar_paralelo(comandos, lista_comandos);

                    }else if (strcmp(comandos[0], "pipe") == 0) {
                        executar_pipe(comandos, lista_comandos);

                    }else {
                        pid_t pid = fork();
                        if (pid < 0) {
                            perror("fork failed");

                        }else if (pid == 0) {
                            Comando* temp = lista_comandos;
                            while (temp != NULL) {
                                if (strcmp(temp->nome, comandos[0]) == 0) {
                                    execvp(temp->args[0], temp->args);
                                    perror("program not found");
                                    exit(4);
                                }
                                temp = temp->next;
                            }
                            perror("invalid command");
                            exit(2);
                        }else {
                            token = NULL;
                            int status;

                            waitpid(pid, &status, 0);

                            if (WIFEXITED(status)){
                                int codigo = WEXITSTATUS(status);
                                printf("Task exited with code %d\n", codigo);
                            }
                        }
                    }
                }else if (strcmp(modo, "input") == 0) {
                    if (cont < 2) {
                        perror("invalid task");
                        continue;
                    }
                    ler_input(comandos, lista_comandos);
                }else if (strcmp(modo, "output") == 0) {
                    if (cont < 2) {
                        perror("invalid task");
                        continue;
                    }
                    //escrever_output(comandos, lista_comandos);
                }else if (strcmp(modo, "append") == 0) {
                    if (cont < 2) {
                        perror("invalid task");
                        continue;
                    }
                    //append_output(comandos, lista_comandos);
                }
            }
        }
    }
    return 0;
}