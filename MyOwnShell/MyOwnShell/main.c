//
//  main.c
//  MyOwnShell
//
//  Created by Mariana Facchina on 16/09/2018.
//  Copyright © 2018 mariana-facchina. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//chdir(), fork(), exec() e pid_t
#include <unistd.h>
//waitpid() e macros associadas
#include <sys/wait.h>


#define BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

//definicao de funcoes
int lsh_cd(char **args);
int lsh_exit(char **args);


char *builtin_str[] = {
    "cd",
    "exit"
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_exit
};

char *readLine(void)
{
    char *buffer = NULL;
    size_t bufsize = BUFSIZE;
    
    buffer = malloc(bufsize * sizeof(char));
    if (!buffer) {
        fprintf(stderr, "Unable to allocate buffer\n");
        exit(1);
    }
    getline(&buffer, &bufsize, stdin);
    return buffer;
}
//transforma os comandos passados em uma lista de argumentos
char **splitLine(char *line)
{
    int bufsize = BUFSIZE;
    int pos = 0;
    //cria uma lista de tokens com o tamanho passado
    char **tokens = malloc(sizeof(char*) * bufsize);
    char *token;
    
    if(!tokens){
        fprintf(stderr, "Unable to allocate buffer\n");
        exit(1);
    }
    //quebra a string quando encontra um dos caracteres delimitadores
    //retornando um *char e coloca um \0 no final de cada token
    token = strtok(line, TOK_DELIM);
    while(token != NULL){
        tokens[pos++] = token;
        //verifica se eh possivel armazenar o token na lista
        if(pos >= bufsize){
            //aloca mais espaco
            bufsize += BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            //verifica se esta tudo ok
            if(!tokens){
                fprintf(stderr, "Unable to allocate buffer\n");
                exit(1);
            }
        }
        //quebra a string quando achar um dos delimitadores
        printf("%s \n", token);
        token = strtok(NULL, TOK_DELIM);
    }
    //seta o ultimo token para null
    tokens[pos] = NULL;
    //retorna a lista de tokens
    return tokens;
}


int launch(char **args) {
    
    pid_t processID, wpid;
    int status;
    //bifurca o processo atual em dois (parent e child)
    processID = fork();
    if (processID == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (processID < 0) {
        // Error forking
        perror("lsh");
    } else {
        // Parent process
        do {
            //suspende a execução até que o processo-filho especificado mude de estado
            wpid = waitpid(processID, &status, WUNTRACED);
            //a macro WIFEXITED(status) retorna true se o processo filho terminou normalmente
            //a macro WIFSIGNALED(status) retorna true se o processo filho terminou devido ao recebimento de um sinal
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int execute(char **args)
{
    int i;
    
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    
    for (i = 0; i < num_builtins(); i++) {
        //verifica se o comando passado eh igual a algum dos comandos padroes criados (cd e exit)
        if (strcmp(args[0], builtin_str[i]) == 0) {
            //se for igual executa o comando
            return (*builtin_func[i])(args);
        }
    }
    //caso nao seja, ele executa o lauch
    return launch(args);
}

//o loop padrao de um shell passa por 3 etapas principais:
//leitura, analise e execucao dos comandos
void loop(void)
{
    char *line;
    char **args;
    int status;
    
    do {
        printf("> ");
        //leitura dos comandos
        line = readLine();
        //analise dos comandos
        args = splitLine(line);
        status = execute(args);
        
        free(line);
        free(args);
    } while (status);
}

//funcoes do proprio shell
int lsh_cd(char **args)
{
    if (args[1] == NULL) {
        //o diretorio nao foi passado apois o comando cd
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        //tenta mudar o diretorio
        if (chdir(args[1]) != 0) {
            //nao conseguiu mudar o diretorio
            perror("lsh");
        }
    }
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

int main(int argc, char **argv)
{
    // Load config files, if any.
    
    // Run command loop.
    loop();
    
    // Perform any shutdown/cleanup.
    return 0;
}

