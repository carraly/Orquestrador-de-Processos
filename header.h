#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef struct Comando{
    char nome[50];
    char** args; // Lembrar que arg[0] é o endereço
    char *input_file;
    char *output_file;
    int append; // 0 = truncate, 1 = append
    struct Comando *next;
} Comando;

void executar_paralelo(char** comandos, Comando* lista_comandos);
void executar_sequencial(char** comandos, Comando* lista_comandos);
void executar_pipe(char **comandos, Comando *lista_comandos);
void ler_input(char **comandos, Comando *lista_comandos);
void escrever_output(char **comandos, Comando *lista_comandos);
void append_output(char **comandos, Comando *lista_comandos);
void aplicar_redirecionamentos(Comando *cmd);

#endif   