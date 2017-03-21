// Trabalho Pratico 1 - Algoritmo e Estrutura de Dados II - 23/02/2017
// Autores - Paulo Henrik Goncalves & Gilberto Kreisler

// Tabela Hash com tratamento de conflito:
// <treatment>:  -linear | -hash_duplo | -quadratica | -encadeamento
// execution: ./exec <treatment> <input_file>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ENCADEAMENTO 0
#define LINEAR       1
#define QUADRATICA   2
#define DUPLO        3
#define L_FACTOR     0.75000f
#define INI_SIZE     500

int totalCollisions=0;


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
int q_hash(int key, int size, int i);
int quad_hash(int key, int size, int i);
int readInput (FILE *inputFile, char *input1, char *input2);
int collisionTreatment(int key, int size, int i, int cod);
int insert (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output);
int delete (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output);
void get (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output);
void get_chain(celHash **ptr, char *input, int key, int index, FILE *output);
int insert_chain(celHash **ptr, char *input, int key, int index, FILE *output);
int delete_chain(celHash **ptr, char *input, int key, int index, FILE *output);

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
	int key, index, hashSize = INI_SIZE, loadHash = 0, aux;
	celHash **head = startHash(hashSize);  // Inicia o vetor de ponteiros
	clock_t start, end; // Variaveis para printar tempo de exucução das func insert - get - delete
	double totalInsert=0, totalGet=0, totalDelete=0;

	while (!feof(inputFile)){
		key = readInput(inputFile, input1, input2); // Le as entradas e calcula a key

		if (key >= 0){

			switch(cod){
				case DUPLO:
					index = s_hash(key, hashSize, 0);  // Calcula a Funcao Hash Inicial
					break;
				case QUADRATICA:
					index = q_hash(key, hashSize, 0);  // Calcula a Funcao Hash Inicial
					break;
				default:
					index = f_hash(key, hashSize, 0);  // Calcula a Funcao Hash Inicial
					break;
			}

			if(strcmp(input1, "INSERT") == 0){
				start = clock();
				aux = insert(head, hashSize, input2, key, index, cod, outputFile);
				end = clock();
				if(aux==1){
					loadHash++;
					if(((float)loadHash/(float)hashSize)>=L_FACTOR)
					head = rehash(head, &hashSize, cod);
				}
				totalInsert += (double)(end - start)/CLOCKS_PER_SEC;
			}
			else{
				if(strcmp(input1, "DELETE") == 0){
					start = clock();
					if(delete(head, hashSize, input2, key, index, cod, outputFile)==1){
						loadHash--;
					}
					end = clock();
					totalDelete += (double)(end - start)/CLOCKS_PER_SEC;
				}
				else{
					if(strcmp(input1, "GET") == 0){
					start = clock();
					get(head, hashSize, input2, key, index, cod, outputFile);
					end = clock();
					totalGet += (double)(end - start)/CLOCKS_PER_SEC;
				}

					else{
						printf("\nERROR: input1 contain a invalid command .. Program closed!\n");
						exit(-1);
					}
				}
			}
		}
	}
	fprintf(outputFile, "%d", totalCollisions);
	printf("\nINSERT TIME: %7.6lf\nDELETE TIME: %7.6lf\nGET TIME:    %7.6lf\n\n", totalInsert, totalDelete,totalGet);
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
	for(i=0; i<size; i++){
		ptr[i] = NULL;
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

		if (cod == ENCADEAMENTO) {
			while(temp != NULL){
				key = rot_hash(temp->keyString, strlen(temp->keyString));  // Calcula a chave
				index = f_hash(key, *size*2, 0);  // Calcula a Funcao Hash Inicial
				insert(newHash, *size*2, temp->keyString, key, index, cod, NULL); // Insere na Nova hash
				temp = temp->prox; // Caso a seja o modo encadeado, a lista percorre ate o final
			}
		}

		else{
			if(temp!=NULL){
				if(strncmp(temp->keyString, "\0", 1) != 0){
					key = rot_hash(temp->keyString, strlen(temp->keyString));  // Calcula a chave
					switch(cod){
						case DUPLO:
							index = s_hash(key, *size*2, 0);  // Calcula a Funcao Hash Inicial
							break;
						case QUADRATICA:
							index = q_hash(key, *size*2, 0);  // Calcula a Funcao Hash Inicial
							break;
						default:
							index = f_hash(key, *size*2, 0);  // Calcula a Funcao Hash Inicial
							break;
					}

					insert(newHash, *size*2, temp->keyString, key, index, cod, NULL); // Insere na Nova hash
				}
			}
		}
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
int s_hash(int key, int size, int i){  return (h1(key,size)+(i*h2(key,size)))%size;  }
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

	//Tratando o encadeamento separadamente - ajuda na abstração
	if (cod == ENCADEAMENTO){
		return insert_chain(ptr, input, key, index, output);
	}

	// ESTA PARTE SEGUE SOMENTE PARA OS DEMAIS CASOS, FICA MAIS FACIL DE VISUALIZAR AGORA ------------------------
	else{
		do{
			if (ptr[aux] == NULL){
				ptr[aux] = (celHash *) malloc (sizeof(celHash)); // Aloca a celula para inserir a chave
				if(ptr[aux]==NULL){
					printf("ERROR: Null pointer!\n");
					exit(0);
				}
				strncpy(ptr[aux]->keyString, input, 101); // String para a nova celula
				if(output!=NULL){ // Se a insercao vem do rehash nao e necessario imprimir a mensagem
					fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
					totalCollisions+=i;
					return 1;
				}
				else
					return 0;
			}
			else{
				if(strncmp(ptr[aux]->keyString,"\0", 1)==0){ // Caso string tenha sido deletada
					strncpy(ptr[aux]->keyString, input, 101);
					fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
					totalCollisions+=i;
					return 1;
				}
				else{
					if(strcmp(ptr[aux]->keyString,input)==0){ // Caso string ja esteja inserida na lista
						fprintf(output, "INSERT \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
						totalCollisions+=i;
						return 0;
					}
				}
			}
			i++;
			aux = collisionTreatment(key, size, i, cod);
		}while (1);
	}
}


int insert_chain(celHash **ptr, char *input, int key, int index, FILE *output){
	int i=0, aux = index;
	celHash *temp = ptr[aux], *temp2, *temp3;

	if (ptr[aux] == NULL) {
		ptr[aux] = (celHash*) malloc (sizeof(celHash));
		if(ptr[aux] == NULL){
			printf("ERROR: Null pointer!\n");
			exit(0);
		}

		strncpy(ptr[aux]->keyString, input, 101);
		ptr[aux]->prox = NULL;

		if(output != NULL){
			fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
			totalCollisions+=i;
			return 1;
		}
		else
			return 0;
	}
	else{
		do{
			if (strcmp(temp->keyString, input) == 0) {
				if(output != NULL){
					fprintf(output, "INSERT \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
				}
				return 0;
			}

			if (i<1)
				i++;

			temp2 = temp;
			temp = temp->prox;

		} while(temp != NULL);

		temp3 = (celHash*) malloc (sizeof(celHash));
		if(temp3 == NULL){
			printf("ERROR: Null pointer!\n");
			exit(0);
		}

		strncpy(temp3->keyString, input, 101);

		temp2->prox = temp3;
		temp3->prox = temp;

		if(output!=NULL){
			fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
			totalCollisions+=i;
			return 1;
		}
		else
		return 0;
	}
}

// Deleta na Hash
int delete (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	int i=0, aux = index;

	if (cod == ENCADEAMENTO) {
		return delete_chain(ptr, input, key, index, output);
	}

	else{
		while(ptr[aux] != NULL){
			if (strcmp(input, ptr[aux]->keyString)==0){
				strncpy(ptr[aux]->keyString, "\0", 1);
				fprintf(output, "DELETE \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
				totalCollisions+=i;
				return 1;
			}

			else{
				i++;
				aux = collisionTreatment(key, size, i, cod);
			}
		}
		fprintf(output, "DELETE \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i); // Caso nao encontre celula para deletar
		totalCollisions+=i;
		return -1;
	}
}

int delete_chain(celHash **ptr, char *input, int key, int index, FILE *output){
	int i=0, aux = index;
	celHash *temp = ptr[aux], *temp2=NULL; // Guarda a posicao incial

	if (ptr[aux] == NULL) {
		if(output != NULL){
			fprintf(output, "DELETE \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
		}
		return 0;
	}

	else{
		do{
			if (strcmp(temp->keyString, input) == 0) {
				if(i==0){
					ptr[aux]=NULL;
					free(temp);
				}
				else{
					temp2->prox=temp->prox;
					free(temp2);
				}

				fprintf(output, "DELETE \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
				totalCollisions+=i;
				return 1;
			}

			temp2 = temp;
			temp = temp->prox;

			if(i < 1)
				i++;

		} while(temp != NULL);

		fprintf(output, "DELETE \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
		return 0;
	}

}

// Busca na Hash por uma chave
void get (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	int i=0, aux = index;

	if (cod == ENCADEAMENTO) {
		get_chain(ptr, input, key, index, output);
	}
	else{
		do {
			if(ptr[aux] == NULL){ // Se nenhuma chave foi inserida nesta posicao
				fprintf(output, "GET \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
				totalCollisions+=i;
				return;
			}
			else{
				if(strcmp(input, ptr[aux]->keyString)==0){ // Se a chave esta nesta posicao
					fprintf(output, "GET \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
					totalCollisions+=i;
					return;
				}
				else{
					i++;
					aux = collisionTreatment(key, size, i, cod);
				}
			}
		} while(1);
	}
}

void get_chain(celHash **ptr, char *input, int key, int index, FILE *output){
	int i = 0, aux = index;
	celHash* temp = ptr[aux];

	while(temp!=NULL){
		if (strcmp(input, temp->keyString) == 0) {
			fprintf(output, "GET \"%s\" %d %d %d %d SUCCESS\n", input, key, index, aux, i);
			totalCollisions+=i;
			return;
		}
		else{
			temp = temp->prox;
			if(i < 1)
				i++;
		}
	}
	fprintf(output, "GET \"%s\" %d %d %d %d FAIL\n", input, key, index, aux, i);
}
