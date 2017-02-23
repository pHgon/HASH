// Trabalho Pratico 1 - Algoritmo e Estrutura de Dados II - 23/02/2017
// Autores - Paulo Henrik Goncalves & Gilberto Kreisler

// Tabela Hash com tratamento de conflito:
// <treatment>:  -linear | -hash_duplo | -quadratica | -encadeamento
// execution: ./exec <treatment> <input_file>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENCADEAMENTO 0
#define LINEAR       1
#define QUADRATICA   2
#define DUPLO        3
#define L_FACTOR     0.75000f
#define INI_SIZE     500


// Celula Hash
struct Node{
	char keyString[101];  // Guarda a String lida
	struct Node *prox;    // Usado no tratamento por encadeamento
}; typedef struct Node celHash;


int option (char argument[]);
void Hash (FILE *inputFile, FILE *outputFile, int cod);
celHash **startHash (int size);
celHash **rehash (celHash **ptr, int *size, int cod);
void destroyHash(celHash **ptr, int size, int cod);
int rot_hash(void *key, int len);
int h1(int key, int size);
int h2(int key, int size);
int f_hash(int key, int size, int i);
int s_hash(int key, int size, int i);
int quad_hash(int key, int size, int i);
int readInput (FILE *inputFile, char *input1, char *input2);
int collisionTreatment(int key, int size, int i, int cod);
int insert (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output);
int delete (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output);
void get (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output);


void main(int argc,char *argv[]){
    if(argc == 1){
    	printf("\nERROR: No argument received .. Program closed!\n");
    	exit(-1);
    }

	int opt = option(argv[1]);  // Recebe o codigo referente ao modo de tratamento escolhido
	FILE *inputFile  = fopen (argv[2], "r");
	FILE *outputFile = fopen("log_output.txt", "w");
	if (inputFile == NULL){
		printf("\nERROR: File not found .. Program closed!\n");
		exit(-1);
	}

	if (opt<0){
		printf("\nERROR: Invalid Argument .. Program closed!\n");
		exit(-1);
	}

	Hash(inputFile, outputFile, opt);

	fclose(inputFile);
	fclose(outputFile);
}


// Retorna opcao de tratamento passado por parâmetro
int option (char argument[]){
	if ((strcmp(argument, "-encadeamento")) == 0)
		return ENCADEAMENTO;
	else{
		if ((strcmp(argument, "-linear")) == 0)
			return LINEAR;
		else{
			if ((strcmp(argument, "-quadratica")) == 0)
				return QUADRATICA;
			else{
				if ((strcmp(argument, "-hash_duplo")) == 0)
					return DUPLO;
				else
					return -1;
			}
		}
	}
}

void Hash (FILE *inputFile, FILE *outputFile, int cod){
	char input1[7], input2[100];
	int key, index, hashSize = INI_SIZE, loadHash = 0;
	celHash **head = startHash(hashSize);  // Inicia o vetor de ponteiros

	while (!feof(inputFile)){
		key = readInput(inputFile, input1, input2); // Le as entradas e calcula a key

		if (key >= 0){
			index = f_hash(key, hashSize, 0);

			if(strcmp(input1, "INSERT") == 0){
				if(insert(head, hashSize, input2, key, index, cod, outputFile)==1){
					loadHash++;
					if(((float)loadHash/(float)hashSize)>=L_FACTOR)
						head = rehash(head, &hashSize, cod);
				}
			}
			else{
				if(strcmp(input1, "DELETE") == 0){
					if(delete(head, hashSize, input2, key, index, cod, outputFile)==1){
						loadHash--;
					}
				}
				else{
					if(strcmp(input1, "GET") == 0)
						get(head, hashSize, input2, key, index, cod, outputFile);
					else{
						printf("\nERROR: input1 contain a invalid command .. Program closed!\n");
						exit(-1);
					}
				}
			}
		}
	}
	destroyHash(head, hashSize, cod);
}

// Inicia a Hash alocando vetor de ponteiros
celHash **startHash (int size){
	int i;
	celHash **ptr;
	ptr = (celHash **) malloc (size * sizeof(celHash *));
	if(ptr==NULL){
		printf("ERROR: Null pointer!\n");
		exit(0);
	}
	return ptr;
}

