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

void master(int ***buckets, int num, int bucketSize, int bucketPerProcess, int count);
void worker(int rank);

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);

	if(argc < 3){
		printf("Numero de argumentos insuficiente\n");
		MPI_Finalize();
		return 1;
	}

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//Guarda o tamanho do bucket
	int bucketSize;
	sscanf(argv[1], "%d", &bucketSize);

	int bucketCount;
	sscanf(argv[2], "%d", &bucketCount);

	int bucketPerProcess;
	sscanf(argv[3], "%d", &bucketPerProcess);

	if(bucketPerProcess * size != bucketCount){
		printf("Organização dos buckets errada\n");
		MPI_Finalize();	
		return 1;
	}

	//Define o ranking para distribuir os buckets
	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	if(myrank != 0){
		worker(myrank);
	} else {
		int **buckets;
		createBuckets(&buckets, bucketCount, bucketSize);
	
		int *values, count;
		//Gera os valores randomicos para serem divididos nos buckets	
		generateValues(&values, &count);
		printValues(values, count);
		//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
		populateBuckets(&buckets, bucketSize * bucketCount, values, count);
		printBuckets(buckets, bucketCount, bucketSize);
		
		master(&buckets, size, bucketSize, bucketPerProcess, count);
		cleanBuckets(buckets, bucketCount);
		free(buckets);
		free(values);
	}
	MPI_Finalize();	
	printf("Lelecsu:%d\n", myrank);
}

void master(int ***buckets, int size, int bucketSize, int bucketPerProcess, int count){
	int **aux = *buckets;
 	int i, j;
 	for(i=1; i < size; i++){
		//Envia primeiro a quantidade de elementos para criação do array
 		MPI_Send(&bucketPerProcess, 1, MPI_INT, i, i, MPI_COMM_WORLD);
		//Envia primeiro a quantidade de elementos para criação do array
 		MPI_Send(&bucketSize, 1, MPI_INT, i, i, MPI_COMM_WORLD);
		//Envia o array para o worker
		for(j=0; j<bucketPerProcess; j++){
 			MPI_Send(&(aux[(i * bucketPerProcess)+j][0]), bucketSize, MPI_INT, i, i, MPI_COMM_WORLD);
		}
 	}
	//Ordena o proprio array	
	for(i = 0; i < bucketPerProcess; i++){
 		aux[i] = bubbleSort(aux[i]);
	}
	//Recebe os outros arrays dos workers
 	MPI_Status status;
 	for(i=1; i < size; i++){
		 for(j=0; j<bucketPerProcess; j++){
			MPI_Recv(aux[(i * bucketPerProcess)+j], bucketSize, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			printf("VRAU: %d\n", aux[(i * bucketPerProcess)+j][0]);
		}
 	}
 	printValues(mergeBuckets(aux, size*bucketPerProcess, count), count);
}

void worker(int rank){
	int qtd, num;
	MPI_Status status;
	//Recebe o tamanho do bucket
	MPI_Recv(&qtd, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	//Recebe o tamanho do bucket
	MPI_Recv(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	//Recebe o bucket
	int **bucket, i;

	bucket = malloc(sizeof(int*) * qtd);
	for(i = 0; i < qtd; i++){
		bucket[i] = malloc(sizeof(int) * num);
	}
	
	for(i=0; i< qtd; i++){
		MPI_Recv(bucket[i], num, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	}
	//Ordena o array
	sort(&bucket, qtd);
	for(i = 0; i < qtd; i++){
		MPI_Send(bucket[i], num, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	free(bucket);
}

void createBuckets(int ***buckets, int num, int bucketSize){
	int i, j, **temp_buckets;
	//Cria a matriz de buckets
	temp_buckets = malloc(sizeof(int*) * num);
	for(i=0; i < num; i++){
		//Aloca um array que representa o bucket
		//Soma 1 pois o indice 0 é usado para guardar o numero de elementos no bucket
		temp_buckets[i] = malloc(sizeof(int) * (bucketSize + 1));
		for(j=0; j < bucketSize + 1; j++) {
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
		if(bucket[i][0] != 0)
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
