//The project included a good set of challenges such as understanding the concept of memory allocation

#include<stdio.h>
#include<stddef.h>
#include"myMalloc.h"

//inicializando o primeiro block
void initialize() {
    //o tamanho do primeiro bloco de memoria equivale a 20.000 bytes menos o tamanho da struct block
    freeList->size = MEMORYLENGHT - sizeof(struct block);
    //setamos free para 1 para indicar que esse bloco ainda nao foi alocado
    freeList->free = 1;
    //setamos next para null
    freeList->next = NULL;
}

// fitting_slot: Um ponteiro para o bloco de metadados que se refere ao bloco de memória cujo tamanho é mais que o necessário
// size: o tamanho requerido do pedaço de memória a ser alocado.
void splitMemoryBlock(struct block *fitting_slot,size_t size){
    //cria um ponteiro que aponta para o espaco de memoria que sobrou
    struct block *new=(void*)((void*)fitting_slot+size+sizeof(struct block));
    //calcula quanto de memoria sobrou
    new->size=(fitting_slot->size)-size-sizeof(struct block);
    //informa q o espaco ta vazio
    new->free=1;
    //ajusta quem eh o proximo bloco
    new->next=fitting_slot->next;
    //atualiza o tamanho do slot alocado
    fitting_slot->size=size;
    //seta ele pra ocupado
    fitting_slot->free=0;
    //faz apontar para o new
    fitting_slot->next=new;
}

//Essa funcao une os blocos livres consecutivos, removendo os blocos de metadados entre eles
//para que haja o maior e melhor aproveitamento da memoria
void *aloca(size_t sizeRequired){
    //bloco atual e anterior
    struct block *curr,*prev;
    //ponteiro para o endereço inicial do espaco de memoria que foi alocado
    void *result;
    
    //se a lista esta vazia inicializa ela
    if (!(freeList->size)) {
        initialize();
        printf("A lista de memoria foi inicializada\n");
    }
    curr = freeList;
    
    //percorre a lista enquanto o tamanho informado pelo bloco atual é menor do que o requerido
    //ou o bloco atual informa que o espaco de memoria ja foi alocado e ainda existir um proximo bloco para consultar
    while ((((curr->size) < sizeRequired) || ((curr->free) == 0)) && (curr->next != NULL)) {
        //previous vai apontar para o que o current aponta
        prev = curr;
        //e o current vai apontar para o que o next aponta
        curr = curr->next;
        //um bloco foi verificado
        printf("Um bloco foi checado e nao pode ser usado :/\n");
    }
    //caso o tamanho informado for igual ao requerido ja sabendo que ele nao esta ocupado por conta do while acima
    if ((curr->size) == sizeRequired) {
        curr->free = 0;
        result = (void*)(++curr);
        printf("O espaco foi alocado em um bloco com o tamanho exato\n");
        return result;
    }
    //caso o tamanho informado seja maior que o tamanho requerido + do novo cabeçalho (tamanho da struct block)
    else if ((curr->size) > (sizeRequired + sizeof(struct block))) {
        splitMemoryBlock(curr, sizeRequired);
        result = (void*)(++curr);
        printf("O espaco foi alocado usando o split\n");
        return result;
    }
    //entao o espaco eh insuficiente
    else {
        result = NULL;
        printf("Nao tem memoria suficiente\n");
        return result;
    }
}

//Essa funcao une os blocos livres consecutivos, removendo os blocos de metadados entre eles
//para que haja o maior e melhor aproveitamento da memoria
void merge(){
    struct block *curr, *prev;
    curr = freeList;
    //enquanto existir um prox bloco
    while ((curr->next) != NULL) {
        //se dois blocos consecutivos estiverem livres
        if ((curr->free) && (curr->next->free)) {
            //define o tamanho dos dois blocos unidos mais o tamanho do header (struct)
            curr->size += (curr->next->size) + sizeof(struct block);
            curr->next = curr->next->next;
        }
        //atualiza os ponteiros
        prev = curr;
        curr = curr->next;
    }
}

//myfree i.e returning memory to freeList and deleting the block
void libera(void* ptr){
    //verifica se o ponteiro passado esta dentro do intervalo de enderecos do array memory
    if(((void*)memory<=ptr)&&(ptr<=(void*)(memory+MEMORYLENGHT))){
        struct block* curr=ptr;
        --curr;
        curr->free=1;
        merge();
    }
    else printf("Please provide a valid pointer allocated by MyMalloc\n");
}

void printMemoryUsed(){
    struct block *curr;
    curr = freeList;
    printf("Percorrendo a memoria\n");
    do{
        printf("Tamanho %zu | Alocado: %d \n", curr->size, curr->free );
        curr = curr->next;
    }while(curr->next != NULL);
    printf("Tamanho %zu | Alocado: %d \n", curr->size, curr->free );
}


int main(){
    char *a = aloca(1000*sizeof(char));
    char *b = aloca(250*sizeof(char));
    char *c = aloca(978*sizeof(char));

    printMemoryUsed();
    
    libera(a);
    libera(b);
    libera(c);
}
