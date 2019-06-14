#include "plateforme.h"

static int state = 0;
static int isInit = 0;
objet* listObjet;
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
	pf->nbItem = 0;
	pf->tabMessageRecu = malloc(sizeof(char*));
	listObjet = (objet*)malloc(sizeof(objet));
	_pf = pf;

}

void PF_run(Plateforme* pf)
{
	int rt;
	connectToServer(pf->nom);

	while(1)
	{
		switch(state)
		{
			case 0:
			for(int i=(pf->nbItem)-1;i>=0;i--)
			{
				if(strlen(pf->tabMessageRecu[i])>1)
				{
					if(isInit == 0)
					{
						//PF pas initialisé, on ne connaît pas notre nom unique.
						free(pf->nom);
						pf->nom = malloc(strlen(pf->tabMessageRecu[i]));
						strcpy(pf->nom,pf->tabMessageRecu[i]);
						isInit = 1;
						printf("My name is %s\n",pf->nom);
					}else
					{
						PF_Traitement_Message(pf->tabMessageRecu[i]);
					}
				}
				free(pf->tabMessageRecu[i]);  
				pf->nbItem--;
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
	printf("Message : %s \n",message);

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
			}
		}
	}
}

void PF_init_nouvel_objet(char* nom, int slotDispo, int nbPattern)
{
	nbObjet++;
	listObjet = realloc(listObjet,nbObjet*sizeof(objet));
	listObjet[nbObjet-1].nom = malloc(strlen(nom));
	strcpy(listObjet[nbObjet-1].nom,nom);

	listObjet[nbObjet-1].slotAvailable = slotDispo;
	listObjet[nbObjet-1].nbPattern = nbPattern;
	listObjet[nbObjet-1].isSecure = 0;
	listObjet[nbObjet-1].listPattern = malloc(sizeof(Pattern));
}

void PF_Distribuer_Travail()
{
	int isSecure = 1;
	for(int i=0;i<nbObjet;i++)
	{
		isSecure = 1;
		if(listObjet[i].isSecure == 0)
		{
			for(int j=0;j<listObjet[i].nbPattern;j++)
			{
				if(listObjet[i].listPattern[j].whoIsWatching == NULL)
				{
					for(int k=0;k<nbObjet;k++)
					{
						if(listObjet[k].slotAvailable>0)
						{
							listObjet[i].listPattern[j].whoIsWatching = malloc(strlen(listObjet[k].nom));
							strcpy(listObjet[i].listPattern[j].whoIsWatching,listObjet[k].nom);
							listObjet[k].slotAvailable--;
							char* messageToSend;
							messageToSend = malloc(strlen(listObjet[k].nom)+sizeof(int)*2+2+strlen(listObjet[i].nom));
							sprintf(messageToSend,"%s|%d/%s/%d",listObjet[k].nom,TO_WATCH,listObjet[i].nom,j);
							client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
							free(messageToSend);
							messageToSend = malloc(strlen(listObjet[i].nom)+sizeof(int)*2+2+strlen(listObjet[k].nom));
							sprintf(messageToSend,"%s|%d/%s/%d",listObjet[i].nom,BEING_WATCH_BY,listObjet[k].nom,j);
							client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
							free(messageToSend);
						}
					}
				}
				if(listObjet[i].listPattern[j].whoIsOrganizing == NULL)
				{
					for(int k=0;k<nbObjet;k++)
					{
						if(listObjet[k].slotAvailable>0)
						{
							listObjet[i].listPattern[j].whoIsOrganizing = malloc(strlen(listObjet[k].nom));
							strcpy(listObjet[i].listPattern[j].whoIsOrganizing,listObjet[k].nom);
							listObjet[k].slotAvailable--;
							char* messageToSend;
							messageToSend = malloc(strlen(listObjet[k].nom)+sizeof(int)*2+2+strlen(listObjet[i].nom));
							sprintf(messageToSend,"%s|%d/%s/%s/%d",listObjet[k].nom,TO_ORGANIZE,listObjet[i].nom,listObjet[i].listPattern[j].whoIsWatching,j);
							client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
							free(messageToSend);
							messageToSend = malloc(strlen(listObjet[i].nom)+sizeof(int)*2+2+strlen(listObjet[k].nom));
							sprintf(messageToSend,"%s|%d/%s/%s/%d",listObjet[i].listPattern[j].whoIsWatching,BEING_ORGANIZE_BY,listObjet[k].nom,listObjet[i].nom,j);
							client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
							free(messageToSend);
						}
					}
				}
				if(listObjet[i].listPattern[j].whoIsWatching == NULL || 
					listObjet[i].listPattern[j].whoIsOrganizing == NULL)
				{
					isSecure = 0;
				}
			}
		}

		if(isSecure == 1)
		{
			printf("Objet safe ! \n");
			char* messageToSend;
			messageToSend = malloc(strlen(listObjet[i].nom)+8+sizeof(int));
			sprintf(messageToSend,"%s|%d/Launch",listObjet[i].nom,LAUNCH_OBJECT);
			printf("%s\n",messageToSend);
			client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
			free(messageToSend);
							
		}
	}
}

void toPrint()
{
	for(int i=0;i<nbObjet;i++)
	{
		printf("OBJET !!!! \nNom : %s\nNbPattern : %d\nslotAvailable %d\n",
			listObjet[i].nom,
			listObjet[i].nbPattern,
			listObjet[i].slotAvailable);
	}
}