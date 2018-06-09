#include <stdio.h>
#include <stdlib.h>

void createBuckets(int ***buckets, int num);
void populateBuckets(int ***buckets, int num, int range);
void printBuckets(int **buckets, int num);
void cleanBuckets(int **buckets, int num);

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Numero de argumentos insuficiente\n");
		return 1;
	}

	int numBuckets;
	sscanf(argv[1], "%d", &numBuckets);

	int **buckets;
	createBuckets(&buckets, numBuckets);

	int rangeNum;
	sscanf(argv[2], "%d", &rangeNum);

	populateBuckets(&buckets, numBuckets, rangeNum);
	printBuckets(buckets, numBuckets);
	
	cleanBuckets(buckets, numBuckets);
	free(buckets);
	return 0;
}

void createBuckets(int ***buckets, int num){
	int i, **temp_buckets;
	temp_buckets = malloc(num * sizeof(int*));
	for(i=0; i < num; i++){
		temp_buckets[i] = malloc(sizeof(int) * 10);	
	}	
	*buckets = temp_buckets;
}

void populateBuckets(int ***buckets, int num, int range){
	int i, j, **temp = *buckets;
	for(i=0; i < num; i++){
		for(j=0;j<10;j++){
			temp[i][j] = 0;
		}
	}
	*buckets= temp;
}

void printBuckets(int **buckets, int num){
	int i, j;
	for(i=0; i < num; i++){
		printf("Bucket %d: ", i);
		for(j=0; j < 10; j++){
			printf("%d ", buckets[i][j]);
		}
		printf("\n");
	}
}


void cleanBuckets(int **buckets, int num){
	int i;
	for(i=0; i<num; i++){
		free(buckets[i]);
	}
}
