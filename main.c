#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

    pid_t** lista_pids = (pid_t**) malloc(20 * sizeof(pid_t*));
    lista_pids[0] = NULL;

    while ((strcmp(input_comando, "exit") != 0)) {
        printf("processflow> ");
        fgets(input_comando, sizeof(input_comando), stdin);

        token = strtok(input_comando, " \n");

        while (token != NULL) {
            if (strcmp(token, "exit") == 0) {
                int cont = 0;
                while (lista_pids[cont] != NULL) {

                    int status;
                    waitpid(*lista_pids[cont], &status, 0);

                    if (WIFEXITED(status)){
                        int codigo = WEXITSTATUS(status);
                        printf("Task exited with code %d\n", codigo);
                    }
                    free(lista_pids[cont]);

                    cont++;
                }
                free(lista_pids);

                Comando* temp;
                
                cont = 0;
                while (lista_comandos != NULL) {
                    temp = lista_comandos;
                    lista_comandos = lista_comandos->next;
                    while (temp->args[cont] != NULL) {
                        free(temp->args[cont]);
                        cont++;
                    }
                    free(temp->args);
                    free(temp);
                }

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

                comandos[cont] = NULL;
                if (strcmp(modo, "run") == 0) {
                    if (cont < 1) {
                        perror("invalid task");
                        continue;
                    }  

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
                                    exit(1);
                                }
                                temp = temp->next;
                            }
                            perror("invalid command");
                            exit(2);
                        }else {
                            cont = 0;
                            while (comandos[cont] != NULL) {
                                free(comandos[cont]);
                                cont++;
                            }
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
                    if (cont != 2) {
                        perror("invalid task");
                        continue;
                    }
                    ler_input(comandos, lista_comandos);

                }else if (strcmp(modo, "output") == 0) {
                    if (cont != 2) {
                        perror("invalid task");
                        continue;
                    }
                    escrever_output(comandos, lista_comandos);

                }else if (strcmp(modo, "append") == 0) {
                    if (cont != 2) {
                        perror("invalid task");
                        continue;
                    }
                    append_output(comandos, lista_comandos);

                }else if (strcmp(modo, "workdir") == 0) {
                    if (cont != 1) {
                        perror("invalid task");
                        continue;
                    }
                    if (chdir(comandos[0]) != 0) {
                        perror("directory failed to open");
                        continue;
                    }
                }else if (strcmp(modo, "start") == 0) {
                    if (cont != 1) {
                        perror("invalid task");
                        continue;
                    }

                    cont = 0;

                    while (lista_pids[cont] != NULL) {
                        cont++;
                    }

                    if (cont > 19) {
                        perror("max background processes reached");
                        continue;
                    }

                    pid_t pid = fork();
                    if (pid < 0) {
                        perror("fork failed");

                    }else if (pid == 0) {
                        Comando* temp = lista_comandos;
                        while (temp != NULL) {
                            if (strcmp(temp->nome, comandos[0]) == 0) {
                                execvp(temp->args[0], temp->args);
                                perror("program not found");
                                exit(1);
                            }
                            temp = temp->next;
                        }
                        perror("invalid command");
                        exit(2);

                    }else {
                        token = NULL;
                        pid_t** pids_temp = lista_pids;
                        cont = 0;

                        while (pids_temp[cont] != NULL) {
                            cont++;
                        }
                        
                        lista_pids[cont] = (pid_t*) malloc(sizeof(pid_t));
                        *(lista_pids[cont]) = pid;
                        lista_pids[cont+1] = NULL;

                        printf("[%d] %d\n", cont+1, pid);
                    }
                }else if (strcmp(modo, "jobs") == 0) {
                    if (cont != 0) {
                        perror("invalid task");
                        continue;
                    }

                    cont = 0;

                    while (lista_pids[cont] != NULL) {
                        printf("[%d] %d\n", cont+1, *(lista_pids[cont]));
                        cont++;
                    }
                }else if (strcmp(modo, "wait") == 0) {
                    if (cont != 1) {
                        perror("invalid task");
                        continue;
                    }

                    int job_id = atoi(comandos[0]);

                    if (job_id < 1 || job_id > 20) {
                        perror("Invalid job_id");
                        continue;;
                    }

                    job_id--; // Para compensar o erro do valor mostrado ao usuário

                    if (lista_pids[job_id] == NULL){
                        perror("invalid pid");
                        continue;
                    }

                    int status;

                    waitpid(*lista_pids[job_id], &status, 0);
                    
                    cont = job_id;

                    while (lista_pids[cont] != NULL) {
                        if (lista_pids[cont+1] == NULL) {
                            lista_pids[cont] = NULL;
                        }else {
                            *(lista_pids[cont]) = *(lista_pids[cont+1]);
                            cont++; 
                        }
                    }

                    if (WIFEXITED(status)){
                        int codigo = WEXITSTATUS(status);
                        printf("Task exited with code %d\n", codigo);
                    }
                }
            }
        }
    }
    return 0;
}