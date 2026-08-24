#include "header.h"
#include <stdio.h>

// comandos[0] comando
// comandos[1] arquivo

void ler_input(char **comandos, Comando *lista_comandos) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");

    }else if (pid == 0) {
        int file = open(comandos[1], O_RDONLY);

        if (file == -1) {
            perror("error opening file");
            exit(1);
        }

        dup2(file, STDIN_FILENO);

        close(file);

        Comando* temp = lista_comandos;
        while (temp != NULL) {
            if (strcmp(temp->nome, comandos[0]) == 0) {
                execvp(temp->args[0], temp->args);
                perror("program not found");
                exit(1);
            }
            temp = temp->next;
        }
        printf("invalid command");
        exit(1);
    }else {
        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status)){
            int codigo = WEXITSTATUS(status);
            printf("Task exited with code %d\n", codigo);
        }
    }
}

void escrever_output(char **comandos, Comando *lista_comandos) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");

    }else if (pid == 0) {
        int file = open(comandos[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (file == -1) {
            perror("error opening file");
            exit(1);
        }

        dup2(file, STDOUT_FILENO);

        close(file);

        Comando* temp = lista_comandos;
        while (temp != NULL) {
            if (strcmp(temp->nome, comandos[0]) == 0) {
                execvp(temp->args[0], temp->args);
                perror("program not found");
                exit(1);
            }
            temp = temp->next;
        }
        printf("invalid command");
        exit(1);
    }else {
        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status)){
            int codigo = WEXITSTATUS(status);
            printf("Task exited with code %d\n", codigo);
        }
    }
}

void append_output(char **comandos, Comando *lista_comandos) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");

    }else if (pid == 0) {
        int file = open(comandos[1], O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (file == -1) {
            perror("error opening file");
            exit(1);
        }

        dup2(file, STDOUT_FILENO);

        close(file);

        Comando* temp = lista_comandos;
        while (temp != NULL) {
            if (strcmp(temp->nome, comandos[0]) == 0) {
                execvp(temp->args[0], temp->args);
                perror("program not found");
                exit(1);
            }
            temp = temp->next;
        }
        printf("invalid command");
        exit(1);
    }else {
        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status)){
            int codigo = WEXITSTATUS(status);
            printf("Task exited with code %d\n", codigo);
        }
    }
}