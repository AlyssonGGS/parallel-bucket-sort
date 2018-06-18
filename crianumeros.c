#include <stdio.h>
#include <stdlib.h>

void populaArquivo(int qtt, int range);

int main(int argc, char *args[]){
	int qtt, range;
	if(argc != 3){
		printf("Erro ao executar programa. Parametros insuficientes");
		return 1;
	}

	sscanf(args[1], "%d", &qtt);
	sscanf(args[2], "%d", &range);
	populaArquivo(qtt, range);
	return 0;
}

void populaArquivo(int qtt, int range){
	FILE *fp;
	if((fp = fopen("nums", "w")) != NULL){
		int i;
		for(i = 0; i < qtt; i++){
			int x = rand() % range;
			fprintf(fp, "%d ", x);
		}
		fclose(fp);
	}
	else{
		printf("Erro ao criar arquivo.\n");
	}
}
