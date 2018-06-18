#include <stdio.h>
//#include <mpi.h>
#include <stdlib.h>
#include <string.h>

void createBuckets(int ***buckets, int num);
void generateValues(int **values, int *count);
void printValues(int *values, int num);
void populateBuckets(int ***buckets, int numBuckets, int *values, int count);
void printBuckets(int **buckets, int num);
void sort(int ***buckets, int num);
int* mergeBuckets(int **buckets, int num, int valuesSize);
void cleanBuckets(int **buckets, int num);

void master(int ***buckets, int num);
void worker(int rank);

int main(int argc, char *argv[]){
	//MPI_Init(&argc,&argv);
	if(argc < 2){
		printf("Numero de argumentos insuficiente\n");
		//MPI_Finalize();
		return 1;
	}
//	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	int numBuckets;
	sscanf(argv[1], "%d", &numBuckets);

	int **buckets;
	createBuckets(&buckets, numBuckets);

	int *values, count;
	//Gera os valores randomicos para serem divididos nos buckets	
	generateValues(&values, &count);
	printValues(values, count);

	//Popula os buckets pelo array gerado no método anterior, dividindo os valores entre os buckets
	populateBuckets(&buckets, numBuckets, values, count);
	printBuckets(buckets, numBuckets);

	//Define o ranking para distribuir os buckets
	int myrank;
	//MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	/*if(myrank != 0){
		worker();
	} else{
		master();
	}*/
	
	//Ordena
	sort(&buckets, numBuckets);
	printBuckets(buckets, numBuckets);

	//Junta nos values de novo
	free(values);
	values = mergeBuckets(buckets, numBuckets, count);
	printValues(values, count);

	//Limpa tuto
	cleanBuckets(buckets, numBuckets);
	free(buckets);
	free(values);
	
	//Encerra o MPI
	//MPI_Finalize();	
	return 0;
}

void master(int ***buckets, int num){
// 	int i;
// //	MPI_Request request;
// 	for(i=1; i<num; i++){
// 		MPI_Isend(*buckets[i], *buckets[i][0], MPI_INT, i, i, MPI_COMM_WORLD, *request);
// 	}
}

void worker(int rank){
	// int []bucket;
	// MPI_Status status;
	// MPI_Recv(&bucket, num, MPI_INT, 0, rank, &status);
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
		printf("%d\n", count);
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
