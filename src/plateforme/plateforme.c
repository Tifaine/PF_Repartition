#include "plateforme.h"

static int state = 0;


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
}

void PF_run(Plateforme* pf)
{
	int rt;
	connectToServer(pf->nom);
	printf("Waiting init\n");	
/*
	while(1)
	{
		switch(state)
		{
			case 0:
			for(int i=(pf->nbItem)-1;i>=0;i--)
			{
				if(strlen(pf->tabMessageRecu[i])>1)
				{
					PF_Traitement_Message(pf->tabMessageRecu[i]);
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
	}*/
}

void PF_Traitement_Message(char* message)
{

}