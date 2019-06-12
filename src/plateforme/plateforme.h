/**
* 
*
* Copyright (c) 2019 STMicroelectronics - All Rights Reserved
* Copyright (c) 2019 CREN - Action de Recherche Tifaifai
*
* \file platforme.h
* \brief Plaforme de répartition des agents, permet de s'initialiser auprès d'un serveur et de répartir les agents
* \author Guyot Tifaine <tifaine.guyot@univ-lemans.fr> <tifaine.guyot@st.com>
* \version 0.1
* \date 03/06/2019
*
* 
*/


#ifndef PLATEFORME_H
#define PLATEFORME_H
#include "../Client_TCP/clientTCP.h"
#include "../util/util.h"

typedef struct objet
{
	char* nom;
	int slotDispo;
	int slotRequisWatcher;
	int slotRequisOrganizer;
}objet;

/**
 * \fn int connectToServer(char* nom)
 * \param nom Nom initial de l'objet, doit être mis à jour une fois la réponse du serveur obtenu
 * \brief Fonction permettant de s'enregistrer auprès du serveur et de recevoir son nom unique sur le réseau.
 *
 */
int connectToServer(char* nom);


/**
 * \fn void initPF(Plaforme* pf);
 * \param pf pointeur vers la structure Plateforme contenant les informations relatives
 * 		au bon fonctionnement de la répartition des agents
 *
 */
void initPF(Plateforme* pf, char* nom);

void PF_run(Plateforme* pf);

void PF_Traitement_Message(char* message);

void PF_init_nouvel_objet(char* nom, int slotDispo, int slotRequisWatcher, int slotRequisOrganizer);

#endif //PLATEFORME_H