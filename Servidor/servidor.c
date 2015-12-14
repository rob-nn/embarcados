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
int server_fd, client_fd, tam, port;
unsigned char *ip;

int main(int argc, char *argv[])
{
	int i;
	unsigned char pixel;

	//Validando os parametros
	if(argc != 3)
	{
		system("clear");
		fprintf(stderr, "Parametros incorretos, digite:  <Porta> <IP>\n\n");
		exit(1);	
	}

	port = atoi(argv[1]);
	ip = argv[2];

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

	FILE *f = fopen("saida.pgm", "wb");
	if(f == NULL)
	{
		system("clear");
		fprintf(stderr, "Erro na criacao do arquivo de saida\n\n");
		exit(1);	
	}

	
	while (recv(client_fd, &pixel, 1, 0))
	{
		fwrite(&pixel, sizeof(unsigned char), 1, f);
	} 
	printf("Imagem recebida com sucesso...\n");
	printf("Imagem gerada com sucesso...\n\n");


	return 0;
}
