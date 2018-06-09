#include <stdio.h>
#include <stdlib.h>

void createBuckets(int ***buckets, int num);

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Numero de argumentos insuficiente\n");
		return 1;
	}

	int numBuckets;
	sscanf(argv[1], "%d", &numBuckets);

	int rangeNum;
	sscanf(argv[2], "%d", &rangeNum);

	int **buckets;
	createBuckets(&buckets, numBuckets);
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
