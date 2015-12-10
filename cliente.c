//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
	Aluno: Thiago Almeida Nunes Guimarães
	Matrícula: 09/0133641

	client.c	
*/
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Variáveis Globais
int largura, altura, larg_im, alt_im, numbits, *chave;
int tam_chave, tam_arq, num_frame, pos=0;
int client_fd, port;  
unsigned char **arquivo, *imagem, *bloco, *ip;
char larg[5], alt[5], l_im[5], a_im[5], n[2];
FILE *f1, *f2, *f3;

//Funções
void Parametros(char *str)
{
	int i, j;

	//Pegar largura do frame
	for(i=0; str[i] != '_'; i++);
	for(j=i+1; str[j] != 'x'; j++)
		larg[j-(i+1)] = str[j];

	//Pegar altura do frame
	for(i=j+1; str[i] != '_'; i++)
		alt[i-(j+1)] = str[i];

	//Pegar largura da imagem estegnografada
	for(j=i+1; str[j] != '_'; j++); 
	for(i=j+1; str[i] != 'x'; i++)
		l_im[i-(j+1)] = str[i];

	//Pegar altura da imagem estegnografada
	for(j=i+1; str[j] != '_'; j++)
		a_im[j-(i+1)] = str[j];

	//Pegar numero de bits
	n[0] = str[j+1];

	//Atribuição
	largura = atoi(larg);
	altura  = atoi(alt);
	larg_im = atoi(l_im);
	alt_im  = atoi(a_im);
	numbits = atoi(n);
}

void *Desembaralhar()												//retirar o bloco e colocar o pixel na im_esteg
{
	//Funções
	void Mat_to_Vector(int pos_x, int pos_y, int key);

	//Variáveis locais
	int i, j,k, l;

	printf("Thread Desembaralhar...\n");
	sleep(1);

	for(i=0, k=0; i<altura*num_frame-3; i+=3)
	{
		for(j=0; j<largura-3; j+=3, k++)
		{
			if(pos == alt_im*larg_im)
				pthread_exit(NULL);

			Mat_to_Vector(i, j, chave[k%tam_chave]);
		}
	}
	
	pthread_exit(NULL);
}

void Mat_to_Vector(int pos_x, int pos_y, int key)
{
	int i, j, k;

	for(i=0, k=0; i<3; i++)
	{
		for(j=0; j<3; j++, k++)
			bloco[k] = arquivo[pos_x+j][pos_y+i];
	}

	if(key != 0 && pos < alt_im*larg_im)
	{
		imagem[pos] = bloco[key-1];
		pos++;
	}
}

void *Extrair()													//inverter os pixel da imagem
{
	//Funções
	unsigned char Inverter(unsigned char pixel);

	//Variáveis locais
	int i;

	printf("Thread Extrair...\n");
	sleep(1);

	for(i=0; i<larg_im*alt_im; i++)
		imagem[i] = Inverter(imagem[i]);	

	pthread_exit(NULL);
}

unsigned char Inverter(unsigned char pixel)
{
	int i;
	unsigned char res, sum=0;

	for(i=0; i<numbits; i++)
	{
		res  = pixel & (int)pow(2, i);
		res *= (int)pow(2, 7-2*i);
		sum |= res; 
	}

	return sum;
}

void Interromper()
{
	int i;
	
	printf("\nPorcentagem: %.2f %%\n", (pos/(float)(larg_im*alt_im)*100));

	for(i=pos+1; i<larg_im*alt_im; i++)
		imagem[i] = 0;

	pos = larg_im*alt_im;
}





