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
	mxml_node_t *xml;
	mxml_node_t *data;
	char _cType[2];
	char result[MAXDATASIZE];
	sprintf(_cType, "%d", type);


	xml = mxmlNewXML("1.0");
	data = mxmlNewElement(xml, "message");
	mxmlElementSetAttr(data,"name",nom_emetteur);
   
    mxmlElementSetAttr(data,"type",_cType);
    mxmlElementSetAttr(data,"content",message);
	mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
	printf("To send : %s\n",result);
	

	
	if(send(sockFileDescriptor, result, strlen(result), MSG_CONFIRM) == -1)
	{
		free ( result );
		perror("send");
		return (1);
	}

	//free(result);
	return 0;
}

void* client_TCP_attente_message(void* arg)
{
	char* buf;
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
				printf("Recu : %s\n",buf);
				int nbMess = openMessage(&((Plateforme*)arg)->tabMessage,buf);				
				free(buf);
				printf("nbMessage : %d\n",nbMess);
				if(nbMess>0)
				{
					pthread_mutex_lock((&((Plateforme*)arg)->lockPF));
					pthread_cond_signal((&((Plateforme*)arg)->condPF));
					pthread_mutex_unlock((&((Plateforme*)arg)->lockPF));				
					
				}
			}else
			{
				return NULL;
			}    			
		}			
	}	
}