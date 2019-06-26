#include "plateforme.h"

static int state = 0;
static int isInit = 0;
vector listObjet;
static int nbObjet = 0;

Plateforme* _pf;

#define TOPIC 2
#define MAXDATASIZE 100

int connectToServer(char* nom)
{
	client_TCP_envoi_message(nom,01,"01");
	return 0;
}

void initPF(Plateforme* pf, char* nom)
{
	pthread_cond_init(&(pf->condPF), NULL);
	pthread_mutex_init(&(pf->lockPF), NULL);
	pf->nom = malloc(strlen(nom));
	strcpy(pf->nom,nom);
	vector_init(&(pf->tabMessage));
	vector_init(&listObjet);
	_pf = pf;

}

void PF_run(Plateforme* pf)
{
	int rt;
	connectToServer(pf->nom);
	int totalMessage;
	while(1)
	{
		switch(state)
		{
			case 0:
			totalMessage = vector_total(&(pf->tabMessage));
			for(int i=totalMessage-1;i>=0;i--)
			{
				char * message = vector_get(&(pf->tabMessage),i);
				if(strlen(message)>1)
				{
					if(isInit == 0)
					{
						//PF pas initialisé, on ne connaît pas notre nom unique.
						free(pf->nom);
						pf->nom = malloc(strlen(message));
						strcpy(pf->nom,message);
						isInit = 1;
					}else
					{
						PF_Traitement_Message(message);
					}
					vector_delete(&(pf->tabMessage),i);
				}
				
			}
			state = 1;
			break;
			case 1:
			state = 2;
			break;
			case 2:			
			state = 3;
			break;			
			case 3:
			pthread_mutex_lock(&(pf->lockPF));
			rt = pthread_cond_wait(&(pf->condPF), &(pf->lockPF));
			if(rt!=0)
			{
				printf("ERREUR : %d\n",rt);
			}else
			{
				state=0;
			}
			pthread_mutex_unlock(&(pf->lockPF));

			break;
		}    	
	}
}

void PF_Traitement_Message(char* message)
{
	char** delimiters;
	int nbElement;
	delimiters = str_split ( message, '-', &nbElement );
	if(nbElement == 2)
	{
		if(atoi(delimiters[0])==TOPIC)
		{
			char** delimiters1;
			int nbElement1;
			delimiters1 = str_split ( delimiters[1], '/', &nbElement1 );
			if(nbElement1 == 3)
			{
				PF_init_nouvel_objet(delimiters1[0],atoi(delimiters1[1]),atoi(delimiters1[2]));
				PF_Distribuer_Travail();
			}else if(nbElement1 == 2)
			{
				if(strlen(delimiters1[0])==1)
				{
					if(atoi(delimiters1[0]) == DEPART_OBJET)
					{
						PF_Depart_objet(delimiters1[1]);
					}
				}
			}
		}
	}
}

void PF_Depart_objet(char * nom)
{
	for(int i=0;i<nbObjet;i++)
	{
		objet* obj = vector_get(&listObjet,i);
		if(strcmp(nom,obj->nom)==0)
		{
			for(int j=0;j<obj->nbPattern;j++)
			{
				if(strcmp(nom,obj->listPattern[j].whoIsWatching) != 0)
				{
					char* messageToSend;
					messageToSend = malloc(strlen(nom)+sizeof(int)+3+strlen(obj->listPattern[j].whoIsWatching));
					sprintf(messageToSend,"%s|%d/%s",obj->listPattern[j].whoIsWatching,DEPART_OBJET,nom);
					client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
					for(int k=0;k<nbObjet;k++)
					{
						objet* obj2 = vector_get(&listObjet,k);
						if(strcmp(obj2->nom,obj->listPattern[j].whoIsWatching)==0)
						{
							obj2->slotAvailable++;
						}
					}
				}
				if(strcmp(nom,obj->listPattern[j].whoIsOrganizing) != 0)
				{
					char* messageToSend;
					messageToSend = malloc(strlen(nom)+sizeof(int)+3+strlen(obj->listPattern[j].whoIsOrganizing));
					sprintf(messageToSend,"%s|%d/%s",obj->listPattern[j].whoIsOrganizing,DEPART_OBJET,nom);
					client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
					for(int k=0;k<nbObjet;k++)
					{
						objet* obj2 = vector_get(&listObjet,k);
						if(strcmp(obj2->nom,obj->listPattern[j].whoIsOrganizing)==0)
						{
							obj2->slotAvailable++;
						}
					}
				}
			}
			vector_delete(&listObjet,i);
		}else
		{

		}
	}
	printf("%d\n",nbObjet);
	nbObjet--;
}

void PF_init_nouvel_objet(char* nom, int slotDispo, int nbPattern)
{
	nbObjet++;
	objet* obj;
	obj = malloc(sizeof(objet));
	vector_add(&listObjet,obj);
	obj->nom = malloc(strlen(nom));
	strcpy(obj->nom,nom);
	obj->slotAvailable = slotDispo;
	obj->nbPattern = nbPattern;
	obj->isSecure = 0;
	obj->listPattern = malloc(sizeof(Pattern));
}

void PF_Distribuer_Travail()
{
	for(int i=0;i<nbObjet;i++)
	{
		objet* obj = vector_get(&listObjet,i);
		if(obj->isSecure == 0)
		{
			for(int j=0;j<obj->nbPattern;j++)
			{
				if(obj->listPattern[j].whoIsWatching == NULL)
				{
					for(int k=0;k<nbObjet;k++)
					{
						objet* obj2 = vector_get(&listObjet,k);
						if(obj2->slotAvailable>0)
						{
							obj->listPattern[j].whoIsWatching = malloc(strlen(obj2->nom));
							strcpy(obj->listPattern[j].whoIsWatching,obj2->nom);
							obj2->slotAvailable--;
							char* messageToSend;
							messageToSend = malloc(strlen(obj2->nom)+sizeof(int)*2+2+strlen(obj->nom));
							sprintf(messageToSend,"%s|%d/%s/%d",obj2->nom,TO_WATCH,obj->nom,j);
							client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
							free(messageToSend);							
						}
					}
				}
				if(obj->listPattern[j].whoIsOrganizing == NULL)
				{
					for(int k=0;k<nbObjet;k++)
					{
						objet* obj2 = vector_get(&listObjet,k);
						if(obj2->slotAvailable>0)
						{
							obj->isSecure = 1;
							obj->listPattern[j].whoIsOrganizing = malloc(strlen(obj2->nom));
							strcpy(obj->listPattern[j].whoIsOrganizing,obj2->nom);
							obj2->slotAvailable--;
							char* messageToSend;
							messageToSend = malloc(
								strlen(obj2->nom)+
								sizeof(int)+
								strlen(obj->nom)+
								strlen(obj->listPattern[j].whoIsWatching)+
								sizeof(int)+4);

							sprintf(messageToSend,"%s|%d/%s/%s/%d",
								obj2->nom,
								TO_ORGANIZE,
								obj->nom,
								obj->listPattern[j].whoIsWatching,
								j);
							client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
							free(messageToSend);
						}
					}
				}
				if(obj->listPattern[j].whoIsWatching == NULL || 
					obj->listPattern[j].whoIsOrganizing == NULL)
				{
					obj->isSecure = 0;
				}
			}
			if(obj->isSecure == 1)
			{
				char* messageToSend;
				messageToSend = malloc(strlen(obj->nom)+8+sizeof(int));
				sprintf(messageToSend,"%s|%d/Launch",obj->nom,LAUNCH_OBJECT);
				client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
				free(messageToSend);				
			}
		}		
	}
}
