#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENCADEAMENTO 0
#define LINEAR       1
#define QUADRATICA   2
#define DUPLO        3
#define L_FACTOR     0.75
#define INI_SIZE     500

// Celula Hash
struct Node{
	char keyString[101];  // Guarda a String lida
	struct Node *prox;    // Usado no tratamento por encadeamento
	//int collisionFlag;    // Flag se a celula sofreu alguma colisao
}; typedef struct Node celHash;




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

// Inicia a Hash
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

// Libera a Hash
void destroyHash(celHash **ptr, int size){
	int i;
	for (i=0; i<size; i++){
		free(ptr[i]);
	}
	free(ptr);
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

// Funcoes de dispersao hash primaria e secundaria
int f_hash(int key, int size, int i){	return (key+i)%size;  }
int s_hash(int key, int size, int i){	return 1+f_hash(key, size-1, i);  }

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

int collisionTreatment(int key, int size, int i, int cod){
	switch(cod){
		case LINEAR:
			return f_hash(key, size, i);
		case DUPLO:
			return s_hash(key, size, i);
		case QUADRATICA:
			return 0;
		case ENCADEAMENTO:
			return 0;
	}
}

// Insere na Hash
int insert (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	int i=0, aux = index;
	do{
		if (ptr[aux] == NULL){
			ptr[aux] = (celHash *) malloc (sizeof(celHash));
			if(ptr[aux]==NULL){
				printf("ERROR: Null pointer!\n");
				exit(0);
			}
			strncpy(ptr[aux]->keyString, input, 101);
			if(output!=NULL){
				fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", ptr[aux]->keyString, key, index, aux, i);
				return 1;
			}
			else{
				return 0;
			}
		}
		else{
			if(strncmp(ptr[aux]->keyString,"\0", 1)==1){
				printf("entro\n");
				strncpy(ptr[aux]->keyString, input, 101);
				fprintf(output, "INSERT \"%s\" %d %d %d %d SUCCESS\n", ptr[aux]->keyString, key, index, aux, i);
				return 1;
			}
			else{
				i++;
				aux = collisionTreatment(index, size, i, cod);
				if (strcmp(ptr[aux]->keyString,input)==0){ // Caso string ja esteja inserida na lista
					if(output!=NULL){
						fprintf(output, "INSERT \"%s\" %d %d %d %d FAIL\n", ptr[aux]->keyString, key, index, aux, i-1);
					}
					return 0;
				}
			}
		}
	} while (1);
}

int delete (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	/*int i=0, aux = index;
	celHash *temp;
	do{
		if (strcmp(input, ptr[aux]->keyString)==0){
			fprintf(output, "DELETE \"%s\" %d %d %d %d SUCCESS\n", ptr[aux]->keyString, key, index, aux, i);
			temp = ptr[aux];
			ptr[aux] = NULL;
			free(temp);
			return 1;
		}
		else{
			i++;
			aux = collisionTreatment(index, size, i, cod);
		}
	} while(ptr[aux]->collisionFlag==1);
	fprintf(output, "DELETE \"%s\" %d %d %d %d FAIL\n", ptr[aux]->keyString, key, index, aux, i);
	return -1;*/
}

void get (celHash **ptr, int size, char *input, int key, int index, int cod, FILE *output){
	int i=0, aux = index;
	do {
		if(ptr[aux]==NULL){
			fprintf(output, "GET \"%s\" %d %d %d %d FAIL\n", ptr[aux]->keyString, key, index, aux, i);
			return;
		}
		else{
			if(strcmp(input, ptr[aux]->keyString)==0){
				fprintf(output, "GET \"%s\" %d %d %d %d SUCCESS\n", ptr[aux]->keyString, key, index, aux, i);
				return;
			}
			else{
				i++;
				aux = collisionTreatment(index, size, i, cod);
			}
		}
	} while(1);
}

// Re-hash dobrando o tamanho do vetor, retorna ponteiro para a nova lista
celHash **rehash (celHash **ptr, int *size, int cod){
	celHash **newHash = startHash(*size*2);
	int i, key, index;
	for (i=0; i<*size; i++){
		if (ptr[i]!=NULL){
			key = rot_hash(ptr[i]->keyString, strlen(ptr[i]->keyString));  // Calcula a chave
			index = f_hash(key, *size*2, 0);  // Calcula a Funcao Hash Inicial
			insert(newHash, *size*2, ptr[i]->keyString, key, index, cod, NULL); // Insere na Nova hash
		}
	}
	destroyHash(ptr, *size);  // Libera Hash antiga
	*size = *size*2;
	printf("%d\n" , *size);
	return newHash;
}

void Hash (FILE *inputFile, FILE *outputFile, int cod){
	char input1[7], input2[100];
	int key, index, hashSize = INI_SIZE, loadHash = 0;
	celHash **head = startHash(hashSize);
	
	while (!feof(inputFile)){
		key = readInput(inputFile, input1, input2);

		if (key >= 0){
			index = f_hash(key, hashSize, 0);

			if(strcmp(input1, "INSERT") == 0){
				if(insert(head, hashSize, input2, key, index, cod, outputFile)==1){
					loadHash++;
					if((loadHash/hashSize)>=L_FACTOR)
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
					if(strcmp(input1, "GET") == 0){
						get(head, hashSize, input2, key, index, cod, outputFile);
					}
					else{
						printf("\nERROR: input1 contain a invalid command .. Program closed!\n");
						exit(-1);
					}
				}
			}
		}
	}
	destroyHash(head, hashSize);
}


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