#include <stdio.h>
#include <stdlib.h>
#define BUCKET_SIZE 5

void createBuckets(int ***buckets, int num);
void generateValues(int **values, int size, int range);
void populateBuckets(int ***buckets, int numBuckets, int *values, int numValues);
void printBuckets(int **buckets, int num);
void sort(int ***buckets, int num);
void cleanBuckets(int **buckets, int num);

int main(int argc, char *argv[]){
	if(argc < 4){
		printf("Numero de argumentos insuficiente\n");
		return 1;
	}

	int numBuckets;
	sscanf(argv[1], "%d", &numBuckets);

	int **buckets;
	createBuckets(&buckets, numBuckets);

	//Range para geração de numeros aleatorios
	int rangeNum;
	sscanf(argv[2], "%d", &rangeNum);

	//Quantidade de valores
	int valuesSize, *values;
	sscanf(argv[3], "%d", &valuesSize);	

	//Gera os valores randomicos para serem divididos nos buckets	
	generateValues(&values, valuesSize, rangeNum);

	//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
	populateBuckets(&buckets, numBuckets, values, valuesSize);
	printBuckets(buckets, numBuckets);

	sort(&buckets, numBuckets);
	printBuckets(buckets, numBuckets);
	//Limpa tuto
	cleanBuckets(buckets, numBuckets);
	free(buckets);
	free(values);
	return 0;
}

void createBuckets(int ***buckets, int num){
	int i, **temp_buckets;
	temp_buckets = malloc(num * sizeof(int*));
	for(i=0; i < num; i++){
		temp_buckets[i] = malloc(sizeof(int) * BUCKET_SIZE);	
		temp_buckets[i][0] = 0;
	}	
	*buckets = temp_buckets;
}

void generateValues(int **values, int size, int range){
	int i, *temp =  malloc(size *sizeof(int));
	for(i=0; i < size; i++){
		//Gera valores entre 0 e o range dos numeros
		//Ex.: para range = 50 -> temp[i] vai receber valores entre 0 e 50(50 não incluso)
		temp[i] = rand()%range;
	}
	*values=temp;
}

void populateBuckets(int ***buckets, int numBuckets, int *values, int numValues){
	int i, j, **temp = *buckets, id;
	for(i=0;i < numValues;i++){
		//TODO: definir um jeito melhor de decidir qual bucket recebe qual numero
		id = values[i] / numBuckets;
		//Atualiza a contagem dos elementos do bucket
		temp[id][0]++;
		//Insere no bucket	
		temp[id][temp[id][0]] = values[i];
	}
	*buckets= temp;
}

void printBuckets(int **buckets, int num){
	int i, j;
	for(i=0; i < num; i++){
		printf("Bucket %d, com size %d: ", i, buckets[i][0]);
		for(j=1; j < BUCKET_SIZE; j++){
			printf("%d ", buckets[i][j]);
		}
		printf("\n");
	}
}

void sort(int ***buckets, int num){
	int i, j, k, ** bucket = *buckets;
	for(i=0; i<num; i++){
		int *tempB = (int*) bucket[i];
		for(j=1;j<tempB[0]+1;j++){
			for(k=j+1;k<tempB[0]+1;k++){
				if(tempB[j] > tempB[k]){
					int temp = tempB[j];
					tempB[j] = tempB[k];
					tempB[k] = temp;
				}
			}
		}
		bucket[i] = tempB;
	}
	*buckets = bucket;
}


void cleanBuckets(int **buckets, int num){
	int i;
	for(i=0; i<num; i++){
		free(buckets[i]);
	}
}
