#include "header.h"

// comandos[0] comando
// comandos[1] arquivo

void ler_input(char **comandos, Comando *lista_comandos) {
    Comando* temp = lista_comandos;
    while (temp != NULL) {
        if (strcmp(temp->nome, comandos[0]) == 0) {
            free(temp->input_file);
            temp->input_file = (char*) malloc((strlen(comandos[1]) + 1) * sizeof(char));
            strcpy(temp->input_file, comandos[1]);
            return;
        }
        temp = temp->next;
    }
    printf("invalid command\n");
}
 
void escrever_output(char **comandos, Comando *lista_comandos) {
    Comando* temp = lista_comandos;
    while (temp != NULL) {
        if (strcmp(temp->nome, comandos[0]) == 0) {
            free(temp->output_file);
            temp->output_file = (char*) malloc((strlen(comandos[1]) + 1) * sizeof(char));
            strcpy(temp->output_file, comandos[1]);
            temp->append = 0;
            return;
        }
        temp = temp->next;
    }
    printf("invalid command\n");
}
 
void append_output(char **comandos, Comando *lista_comandos) {
    Comando* temp = lista_comandos;
    while (temp != NULL) {
        if (strcmp(temp->nome, comandos[0]) == 0) {
            free(temp->output_file);
            temp->output_file = (char*) malloc((strlen(comandos[1]) + 1) * sizeof(char));
            strcpy(temp->output_file, comandos[1]);
            temp->append = 1;
            return;
        }
        temp = temp->next;
    }
    printf("invalid command\n");
}

void aplicar_redirecionamentos(Comando *cmd) {
    if (cmd->input_file != NULL) {
        int file = open(cmd->input_file, O_RDONLY);
        if (file == -1) {
            perror("error opening file");
            _exit(1);
        }
        dup2(file, STDIN_FILENO);
        close(file);
    }
 
    if (cmd->output_file != NULL) {
        int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
        int file = open(cmd->output_file, flags, 0644);
        if (file == -1) {
            perror("error opening file");
            _exit(1);
        }
        dup2(file, STDOUT_FILENO);
        close(file);
    }
}