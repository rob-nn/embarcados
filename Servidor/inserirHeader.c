#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[]){
	if ( argc != 4 ){
		printf("Erro na insersação dos parâmetros! Formato Correto:\n");
		printf("<NOME_IMAGEM> <LARGURA> <ALTURA>\n");
		exit(1);
	}
	unsigned int ROW, COL;
	char *nomeImgEntrada, *nomeImgSaida;
	unsigned char *imagem;
	unsigned int i;
	
	/*	Conversão de Parâmetros	*/
	nomeImgEntrada = argv[1];
	COL = atoi(argv[2]);
	ROW = atoi(argv[3]);
	
	/*	Nome do Arquivo de Saida	*/
	nomeImgSaida = (char *)malloc(255);
	nomeImgSaida[0] = "\0";
	strcat(nomeImgSaida, nomeImgEntrada); 
	strcat(nomeImgSaida, ".pgm");
	printf("Gerando imagem com cabeçalho: %d", nomeSaida);

	/*	Ponteiro para Imagem de Entrada	*/
	FILE *ptrImgEntrada;
	ptrImgEntrada = fopen(nomeImgEntrada, "rb");
	imagem = (unsigned char *)malloc(ROW*COL*sizeof(unsigned char));
	fread(imagem, sizeof(unsigned char), ROW*COL, ptrImgEntrada);	
	fclose(ptrImgEntrada);
	
	/*	Ponteiro para Imagem de Saida	*/
	FILE *ptrImgSaida;
	ptrImgSaida = fopen("saida.pgm", "w");
	fprintf(ptrImgSaida, "P5 %d %d 255 ", COL, ROW);
	fwrite(imagem, sizeof(unsigned char), ROW*COL, ptrImgSaida);
	fclose(ptrImgSaida);
	
	/*	Libera memória alocada	*/
	free( nomeImgSaida );
	free( imagem );
}
