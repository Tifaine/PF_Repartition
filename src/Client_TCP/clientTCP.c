#include "clientTCP.h"


#define MAXDATASIZE 500
static int sockFileDescriptor;
static int isConnected = 0;

pthread_t threadClientTCP;

// Declaration of thread condition variable
pthread_cond_t condClient = PTHREAD_COND_INITIALIZER;
  
// declaring mutex 
pthread_mutex_t lockClient = PTHREAD_MUTEX_INITIALIZER;


struct timespec timeToWait;
struct timeval now;

int client_TCP_init_connec(char* addr, int port)
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
	pthread_create(&(threadClientTCP),NULL,client_TCP_attente_message,NULL);
	return 0;
}

int client_TCP_envoi_message(char* nom_emetteur, int type, char* message)
{
	int rt;
	char* messageToSend;
	messageToSend = malloc(strlen("01AB")+strlen(message)+strlen(nom_emetteur)+5+sizeof(int));
	sprintf(messageToSend,"01AB|%d|%s|%d|%d|%s",strlen(nom_emetteur),nom_emetteur,type,strlen(message),message);


	if(send(sockFileDescriptor, messageToSend, strlen(messageToSend), MSG_CONFIRM) == -1)
	{
        perror("send");
		return (1);
	}
	gettimeofday(&now,NULL);
	timeToWait.tv_sec = now.tv_sec+1;
	pthread_mutex_lock(&lockClient);
	rt = pthread_cond_timedwait(&condClient, &lockClient, &timeToWait);
	if(rt!=0)
	{
		printf("ERREUR : %d\n",rt);
	}
	
	pthread_mutex_unlock(&lockClient);
	return 0;
}

void* client_TCP_attente_message(void* ag)
{
	char* buf;
	printf("Attente message\n");
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
				for(int i = nbMess-1;i>=0;i--)
				{		
					printf("Message reçu : %s\n",result[i]);
					if(strstr(result[i],"success")!=NULL)
					{
						
						free(result[i]);
						pthread_mutex_lock(&lockClient);
						pthread_cond_signal(&condClient); 
						pthread_mutex_unlock(&lockClient);
						usleep(100);
					
					}else
					{  					
						
    					free(result[i]);
					}

				}				
			}else
			{
				return NULL;
			}    			
		}			
	}	
}