// Re-hash dobrando o tamanho do vetor, retorna ponteiro para a nova lista
celHash **rehash (celHash **ptr, int *size, int cod){
	celHash **newHash = startHash(*size*2);
	int i, key, index;
	celHash *temp;
	for (i=0; i<*size; i++){
		temp = ptr[i]; // Salva a posicao inicial da lista
		while(ptr[i]!=NULL && strncmp(ptr[i]->keyString,"\0", 1)!=1){
			key = rot_hash(ptr[i]->keyString, strlen(ptr[i]->keyString));  // Calcula a chave
			index = f_hash(key, *size*2, 0);  // Calcula a Funcao Hash Inicial
			insert(newHash, *size*2, ptr[i]->keyString, key, index, cod, NULL); // Insere na Nova hash
			ptr[i] = ptr[i]->prox; // Caso a seja o modo encadeado, a lista percorre ate o final
		}
		ptr[i] = temp; // Recupera a posicao inicial do vetor
	}
	destroyHash(ptr, *size, cod);  // Libera Hash antiga
	*size = *size*2; //Atualiza o tamanho total da hash
	return newHash;
}

// Libera a Hash
void destroyHash(celHash **ptr, int size, int cod){
	int i;
	celHash *temp, *temp2;
	for (i=0; i<size; i++){
		if (ptr[i]!=NULL){
			if (cod==ENCADEAMENTO){ // Caso Encadeamento, libera todas as celulas para depois liberar a head
				temp=ptr[i]; // Salva a posicao inicial da lista
				while(temp!=NULL){
					temp2=temp; // Celula que sera liberada
					temp=temp->prox; // atualiza para a proxima celula
					free(temp2); // libera Celula anterior
				}
			}
			else // Demais casos, libera somente a celula
				free(ptr[i]);
		}
	}
	free(ptr); // Libera o vetor de ponteiros
}

// Funcao HASH Rotation ou Rolling, recebe a chave a ser codificada e o tamanho
int rot_hash(void *key, int len){
    unsigned char *p = key;
    int h = 0;
    int i;
    for (i = 0; i < len; i++){
        h = (h << 4) ^ (h >> 28) ^ p[i];
    }
    return abs(h);
}

// Funcoes de dispersao
int h1(int key, int size){  return key%size;  }
int h2(int key, int size){  return 1+(key%(size-1));  }

int f_hash(int key, int size, int i){  return (h1(key,size)+i)%size;  }
int s_hash(int key, int size, int i){  return ((h1(key,size)+i)*h2(key,size))%size;  }
int q_hash(int key, int size, int i){  return (h1(key,size)+i+(i*i))%size;  }

// Le do arquivo de entrada e retorna o valor da Key do Rotation Hash
int readInput (FILE *inputFile, char *input1, char *input2){
	if(fscanf(inputFile, "%s \"%s", input1, input2)>1){
		int tam = strlen(input2);
		tam--;
		input2[tam] = '\0'; // Limpa as " do final da string
		return rot_hash(input2, tam);
	}
	else{
		return -1;
	}
}

// Calcula e retorna a nova posicao apos um conflito
int collisionTreatment(int key, int size, int i, int cod){
	switch(cod){
		case LINEAR:
			return f_hash(key, size, i);
		case DUPLO:
			return s_hash(key, size, i);
		case QUADRATICA:
			return q_hash(key, size, i);
	}
}

