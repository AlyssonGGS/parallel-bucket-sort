#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

/*

mpicc -o trab bucket.c & scp ./trab lab@L307-1A:/home/lab/lab/
mpirun --hostfile host -np 2 -mca plm_rsh_no_tree_spawn 1 trab 3 50
mpicc -o trab bucket.c 

*/


void createBuckets(int ***buckets, int num, int bucketSize);
void generateValues(int **values, int *count);
void printValues(int *values, int num);
void populateBuckets(int ***buckets, int numBuckets, int *values, int count);
void printBuckets(int **buckets, int num, int bucketSize);
int* bubbleSort(int *bucket);
void sort(int ***buckets, int num);
int* mergeBuckets(int **buckets, int num, int valuesSize);
void cleanBuckets(int **buckets, int num);

void master(int ***buckets, int num, int bucketSize);
void worker(int rank);

int main(int argc, char *argv[]){
	MPI_Init(&argc,&argv);
	if(argc < 2){
		printf("Numero de argumentos insuficiente\n");
		MPI_Finalize();
		return 1;
	}
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	//Define o ranking para distribuir os buckets
	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	if(myrank != 0){
		worker(myrank);
	} else{
		int bucketSize;
		sscanf(argv[1], "%d", &bucketSize);

		int **buckets;
		createBuckets(&buckets, size, bucketSize);
	
		int *values, count;
		//Gera os valores randomicos para serem divididos nos buckets	
		generateValues(&values, &count);
		printValues(values, count);
		printf("\ncount = %d\n", count);
		//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
		populateBuckets(&buckets, size, values, count);
		printBuckets(buckets, size, bucketSize);
		
		master(&buckets, size, bucketSize);
		//cleanBuckets(buckets, size);
		//free(buckets);
	}
	
	/*
	//Ordena
	sort(&buckets, numBuckets);
	printBuckets(buckets, numBuckets, bucketSize);

	//Junta nos values de novo
	free(values);
	values = mergeBuckets(buckets, numBuckets, count);
	printf("Ordenado: ");
	printValues(values, count);
	
	//Limpa tuto
	cleanBuckets(buckets, numBuckets);
	free(buckets);
	free(values);
	*/
	//Encerra o MPI
	MPI_Finalize();	
	return 0;
}

void master(int ***buckets, int size, int bucketSize){
	int **aux = *buckets;
 	int i;
	MPI_Request request;
 	for(i=1; i < size; i++){
 		MPI_Send(&bucketSize, 1, MPI_INT, i, i, MPI_COMM_WORLD);
 		int * bucket = aux[i];
 		MPI_Send(bucket, bucketSize, MPI_INT, i, i, MPI_COMM_WORLD);
 	}
 	aux[0] = bubbleSort(aux[0]);
 	MPI_Status status;
 	for(i=1; i < size; i++){
 		MPI_Recv(aux[i], bucketSize, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
 	}
 	
 	//printBuckets(aux, size, 0);
 	int * values = mergeBuckets(aux, size, 10);
 	printValues(values, 10);
 	//free(values);
}

void worker(int rank){
	int num;
	MPI_Status status;
	MPI_Recv(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	int bucket[num];
	MPI_Recv(&bucket, num, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	memcpy(bucket, bubbleSort(bucket), num);
	
	/*voltar pro master*/
	MPI_Send(bucket, num, MPI_INT, 0, 0, MPI_COMM_WORLD);
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

void populateBuckets(int ***buckets, int numBuckets, int *values, int count){
	int i, j, **temp = *buckets, id;
	for(i=0;i < count;i++){
		//TODO: definir um jeito melhor de decidir qual bucket recebe qual numero
		id = values[i] / numBuckets;
		id %= numBuckets;
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
		printf("num: %d\n", num);
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
