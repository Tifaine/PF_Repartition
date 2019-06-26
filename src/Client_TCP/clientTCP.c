#include "clientTCP.h"


#define MAXDATASIZE 2000
static int sockFileDescriptor;
static int isConnected = 0;

pthread_t threadClientTCP;

int client_TCP_init_connec(char* addr, int port, void* arg)
{
	
	struct sockaddr_in servaddr;
	sockFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFileDescriptor == -1)
	{
		printf("Failed \n");
		return -1; 
	}
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(addr); 
	servaddr.sin_port = htons(port);
	if (connect(sockFileDescriptor, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
	{ 
		printf("connection with the server failed...\n"); 
		return -2; 
	}else
	{
		printf("connected to the server.\n"); 
	}
	isConnected = 1;
	pthread_create(&(threadClientTCP),NULL,client_TCP_attente_message,arg);
	return 0;
}

int client_TCP_envoi_message(char* nom_emetteur, int type, char* message)
{
	int rt;
	char* messageToSend = malloc( strlen("01AB") + strlen(message) + strlen(nom_emetteur) + 5 + 10 );
	sprintf(messageToSend,"01AB%d|%s|%d|%d|%s",strlen(nom_emetteur),nom_emetteur,type,strlen(message),message);
	if(send(sockFileDescriptor, messageToSend, strlen(messageToSend), MSG_CONFIRM) == -1)
	{
		free ( messageToSend );
		perror("send");
		return (1);
	}
	free ( messageToSend );
	
	return 0;
}

void* client_TCP_attente_message(void* arg)
{
	char* buf;
	int mustPreventPF = 0;
	while(1)
	{
		if(isConnected == 1)
		{
			buf = calloc(MAXDATASIZE,sizeof(char));
			buf[0]='\0';
			if(recv(sockFileDescriptor, buf, MAXDATASIZE, 0)>0)
			{
				char** result;
				result = malloc(sizeof(char*));	
				int nbMess = findSubstring(buf,"01AB", &result);
				free(buf);


				if(nbMess>0)
				{
					for(int i = nbMess-1;i>=0;i--)
					{
						vector_add(&((Plateforme*)arg)->tabMessage,result[i]);
						mustPreventPF = 1;						
					}
				}
				if(mustPreventPF == 1)
				{
					pthread_mutex_lock((&((Plateforme*)arg)->lockPF));
					pthread_cond_signal((&((Plateforme*)arg)->condPF));
					pthread_mutex_unlock((&((Plateforme*)arg)->lockPF));
					mustPreventPF = 0;
				}
			}else
			{
				return NULL;
			}    			
		}			
	}	
}