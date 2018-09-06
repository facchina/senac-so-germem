#include<stdio.h>
#include<stddef.h>
#include "mymalloc.h"

//inicializando o primeiro block
void initialize() {
	//o tamanho do primeiro bloco de memoria equivale a 20.000 bytes menos o tamanho da struct block
	freeList->size = MEMORYLENGHT - sizeof(struct block);
	//setamos free para 1 para indicar que esse bloco ainda nao foi alocado
	freeList->free = 1;
	//setamos next para null
	freeList->next = NULL;
}

// slotMaisApropriado: Um ponteiro para o bloco de metadados que se refere ao bloco de memória cujo tamanho é mais que o necessário
// tamanhoRequerido: o tamanho requerido do pedaço de memória a ser alocado.
void splitMemoryBlock(struct block *slotMaisApropriado, size_t tamanhoRequerido) {
	//cria um ponteiro que aponta para o espaco de memoria que sobrou 
	struct block *new = (void*)((void*)(slotMaisApropriado + tamanhoRequerido + sizeof(struct block)));
	//calcula quanto de memoria sobrou
	new->size = (slotMaisApropriado->size) - tamanhoRequerido - sizeof(struct block);
	//informa q o espaco ta vazio
	new->free = 1;
	//ajusta quem eh o proximo bloco
	new->next = slotMaisApropriado->next;
	//atualiza o tamanho do slot alocado
	slotMaisApropriado->size = tamanhoRequerido;
	//seta ele pra ocupado
	slotMaisApropriado->free = 0;
	//faz apontar para o new
	slotMaisApropriado->next = new;
}
void *Aloca(size_t sizeRequired) {
	//bloco atual
	struct block *curr;
	//bloco anterior
	struct block *prev;
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
void mergeMemoryBlocks() {
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

void Libera(void* ptr) {
	//verifica se o ponteiro passado esta dentro do intervalo de enderecos do array memory
	if (((void*)memory <= ptr) && (ptr <= (void*)(memory + 20000))) {
		struct block* curr = ptr;
		--curr;
		curr->free = 1;
		mergeMemoryBlocks();
	}
	else printf("Ponteiro inválido\n");
}
int main(void) {
	printf("The size of integer is %d\n", sizeof(int));
	//tenta alocar 400 bytes
	int *p = (int)Aloca(100 * sizeof(int));
	//tenta alocar 250 bytes
	char *q = (char)Aloca(250 * sizeof(char));
	Libera(p);
	//tenta alocar 200 bytes
	int *r = (int)Aloca(100 * sizeof(int));
	system("pause");
}