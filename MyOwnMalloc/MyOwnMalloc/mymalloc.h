/*
 * the reference tutorial: https://bit.ly/2oMVY0U
 * A estrat�gia de aloca��o utilizada � a first fit 
 * Esta estrat�gia aloca o espa�o requerido na primeira parti��o 
 * que o couber, independente do espa�o livre que vai deixar. 
 * Das 3 estrat�gias (worst-fit, best-fit e first-fit), 
 * esta � a mais r�pida, pois consome menos recursos do sistema.
 */

#include<stdio.h>
#include<stddef.h>

#define MEMORYLENGHT 800
//alocando 20.000 bytes de memoria, uma vez que um char equivale a 1 byte
char memory[MEMORYLENGHT];

struct block {
	//tamanho do bloco alocado
	size_t size; 
	//flag para saber quando o bloco esta livre ou nao (1: livre, 0: ocupado).
	int free;   
	//ponteiro para o prox block
	struct block *next; 
};

/* Inicializando um ponteiro do tipo block, que aponta para o endereco inicial do espa�o alocado por memory.
   A ideia eh que freeList aponte para o inicio da lista encadeada de blocks, e por isso, o endereco inicial de memory
   deve ser convertido para o tipo void para que aloquemos blocks de tipos de dados diferentes (char, double, int, etc).
*/
struct block *freeList = (void*)memory;

//Definicao de funcoes
void initialize();
void splitMemoryBlock(struct block *slotMaisApropriado, size_t tamanhoRequerido);
void *Aloca(size_t sizeRequired);
void mergeMemoryBlocks();
void Libera(void* ptr);

