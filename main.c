#include "header.h"

void limpar_comandos(char** comandos){
    int cont = 0;
    while (comandos[cont] != NULL) {
        free(comandos[cont]);
        cont++;
    }
    free(comandos);
}

void adicionar_comando(Comando** lista, char nome[], char** args){
    Comando* temp = *lista;
    if (temp == NULL){
        temp = (Comando*) malloc(sizeof(Comando));
        *lista = temp;
    } else {
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = (Comando*) malloc(sizeof(Comando));
        temp = temp->next;
    }

    strcpy(temp->nome, nome);
    temp->args = args;
    temp->input_file = NULL;
    temp->output_file = NULL;
    temp->append = 0;
    temp->next = NULL;
}

int main(int argc, char *argv[]){
    setbuf(stdout, NULL);
    FILE *file;
    if (argc == 2) {
        file = fopen(argv[1], "r");

        if (file == NULL) {
            perror("error opening workflow file");
            exit(1);
        }
    }else if (argc != 1) {
        printf("too many arguments on calling processflow\n");
        exit(1);
    }

    Comando* lista_comandos = NULL;
    char input_comando[200] = "";
    char* token;

    pid_t** lista_pids = (pid_t**) malloc(21 * sizeof(pid_t*));
    lista_pids[0] = NULL;

    while ((strcmp(input_comando, "exit") != 0)) {
        if (argc == 1) {
            if (isatty(STDIN_FILENO)) printf("processflow> ");
            if (fgets(input_comando, sizeof(input_comando), stdin) == NULL) {
                if (feof(stdin)) {
                    printf("reached end of input\n");

                }else {
                    perror("fail on reading from stdin");
                }
                strcpy(input_comando, "exit");
            }

        }else if (argc == 2) {
            if (fgets(input_comando, sizeof(input_comando), file) != NULL) {
                printf("%s", input_comando);
                
            }else {
                if (feof(file)) {
                    printf("reached end of input\n");

                }else {
                    perror("fail on reading from file");
                }
                strcpy(input_comando, "exit");
            }
        }

        token = strtok(input_comando, " \n");

        while (token != NULL) {
            if (strcmp(token, "exit") == 0) {
                int cont = 0;
                while (lista_pids[cont] != NULL) {

                    int status;
                    waitpid(*lista_pids[cont], &status, 0);

                    free(lista_pids[cont]);

                    cont++;
                }
                free(lista_pids);

                Comando* temp;
                
                while (lista_comandos != NULL) {
                    temp = lista_comandos;
                    lista_comandos = lista_comandos->next;
                    cont = 0;
                    while (temp->args[cont] != NULL) {
                        free(temp->args[cont]);
                        cont++;
                    }
                    free(temp->args);
                    free(temp->input_file);
                    free(temp->output_file);
                    free(temp);
                }

                if (argc == 2) {
                    fclose(file);    
                }

                break;

            }else if (strcmp(token, "task") == 0) {
                int cont = 0;
                char nome[50];
                char **args = (char**) malloc(21 * sizeof(char*));

                token = strtok(NULL, " \n");
                // Agora sim descarta só o primeiro

                while (token != NULL) {
                    if (cont == 0){
                        if (strlen(token) >= 50) {
                            printf("command name is longer than expected\n");
                            token = NULL;
                            continue;
                        }
                        strcpy(nome, token);
                    }else{
                        if (strlen(token) >= 200) {
                            printf("command file/argument is longer than expected\n");
                            token = NULL;
                            continue;
                        }
                        if (cont >= 20) {
                            printf("more arguments than expected\n");
                            token = NULL;
                            continue;
                        }
                        args[cont-1] = (char*) malloc(200 * sizeof(char));
                        strcpy(args[cont-1], token);
                    }
                    token = strtok(NULL, " \n");
                    cont++;
                }

                if (cont < 2) {
                    printf("fewer commands than expected\n");
                    cont = 0;
                    while (args[cont] != NULL) {
                        free(args[cont]);
                        cont++;
                    }
                    free(args);

                    continue;
                    }

                args[cont-1] = NULL;

                adicionar_comando(&lista_comandos, nome, args);

            }else {
                char modo[50];
                if (strlen(token) >= 50) {
                    printf("command mode is longer than expected\n");
                    token = NULL;
                    continue;
                }
                strcpy(modo, token);
                
                int cont = 0;
                char **comandos = (char**) malloc(21 * sizeof(char*));

                token = strtok(NULL, " \n");
                // Agora sim descarta só o primeiro

                while (token != NULL) {
                    if (cont >= 20) {
                        printf("more arguments than expected\n");
                        token = NULL;
                        continue;
                    }
                    if (strlen(token) >= 200) {
                        printf("command file/argument is longer than expected\n");
                        token = NULL;
                        continue;
                    }

                    comandos[cont] = (char*) malloc(200 * sizeof(char));

                    strcpy(comandos[cont], token);
                    token = strtok(NULL, " \n");
                    cont++;
                }

                comandos[cont] = NULL;
                if (strcmp(modo, "run") == 0) {
                    if (cont < 1) {
                        printf("fewer commands than expected\n");
                        token = NULL;
                        continue;
                    }  

                    if (strcmp(comandos[0], "sequential") == 0) {
                        executar_sequencial(comandos, lista_comandos);
                        
                        limpar_comandos(comandos);

                    }else if (strcmp(comandos[0], "parallel") == 0) {
                        executar_paralelo(comandos, lista_comandos);

                        limpar_comandos(comandos);
                        
                    }else if (strcmp(comandos[0], "pipe") == 0) {
                        executar_pipe(comandos, lista_comandos);

                        limpar_comandos(comandos);
                        
                    }else {
                        if (cont > 1) {
                            printf("more arguments than expected\n");
                            token = NULL;
                            continue;
                        }  

                        pid_t pid = fork();
                        if (pid < 0) {
                            perror("fork failed");

                        }else if (pid == 0) {
                            Comando* temp = lista_comandos;
                            while (temp != NULL) {
                                if (strcmp(temp->nome, comandos[0]) == 0) {
                                    aplicar_redirecionamentos(temp);
                                    execvp(temp->args[0], temp->args);
                                    printf("program not found\n");
                                    _exit(1);
                                }
                                temp = temp->next;
                            }
                            printf("invalid command\n");
                            _exit(2);
                        }else {
                            cont = 0;
                            while (comandos[cont] != NULL) {
                                free(comandos[cont]);
                                cont++;
                            }
                            free(comandos);
                            
                            token = NULL;
                            int status;

                            waitpid(pid, &status, 0);
                        }
                    }
                }else if (strcmp(modo, "input") == 0) {
                    if (cont < 2) {
                        printf("fewer commands than expected\n");
                        token = NULL;
                        continue;
                    }else if (cont > 2) {
                        printf("more commands than expected\n");
                        token = NULL;
                        continue;
                    }
                    ler_input(comandos, lista_comandos);

                    limpar_comandos(comandos);

                }else if (strcmp(modo, "output") == 0) {
                    if (cont < 2) {
                        printf("fewer commands than expected\n");
                        token = NULL;
                        continue;
                    }else if (cont > 2) {
                        printf("more commands than expected\n");
                        token = NULL;
                        continue;
                    }
                    escrever_output(comandos, lista_comandos);

                    limpar_comandos(comandos);

                }else if (strcmp(modo, "append") == 0) {
                    if (cont < 2) {
                        printf("fewer commands than expected\n");
                        token = NULL;
                        continue;
                    }else if (cont > 2) {
                        printf("more commands than expected\n");
                        token = NULL;
                        continue;
                    }
                    append_output(comandos, lista_comandos);

                    limpar_comandos(comandos);

                }else if (strcmp(modo, "workdir") == 0) {
                    if (cont < 1) {
                        printf("fewer commands than expected\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;

                    }else if (cont > 1) {
                        printf("more commands than expected\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }
                    if (chdir(comandos[0]) != 0) {
                        perror("directory failed to open");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }

                    limpar_comandos(comandos);
                
                }else if (strcmp(modo, "start") == 0) {
                    if (cont < 1) {
                        printf("fewer commands than expected\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }else if (cont > 1) {
                        printf("more commands than expected\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }

                    cont = 0;

                    while (lista_pids[cont] != NULL) {
                        cont++;
                    }

                    if (cont > 19) {
                        printf("max background processes reached\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }

                    pid_t pid = fork();
                    if (pid < 0) {
                        perror("fork failed");

                    }else if (pid == 0) {
                        Comando* temp = lista_comandos;
                        while (temp != NULL) {
                            if (strcmp(temp->nome, comandos[0]) == 0) {
                                aplicar_redirecionamentos(temp);
                                execvp(temp->args[0], temp->args);
                                printf("program not found\n");
                                _exit(1);
                            }
                            temp = temp->next;
                        }
                        printf("invalid command\n");
                        _exit(2);

                    }else {
                        token = NULL;
                        lista_pids[cont] = (pid_t*) malloc(sizeof(pid_t));
                        *(lista_pids[cont]) = pid;
                        lista_pids[cont+1] = NULL;

                        printf("[%d] %d\n", cont+1, pid);

                        limpar_comandos(comandos);
                    }
                }else if (strcmp(modo, "jobs") == 0) {
                    if (cont != 0) {
                        printf("invalid task\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }

                    cont = 0;

                    while (lista_pids[cont] != NULL) {
                        printf("[%d] %d\n", cont+1, *(lista_pids[cont]));
                        cont++;
                    }

                    limpar_comandos(comandos);

                }else if (strcmp(modo, "wait") == 0) {
                    if (cont != 1) {
                        printf("invalid task\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }

                    int job_id = atoi(comandos[0]);

                    if (job_id < 1 || job_id > 20) {
                        printf("invalid job_id\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;;
                    }

                    job_id--; // Para compensar o erro do valor mostrado ao usuário

                    if (lista_pids[job_id] == NULL){
                        printf("invalid pid\n");
                        token = NULL;
                        limpar_comandos(comandos);
                        continue;
                    }

                    int status;

                    waitpid(*lista_pids[job_id], &status, 0);
                    
                    cont = job_id;

                    while (lista_pids[cont] != NULL) {
                        if (lista_pids[cont+1] == NULL) {
                            free(lista_pids[cont]);
                            lista_pids[cont] = NULL;
                        }else {
                            *(lista_pids[cont]) = *(lista_pids[cont+1]);
                            cont++; 
                        }
                    }

                    limpar_comandos(comandos);

                }else {
                    printf("command not found\n");
                }
            }
        }
    }
    return 0;
}