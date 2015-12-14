//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Variaveis globais
int server_fd, client_fd, tam, tamanho_img, port, w_im, h_im;
unsigned char *img, *ip;

void insertHeader(char * nomeImgEntrada, int col, int row){
	unsigned int ROW, COL;
	char *nomeImgSaida;
	unsigned char *imagem;
	unsigned int i;
	
	/*	Conversão de Parâmetros	*/
	COL = col;
	ROW = row;
	
	/*	Nome do Arquivo de Saida	*/
	nomeImgSaida = (char *)malloc(255);
	nomeImgSaida[0] = '\0';

	strcat(nomeImgSaida, nomeImgEntrada);
	nomeImgSaida[strcspn(nomeImgSaida, ".y")] = "\0";
	strcat(nomeImgSaida, ".pgm");

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

int main(int argc, char *argv[])
{
	int i;
	unsigned char pixel;

	//Validando os parametros
	if(argc != 5)
	{
		system("clear");
		fprintf(stderr, "Parametros incorretos, digite: <Largura> <Altura> <Porta> <IP>\n\n");
		exit(1);	
	}

	w_im = atoi(argv[1]);
	h_im = atoi(argv[2]);
	port = atoi(argv[3]);
	ip = argv[4];

	//Estrutura do socket
	struct sockaddr_in client_addr, server_addr;
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	//Criando socket
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		system("clear");
		fprintf(stderr, "Erro na criacao do socket\n\n");
		exit(1);	
	}
	printf("Socket criado com sucesso...\n");

	//Bind
	if(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
	{
		system("clear");
		fprintf(stderr, "Erro na execucao do bind\n\n");
		exit(1);	
	}
	printf("Bind executado com sucesso...\n");

	//Listen
	if(listen(server_fd, 1) == -1)
	{
		system("clear");
		fprintf(stderr, "Erro na execucao do listen\n\n");
		exit(1);	
	}
	printf("Listen executado com sucesso...\n");

	//Accept
	tam = sizeof(client_addr);

	if((client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &tam)) == -1)
	{
		system("clear");
		fprintf(stderr, "Erro na execucao do accept\n\n");
		exit(1);	
	}
	printf("Accept executado com sucesso...\n");

	//Recebendo informação
	printf("Recebendo dados...\n");

	tamanho_img = 9 + w_im*h_im + strlen(argv[1]) + strlen(argv[2]);
	
	//Alocando imagem recebida
	img = (unsigned char *) calloc(tamanho_img, sizeof(unsigned char));
	if(img == NULL)
	{
		system("clear");
		fprintf(stderr, "Erro na alocação de memoria para imagem de saida\n\n");
		exit(1);	
	}
	
	for(i=0; i<tamanho_img; i++)
	{
		if(recv(client_fd, &pixel, 1, 0) == -1)
		{
			system("clear");
			fprintf(stderr, "Erro no recebimento dos dados\n\n");
			exit(1);	
		}	
		
		img[i] = pixel;
	}	

	printf("Imagem recebida com sucesso...");

	FILE *f = fopen("saida.y", "wb");
	if(f == NULL)
	{
		system("clear");
		fprintf(stderr, "Erro na criacao do arquivo de saida\n\n");
		exit(1);	
	}

	fwrite(img, sizeof(unsigned char), tamanho_img, f);
	printf("Imagem gerada com sucesso...\n\n");

	insertHeader("saida.y", w_im, h_im);

	return 0;
}
