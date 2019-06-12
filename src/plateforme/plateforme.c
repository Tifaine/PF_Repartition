#include "plateforme.h"

static int state = 0;
static int isInit = 0;
objet* listObjet;
static int nbObjet = 0;

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

}

void PF_init_nouvel_objet(char* nom, int slotDispo, int slotRequisWatcher, int slotRequisOrganizer)
{
	nbObjet++;
	listObjet = realloc(listObjet,nbObjet*sizeof(objet));
	listObjet[nbObjet-1].nom = malloc(strlen(nom));
	listObjet[nbObjet-1].slotDispo = slotDispo;
	listObjet[nbObjet-1].slotRequisWatcher = slotRequisWatcher;
	listObjet[nbObjet-1].slotRequisOrganizer = slotRequisOrganizer;
}