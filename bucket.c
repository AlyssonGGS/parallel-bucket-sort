#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define BUCKET_SIZE 5

void createBuckets(int ***buckets, int num);
void generateValues(int **values, int size, int range);
void printValues(int *values, int num);
void populateBuckets(int ***buckets, int numBuckets, int *values, int numValues);
void printBuckets(int **buckets, int num);
void sort(int ***buckets, int num);
int* mergeBuckets(int **buckets, int num, int valuesSize);
void cleanBuckets(int **buckets, int num);

void master(int ***buckets, int num);
void worker(int rank);

int main(int argc, char *argv[]){
	/* numero de buckets, range dos numeros, quantidade de valores */
	MPI_Init(&argc,&argv);
	if(argc < 4){
		printf("Numero de argumentos insuficiente\n");
		MPI_Finalize();
		return 1;
	}
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	int numBuckets;
	sscanf(argv[1], "%d", &numBuckets);

	int **buckets;
	printf("criando buckets\n");
	createBuckets(&buckets, numBuckets);

	//Range para geração de numeros aleatorios
	int rangeNum;
	sscanf(argv[2], "%d", &rangeNum);

	//Quantidade de valores
	int valuesSize, *values;
	sscanf(argv[3], "%d", &valuesSize);	

	//Gera os valores randomicos para serem divididos nos buckets	
	printf("preenchendo valores\n");
	generateValues(&values, valuesSize, rangeNum);
	printValues(values, valuesSize);

	//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
	printf("populando buckets\n");
	populateBuckets(&buckets, numBuckets, values, valuesSize);
	printBuckets(buckets, numBuckets);

	//Define o ranking para distribuir os buckets
	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	/*
	if(myrank != 0){
		worker(myrank);
	} else{
		master(&buckets, myrank);
	}*/
	
	//Ordena
	printf("ordenando buckets\n");
	sort(&buckets, numBuckets);
	printBuckets(buckets, numBuckets);

	//Junta nos values de novo
	printf("juntando values\n");
	free(values);
	values = mergeBuckets(buckets, numBuckets, valuesSize);
	printValues(values, valuesSize);

	//Limpa tuto
	printf("limpando\n");
	cleanBuckets(buckets, numBuckets);
	free(buckets);
	free(values);
	
	//Encerra o MPI
	MPI_Finalize();	
	return 0;
}

void master(int ***buckets, int num){
	int i;
	MPI_Request request;
	for(i=1; i<num; i++) MPI_Isend(*buckets[i], *buckets[i][0], MPI_INT, i, i, MPI_COMM_WORLD, &request);
	
	//recebe buckets e junta
}

void worker(int rank){
	int *bucket, num;
	MPI_Status status;
	MPI_Recv(&bucket, num, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
	
	//ordena
	
	//envia de volta
	
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

void printValues(int *values, int num) {
	int i;
	for(i=0;i<num;i++){
		printf("%d>", values[i]);
	}
	printf("\n");
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
		if(buckets[i][0] > 1) printf("Bucket %d, com size %d: ", i, buckets[i][0]);
		for(j=1; j < buckets[i][0]; j++){
			printf("%d ", buckets[i][j]);
		}
		printf("\n");
	}
}

void bubbleSort(int **bucket, int i){
	int j, k, *tempB = (int*) bucket[i];
	for(j = 1; j < tempB[0] + 1; j++){
		for(k = j + 1; k< tempB[0] + 1; k++){
			if(tempB[j] > tempB[k]){
				int temp = tempB[j];
				tempB[j] = tempB[k];
				tempB[k] = temp;
			}
		}
	}
	
	*bucket = tempB;
}

void sort(int ***buckets, int num){
	int i, ** bucket = *buckets;
	for(i = 0; i < num; i++){
		bubbleSort(buckets[i], i);
	}
	*buckets = bucket;
}

int* mergeBuckets(int **buckets, int num, int valuesSize){
	int *result = malloc(valuesSize * sizeof(int));
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
