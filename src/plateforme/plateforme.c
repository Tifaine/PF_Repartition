#include "plateforme.h"

static int state = 0;
static int isInit = 0;
vector listObjet;
static int nbObjet = 0;

Plateforme* _pf;

#define TOPIC 2
#define MAXDATASIZE 500

int connectToServer(char* nom)
{
	client_TCP_envoi_message(nom,01,"1");
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
				_message* message = vector_get(&(pf->tabMessage),i);
				
				if(isInit == 0)
				{
						//PF pas initialisé, on ne connaît pas notre nom unique.
					free(pf->nom);
					pf->nom = malloc(strlen(vector_get(&(message->listArg),0)));
					strcpy(pf->nom,vector_get(&(message->listArg),0));
					isInit = 1;
				}else
				{
					PF_Traitement_Message(message);
				}					
				
				vector_delete(&(pf->tabMessage),i);
				
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

void PF_Traitement_Message(_message* message)
{
	if(message->type==DEPART_OBJET)
	{
		PF_Depart_objet(vector_get(&(message->listArg),0));
	}else if(message->type == NEW_OBJECT)
	{	
		PF_init_nouvel_objet(vector_get(&(message->listArg),1),atoi(vector_get(&(message->listArg),0)),atoi(vector_get(&(message->listArg),2)));
		PF_Distribuer_Travail();
	}
}

void PF_Depart_objet(char * nom)
{
	for(int i=0;i<nbObjet;i++)
	{
		objet* obj = vector_get(&listObjet,i);
		if(strcmp(nom,obj->nom)==0)
		{
			/*for(int j=0;j<obj->nbPattern;j++)
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
			}*/
			vector_delete(&listObjet,i);
		}else
		{

		}
	}
	
	nbObjet--;
}

void PF_init_nouvel_objet(char* nom, int slotDispo, int nbPattern)
{
	nbObjet++;
	objet* obj;
	obj = malloc(sizeof(*(obj)));
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
							mxml_node_t *xml;
							mxml_node_t *data;

							char result[MAXDATASIZE];
							xml = mxmlNewXML("1.0");
							data = mxmlNewElement(xml, "toTransfer");
							mxmlElementSetAttr(data,"receiver",obj2->nom);
							mxmlElementSetAttr(data,"type","3");
							mxmlElementSetAttr(data,"toWatch",obj->nom);
							char _cPatternNumber[2];
							sprintf(_cPatternNumber, "%d", j);
							mxmlElementSetAttr(data,"patternNumber",_cPatternNumber);							
							mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
						
							client_TCP_envoi_message(_pf->nom,2,result);
							break;

							// char* messageToSend;
							// messageToSend = malloc(strlen(obj2->nom)+sizeof(int)*2+2+strlen(obj->nom));
							// sprintf(messageToSend,"%s|%d/%s/%d",obj2->nom,TO_WATCH,obj->nom,j);
							// client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
							// free(messageToSend);							
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
							mxml_node_t *xml;
							mxml_node_t *data;

							char result[MAXDATASIZE];
							xml = mxmlNewXML("1.0");
							data = mxmlNewElement(xml, "toTransfer");
							mxmlElementSetAttr(data,"receiver",obj2->nom);
							mxmlElementSetAttr(data,"type","4");
							mxmlElementSetAttr(data,"toOrganize",obj->nom);
							mxmlElementSetAttr(data,"isWatching",obj->listPattern[j].whoIsWatching);
							char _cPatternNumber[2];
							sprintf(_cPatternNumber, "%d", j);
							mxmlElementSetAttr(data,"patternNumber",_cPatternNumber);							
							mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
							
							client_TCP_envoi_message(_pf->nom,2,result);
							break;



						// 	char* messageToSend;
						// 	messageToSend = malloc(
						// 		strlen(obj2->nom)+
						// 		sizeof(int)+
						// 		strlen(obj->nom)+
						// 		strlen(obj->listPattern[j].whoIsWatching)+
						// 		sizeof(int)+4);

						// 	sprintf(messageToSend,"%s|%d/%s/%s/%d",
						// 		obj2->nom,
						// 		TO_ORGANIZE,
						// 		obj->nom,
						// 		obj->listPattern[j].whoIsWatching,
						// 		j);
						// 	client_TCP_envoi_message(_pf->nom,MESSAGE,messageToSend);
						// 	free(messageToSend);
						}
					}
				}
				if(obj->listPattern[j].whoIsWatching == NULL || 
					obj->listPattern[j].whoIsOrganizing == NULL)
				{
					obj->isSecure = 0;
				}
			}
			mxml_node_t *xml;
			mxml_node_t *data;
			if(obj->isSecure == 1)
			{

				char result[MAXDATASIZE];
				xml = mxmlNewXML("1.0");
				data = mxmlNewElement(xml, "toTransfer");
				mxmlElementSetAttr(data,"receiver",obj->nom);
				mxmlElementSetAttr(data,"type","9");
				mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
				client_TCP_envoi_message(_pf->nom,2,result);	
			}
		}		
	}
}