// Insere na Hash
int insert (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	int i=0, aux = index;
	celHash *temp = ptr[aux], *temp2; // Guarda o valor da Head;
	do{
		if(cod == ENCADEAMENTO){
			while(ptr[aux]!=NULL){ // Percorre a Lista de forma linear pesquisando se a chave ja existe;
				if(strcmp(ptr[aux]->keyString, input)==0){
					fprintf(output, "INSERT \"%s\" %d %d %d 0 FAIL\n", input, key, index, aux);
					return 0;
				}
				temp2 = ptr[aux]; // Ponteiro posicao anterior
				ptr[aux] = ptr[aux]->prox;	// Ponteiro para proxima
				i=1; // Valor teste pra comparar as saidas depois
			}
			ptr[aux] = (celHash *) malloc (sizeof(celHash)); // Aloca a nova
			if(ptr[aux]==NULL){
				printf("ERROR: Null pointer!\n");
				exit(0);
			}
			strncpy(ptr[aux]->keyString, input, 101); // String para a nova celula
			ptr[aux]->prox = NULL; // Ponteiro da nova é nulo, inserida no fim da lista
			if(i==1){
				temp2->prox = ptr[aux]; // Anterior aponta para a nova
				ptr[aux] = temp;
			}
			if(output!=NULL){
				fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
				return 1;
			}
			else
				return 0;
		}
		else{ // ESTA PARTE SEGUE SOMENTE PARA OS DEMAIS CASOS, FICA MAIS FACIL DE VISUALIZAR AGORA ------------------------
			if (ptr[aux] == NULL){
				ptr[aux] = (celHash *) malloc (sizeof(celHash)); // Aloca a celula para inserir a chave
				if(ptr[aux]==NULL){
					printf("ERROR: Null pointer!\n");
					exit(0);
				}
				strncpy(ptr[aux]->keyString, input, 101); // String para a nova celula
				if(output!=NULL){ // Se a insercao vem do rehash nao e necessario imprimir a mensagem
					fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
					return 1;
				}
				else
					return 0;
			}
			else{
				if(strncmp(ptr[aux]->keyString,"\0", 1)==1){ // Caso string tenha sido deletada
					strncpy(ptr[aux]->keyString, input, 101);
					fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
					return 1;
				}
				else{
					if(strcmp(ptr[aux]->keyString,input)==0){ // Caso string ja esteja inserida na lista
						if(output!=NULL){ // Se a insercao vem do rehash nao e necessario imprimir a mensagem
							fprintf(output, "INSERT \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
						}
						return 0;
					}
					i++;
					aux = collisionTreatment(key, size, i, cod);
				}
			}
		}
	} while (1);
}

// Deleta na Hash
int delete (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	int i=0, aux = index;
	celHash *temp = ptr[aux], *temp2 = ptr[aux]; // Guarda a posicao incial
	while(ptr[aux]!=NULL){
		if (strcmp(input, ptr[aux]->keyString)==0){
			if(cod==ENCADEAMENTO){
				if(i=0){ // Caso encontre a chave na primeira posicao
					ptr[aux]=ptr[aux]->prox; // Head aponta para a proxima na lista;
					free(temp2); // Libera a primeira celula
				}
				else{ // Caso encontre a chave nas demais posicoes
					temp2->prox = ptr[aux]->prox; // Celula anterior aponta para a proxima celula
					temp2 = ptr[aux]; // Recebe a celula que sera deletada
					free(temp2);
				}
			}
			else
				strncpy(ptr[aux]->keyString, "\0", 1);

			fprintf(output, "DELETE \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
			ptr[aux] = temp; // Recupera a posicao inicial do vetor
			return 1;
		}
		else{
			i++;
			if(cod==ENCADEAMENTO){
				temp2 = ptr[aux]; // Salva a posicao anterior da celula que será comparada
				ptr[aux] = ptr[aux]->prox;	// Celula que sera comparada
			}
			else
				aux = collisionTreatment(key, size, i, cod);
		}
	}
	fprintf(output, "DELETE \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i); // Caso nao encontre celula para deletar
	ptr[aux] = temp; // Recupera a posicao inicial do vetor
	return -1;
}

// Busca na Hash por uma chave
void get (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	int i=0, aux = index;
	celHash* temp = ptr[aux]; // Salva a posicao inicial
	do {
		if(ptr[aux]==NULL){ // Se nenhuma chave foi inserida nesta posicao
			fprintf(output, "GET \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
			ptr[aux] = temp; // Recupera a posicao inicial do vetor
			return;
		}
		else{
			if(strcmp(input, ptr[aux]->keyString)==0){ // Se a chave esta nesta posicao
				fprintf(output, "GET \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
				ptr[aux] = temp; // Recupera a posicao inicial do vetor
				return;
			}
			else{
				i++;
				if(cod==ENCADEAMENTO) // Caso encadeado, percorre a lista
					ptr[aux] = ptr[aux]->prox;
				else
					aux = collisionTreatment(key, size, i, cod);
			}
		}
	} while(1);
}
