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
	//Define o ranking para distribuir os buckets
	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	if(myrank != 0){
		worker(myrank);
	} else{
		int bucketCount;
		MPI_Comm_size(MPI_COMM_WORLD, &bucketCount);
	
		//Guarda o tamanho do bucket
		int bucketSize;
		sscanf(argv[1], "%d", &bucketSize);

		int **buckets;
		createBuckets(&buckets, bucketCount, bucketSize);
	
		int *values, count;
		//Gera os valores randomicos para serem divididos nos buckets	
		generateValues(&values, &count);
		printValues(values, count);
		printf("\ncount = %d\n", count);
		//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
		populateBuckets(&buckets, bucketSize, values, count);
		printBuckets(buckets, bucketCount, bucketSize);
		
		master(&buckets, bucketCount, bucketSize);
		cleanBuckets(buckets, size);
		free(buckets);
	}
	
	MPI_Finalize();	
	return 0;
}

void master(int ***buckets, int bucketCount, int bucketSize){
	int **aux = *buckets;
 	int i;
 	for(i=1; i < bucketCount; i++){
		//Envia primeiro a quantidade de elementos para criação do array
 		MPI_Send(&bucketSize, 1, MPI_INT, i, i, MPI_COMM_WORLD);
		//Envia o array para o worker
 		MPI_Send(aux[i], bucketSize, MPI_INT, i, i, MPI_COMM_WORLD);
 	}
	//Ordena o proprio array	
 	aux[0] = bubbleSort(aux[0]);
	//Recebe os outros arrays dos workers
 	MPI_Status status;
 	for(i=1; i < bucketCount; i++){
 		MPI_Recv(aux[i], bucketSize, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
 	}
 	int * values = mergeBuckets(aux, bucketCount, 10);
 	// printValues(values, 10);
	free(values);
}

void worker(int rank){
	int num;
	MPI_Status status;
	//Recebe o tamanho do bucket
	MPI_Recv(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	//Recebe o bucket
	int bucket[num];
	MPI_Recv(&bucket, num, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	//Ordena o array
	memcpy(bucket, bubbleSort(bucket), num);
	//Volta pro master
	MPI_Send(bucket, num, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

void createBuckets(int ***buckets, int num, int bucketSize){
	int i, j, **temp_buckets;
	//Cria a matriz de buckets
	temp_buckets = malloc(sizeof(int*) * num);
	for(i=0; i < num; i++){
		//Aloca um array que representa o bucket
		//Soma 1 pois o indice 0 é usado para guardar o numero de elementos no bucket
		temp_buckets[i] = malloc(sizeof(int) * (bucketSize + 1);
		for(j=0; j < bucketSize; j++){
			temp_buckets[i][j] = 0;
		}
	}	
	*buckets = temp_buckets;
}

void generateValues(int **values, int *count_values){
	FILE *file;
	if((file = fopen("nums", "r")) != NULL){
		//Guarda a linha		
		char *line = NULL;
		size_t len = 0;
		getline(&line, &len, file);
		//Cria um array temporario para atribuir posteriormente ao array na main
		int *temp =  malloc(count * sizeof(int));
		//Faz o parsing dos numeros para o array
		int count = 0;
		char *token;
		token = strtok(line, " ");
		while(token != NULL ) {
			sscanf(token, "%d", &temp[count++]);			
			token = strtok(NULL, " ");
		}
		//Copia o array temporario para a main
		*values = temp;
		//Atualiza a variavel que conta os numeros na main
		*count_values = count;
		//Fecha o arquivo
		fclose(file);
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
