#include <stdio.h>

//quick, select, bubble, insert

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Numero de argumentos insuficiente\n");
		return 1;
	}

	int numBuckets;
	sscanf(argv[1], "%d", &numBuckets);

	int rangeNum;
	sscanf(argv[2], "%d", &rangeNum);

	return 0;
}