int main(int argc, char **argv)
{
	//Funções
	void Parametros(char *str);
	void *Desembaralhar();
	void *Extrair();	
	unsigned char Inverter(unsigned char pixel);
	void Mat_to_Vector(int pos_x, int pos_y, int key);
	void Interromper();

	//Variáveis locais
	int i, j;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Validação dos parâmetros
	if(argc != 5)
	{
		system("clear");
		fprintf(stderr, "Parametros incorretos: <Arquivo> <Chave> <Porta> <IP>\n\n");
		exit(1);											
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Atruibuição das variáveis
	Parametros(argv[0]);
	port = atoi(argv[3]);
	ip = argv[4];

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Estruturas do socket
	struct sockaddr_in client_addr;

	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(ip);
	client_addr.sin_port = htons(port);

	//Criando socket
	if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		system("clear");
		fprintf(stderr, "Falha na criacao do socket.\n\n");
		exit(1);	
	} 
	printf("Socket criado...\n");

	//Conectando
	if(connect(client_fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) == -1)
	{
		system("clear");
		fprintf(stderr, "Falha ao executar connect.\n\n");
		exit(1);	
	} 
	printf("Connect executado com sucesso...\n");
	

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Preparando vídeo
	tam_arq = 0;

	f1 = fopen(argv[1], "rb");
	if(!f1)
	{
		system("clear");
		fprintf(stderr, "Erro na abertura do arquivo, verifique o nome dos arquivo de entrada.\n\n");
		exit(1);	
	}

		//Tamanho arquivo
		while(fgetc(f1)!=EOF) tam_arq++;
		rewind(f1);

		//Parâmetros
		num_frame      = tam_arq/(largura*altura);

		//Alocando espaço para arquivo
		arquivo = (unsigned char **)calloc(altura*num_frame, sizeof(unsigned char *));
		if(arquivo == NULL)
		{
			system("clear");
			fprintf(stderr, "Erro na alocacao de memoria do video.\n\n");
			exit(1);	
		}
		for(i=0; i<altura*num_frame; i++)
		{
			arquivo[i] = (unsigned char *)calloc(largura, sizeof(unsigned char));
			if(arquivo[i] == NULL)
			{
				system("clear");
				fprintf(stderr, "Erro na alocacao de memoria do video.\n\n");
				exit(1);	
			}

			for(j=0; j<largura; j++)
				arquivo[i][j] = fgetc(f1);
		}

	fclose(f1);		

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Preparando chave
	int n1;
	tam_chave = 0, i=0;

	f2 = fopen(argv[2], "r");
	if(!f2)
	{
		system("clear");
		fprintf(stderr, "Erro na abertura do arquivo, verifique o nome dos arquivo chave.\n\n");
		exit(1);	
	}

		//Tamanho chave
		while(!feof(f2)) {fscanf(f2, "%d", &n1); tam_chave++;} if(tam_chave==17) tam_chave--;

		//Alocando espaço para chave
		rewind(f2);
		chave = (int *) calloc(tam_chave, sizeof(int));
		if(chave == NULL)
		{
			system("clear");
			fprintf(stderr, "Erro na alocacao de memoria da chave.\n\n");
			exit(1);	
		}
	
		//Salvando chave em variável
		while(!feof(f2)) {fscanf(f2, "%d", &chave[i]); i++;}								//Obs: tamanho da chave

	fclose(f2);	

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//Alocando espaço para o bloco
	bloco = (unsigned char *) calloc(9, sizeof(unsigned char));
	if(bloco == NULL)
	{	
		system("clear");
		fprintf(stderr, "Erro na alocacao de memoria do bloco.\n\n");
		exit(1);
	}
	

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Alocando espaço para a imagem de saida
	imagem = (unsigned char *) calloc(alt_im*larg_im, sizeof(unsigned char));
	if(imagem == NULL)
	{
		system("clear");
		fprintf(stderr, "Erro na alocacao de memoria da chave.\n\n");
		exit(1);
	}	

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Sinais
	signal(SIGINT, Interromper);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	//Threads
	pthread_t thread1;
	pthread_t thread2;

	system("clear");
	printf("Criando threads...\n");

		//Criando threads
		pthread_create (&thread1, NULL, (void *)Desembaralhar, NULL);
		pthread_join (thread1, NULL);	
		pthread_create (&thread2, NULL, (void *)Extrair, NULL);	
		pthread_join (thread2, NULL);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Passando imagem estegnografada para arquivo de saída
	f3 = fopen("saida.y", "wb");
	if(!f3)
	{
		system("clear");
		fprintf(stderr, "Erro na abertura do arquivo da imagem de saida.\n\n");
		exit(1);
	}

	fwrite(imagem, sizeof(unsigned char), larg_im*alt_im, f3);

	fclose(f3);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Print de informações importantes
	printf("\n===============================  INFORMACOES  =================================\n\n");
	printf("Nome do arquivo: %s\n", argv[1]);
	printf("Tamanho do arquivo: %.3f MB\n", tam_arq/(float)1000000);
	printf("Tamanho de um frame: %dx%d\n", largura, altura);
		printf("Numero de frames: %d\n", num_frame);
	printf("Tamanho da imagem estegnografada: %dx%d\n", larg_im, alt_im);
	printf("Nome chave: %s\n", argv[2]);
	printf("Tamanho chave: %d\n", tam_chave);
	printf("Chave: ");
	for(i=0; i<tam_chave; i++)
		printf("%d ", chave[i]);
	printf("\nQuantidade de pixel da imagem = %d", pos);

	printf("\n\n===============================================================================");	
	printf("\n\n");

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Desalocando memoria
	for(i=0; i<altura*num_frame; i++)
		free(arquivo[i]);
	free(arquivo);
	free(chave);
	free(imagem);
	free(bloco);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Usando execl para colocar cabeçalho na imagem de saída
	int ret;

	ret = fork();

	if(ret == 0)
	{
		//processo filho
		printf("Colocando cabecalho.\n\n");
		execl("cabecalho", "cabecalho", "saida.y", l_im, a_im, NULL);
		printf("\nERRO! A funcao execl falhou.");	
	}
	else	
	{
		sleep(1);
		
		//Variaveis locais
		unsigned char *img, pixel;
		int tamanho_img;

		//Abrindo arquivo com cabecalho
		FILE *f3 = fopen("saida.y", "rb");
		if(f3 == NULL)
		{
			system("clear");
			fprintf(stderr, "Erro na abertura do arquivo.\n\n");
			exit(1);
		}
	
		//Calculando tamanho do arquivo
		tamanho_img = larg_im*alt_im + 9 + strlen(l_im) + strlen(a_im);
		
		//Alocando imagem
		img = (unsigned char *) calloc(tamanho_img, sizeof(unsigned char));
		if(img == NULL)
		{
			system("clear");
			fprintf(stderr, "Erro na alocacao de memoria da imagem com cabecalho.\n\n");
			exit(1);
		}

		fread(img, sizeof(unsigned char), tamanho_img, f3);
		fclose(f3);

		for(i=0; i<tamanho_img; i++)
		{
			pixel = img[i];
			
			if(send(client_fd, &pixel, 1, 0) == -1)
			{
				system("clear");
				fprintf(stderr, "Erro no envio de dados\n\n");
				exit(1);	
			}	
		}	

		//Desalocando memoria
		free(img);	

		printf("Envio concluido com sucesso...\n");
	}

	//Fechando conexão
	close(client_fd);
	printf("Conexao finalizada...\n\n");
		
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	return 0;
}