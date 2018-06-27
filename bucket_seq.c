#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void createBuckets(int ***buckets, int num, int bucketSize);
void generateValues(int **values, int *count);
void printValues(int *values, int num);
void populateBuckets(int ***buckets, int numBuckets, int *values, int count);
void printBuckets(int **buckets, int num, int bucketSize);
int* bubbleSort(int *bucket);
void sort(int ***buckets, int num);
int* mergeBuckets(int **buckets, int num, int valuesSize);
void cleanBuckets(int **buckets, int num);

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Numero de argumentos insuficiente\n");
		return 1;
	}
	//Guarda o tamanho do bucket
	int bucketSize;
	sscanf(argv[1], "%d", &bucketSize);

	int bucketCount;
	sscanf(argv[2], "%d", &bucketCount);

	int **buckets;
	createBuckets(&buckets, bucketCount, bucketSize);

	int *values, count;
	clock_t t;
    t = clock();
	//Gera os valores randomicos para serem divididos nos buckets	
	generateValues(&values, &count);
	printValues(values, count);
	//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
	populateBuckets(&buckets, bucketSize, values, count);
	printBuckets(buckets, bucketCount, bucketSize);
	sort(&buckets, bucketCount);
	values = mergeBuckets(buckets, bucketCount, count);
	printValues(values, count);
	t = clock();
	double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
	printf("Elapsed time: %.7lf \n", time_taken);
	free(values);
	cleanBuckets(buckets, bucketCount);
	free(buckets);
	return 0;
}

void createBuckets(int ***buckets, int num, int bucketSize){
	int i, j, **temp_buckets;
	//Cria a matriz de buckets
	temp_buckets = malloc(sizeof(int*) * num);
	for(i=0; i < num; i++){
		//Aloca um array que representa o bucket
		//Soma 1 pois o indice 0 é usado para guardar o numero de elementos no bucket
		temp_buckets[i] = malloc(sizeof(int) * (bucketSize + 1));
		for(j=0; j < bucketSize; j++){
			temp_buckets[i][j] = 0;
		}
	}	
	*buckets = temp_buckets;
}


void generateValues(int **values, int *count_values){
	FILE *file;
	if((file = fopen("nums", "r")) != NULL){
		char *line = NULL;
		size_t len = 0;
		getline(&line, &len, file);
				getline(&line, &len, file);
	
		//Cria o array dos numeros
		char *tempS = line;
		int count = 0;
		while(*tempS){
			if(*tempS++ == ' ') count++;
		}
		*count_values = count;
		int *temp =  malloc(count * sizeof(int));
		char *token;
		token = strtok(line, " ");
		count = 0;
		while( token != NULL ) {
			sscanf(token, "%d", &temp[count]);			
			token = strtok(NULL, " ");
			count++;
		}
		*values = temp;
	}
}

void printValues(int *values, int num) {
	int i;
	for(i=0;i<num;i++){
		printf("%d>", values[i]);
	}
	printf("\n");
}

void populateBuckets(int ***buckets, int bucketSize, int *values, int count){
	int i, j, **temp = *buckets, id;
	for(i=0;i < count;i++){
		//TODO: definir um jeito melhor de decidir qual bucket recebe qual numero
		//Ex: Elemento = 120 e bucket size 50 para 4 buckets
		//Id = 120/50 = 2 com resto 20
		//Ou seja, vai parar no 3º bucket(indice 2)
		id = values[i]/bucketSize;
		//Atualiza a contagem dos elementos do bucket
		//Insere no bucket	
		temp[id][++temp[id][0]] = values[i];
	}
	*buckets= temp;
}

void printBuckets(int **buckets, int num, int bucketSize){
	int i, j;
	for(i=0; i < num; i++){
		printf("Bucket %d, com size %d: ", i, buckets[i][0]);
		for(j=1; j <= buckets[i][0]; j++){
			printf("%d ", buckets[i][j]);
		}
		printf("\n");
	}
}

int* bubbleSort(int *bucket) {
	int j, k;
	for(j=1;j < bucket[0]+1;j++){
		for(k=j+1;k<bucket[0]+1;k++){ 
			if(bucket[j] > bucket[k]){
				int temp = bucket[j];
				bucket[j] = bucket[k];
				bucket[k] = temp;
			}
		}
	}
	return bucket;
}

void sort(int ***buckets, int num){
	int i, **bucket = *buckets;
	for(i=0; i<num; i++){
		bucket[i] = bubbleSort(bucket[i]);
	}
	*buckets = bucket;
}

int* mergeBuckets(int **buckets, int num, int size){
	int *result = malloc(size * sizeof(int));
	int i, j, count = 0;
	for(i=0;i<num;i++){
		for(j=1;j<buckets[i][0]+1;j++){
			result[count++] = buckets[i][j];
		}
	}
	return result;
}


void cleanBuckets(int **buckets, int num){
	int i;
	for(i=0; i<num; i++){
		free(buckets[i]);
	}
}
