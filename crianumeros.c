#include <stdio.h>
#include <stdlib.h>
#define RANGE 10000
#define QUANTIDADE 1000
//rand()%range;

void populaArquivo();

int main(){
	populaArquivo();
	return 0;
}

void populaArquivo(){
	FILE *fp;
	if((fp = fopen("nums.txt", "w")) != NULL){
		int i;
		for(i = 0; i < QUANTIDADE; i++){
			int x = rand() % RANGE;
			fprintf(fp, "%d ", x);
		}
		fclose(fp);
	}
	else{
		printf("Erro ao criar arquivo.\n");
	}
}
