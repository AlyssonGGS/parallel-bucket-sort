#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

void createBuckets(int ***buckets, int num, int bucketSize);
void generateValues(int **values, int *count);
void printValues(int *values, int num);
void populateBuckets(int ***buckets, int numBuckets, int *values, int count);
void printBuckets(int **buckets, int num, int bucketSize);
void sort(int ***buckets, int num);
void sort_p(int ***buckets, int id, int num);
int* mergeBuckets(int **buckets, int num, int valuesSize);
void cleanBuckets(int **buckets, int num);

void master(int ***buckets, int num);
void worker(int rank);

int main(int argc, char *argv[]){
	clock_t start, end;
	start = clock();
	
	if(argc < 4){
		printf("Numero de argumentos insuficiente\n");
		return 1;
	}
	
	int numBuckets;
	sscanf(argv[1], "%d", &numBuckets);

	int bucketSize;
	//sscanf(argv[2], "%d", &bucketSize);
	int range;
	sscanf(argv[2], "%d", &range);
	bucketSize = range/numBuckets;
	//printf("range: %d\nnumBuckets: %d\nbucketSize: %d\n",range,numBuckets,bucketSize);
	if (range%numBuckets!=0) bucketSize++;
	
	int nthreads;
	sscanf(argv[3], "%d", &nthreads);
	

	int **buckets;
	createBuckets(&buckets, numBuckets, bucketSize);

	int *values, count;
	//Gera os valores randomicos para serem divididos nos buckets	
	generateValues(&values, &count);
	

	//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
	populateBuckets(&buckets, bucketSize , values, count);
	
	//Ordena
	omp_set_num_threads(nthreads);
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int nthreads = omp_get_num_threads();
		sort_p(&buckets, id ,numBuckets/nthreads);
	}
	//printBuckets(buckets, numBuckets, bucketSize);

	//Junta nos values de novo
	free(values);
	values = mergeBuckets(buckets, numBuckets, count);
	printf("Ordenado: ");
	printValues(values, count);

	//Limpa tuto
	cleanBuckets(buckets, numBuckets);
	free(buckets);
	free(values);
	
	end = clock();
	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Elapsed Time: %lf",cpu_time_used);
	
	return 0;
}

void createBuckets(int ***buckets, int num, int bucketSize){
	int i, **temp_buckets;
	temp_buckets = malloc(num * sizeof(int*));
	for(i=0; i < num; i++){
		temp_buckets[i] = malloc(sizeof(int) * bucketSize);	
		temp_buckets[i][0] = 0;
	}	
	*buckets = temp_buckets;
}

void generateValues(int **values, int *count_values){
	FILE *file;
	if((file = fopen("nums", "r")) != NULL){
		char *line = NULL;
		size_t len = 0;
		getline(&line, &len, file);
		//Cria o array dos numeros
		char *tempS = line;
		int count = 0;
		while(*tempS){
			if(*tempS++ == ' ') count++;
		}
		//printf("%d\n", count);
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

void populateBuckets(int ***buckets, int tamBuckets, int *values, int count){
	int i, j, **temp = *buckets, id;
	
	for(i=0;i < count;i++){
		//TODO: definir um jeito melhor de decidir qual bucket recebe qual numero
		id = values[i] / tamBuckets;
		
		//printf("values %d\n",values[i]);
		//printf("tamanho bucket %d\n",tamBuckets);
		//printf("%d\n",id);
		
		//Atualiza a contagem dos elementos do bucket
		temp[id][0]++;
		//Insere no bucket	
		temp[id][temp[id][0]] = values[i];
		
		
	}
	*buckets= temp;
	
}

void printBuckets(int **buckets, int num, int bucketSize){
	int i, j;
	for(i=0; i < num; i++){
		printf("Bucket %d, com size %d: ", i, buckets[i][0]);
		for(j=1; j < bucketSize; j++){
			printf("%d ", buckets[i][j]);
		}
		printf("\n");
	}
}

void sort_p(int ***buckets, int id, int num){
	int i, j, k, ** bucket = *buckets;
	
	
	printf("id dele:%d\n",id);
	printf("num %d\n",num);
	for(i= 0; i< num; i++){
		
		int *tempB = (int*) bucket[(id*num)+i];
		
		for(j=1;j<tempB[0]+1;j++){
			for(k=j+1;k<tempB[0]+1;k++){
				if(tempB[j] > tempB[k]){
					int temp = tempB[j];
					tempB[j] = tempB[k];
					tempB[k] = temp;
				}
			}
		}
		bucket[(id*num)+i] = tempB;
	}
	*buckets = bucket;
	
}

/*
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
			} the remote repository: git push -u origin feature_branch_nam
		}
		bucket[i] = tempB;
	}
	*buckets = bucket;
}
*/

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
