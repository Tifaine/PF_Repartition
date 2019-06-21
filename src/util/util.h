/*
* util.h
*
* Copyright (c) 2019 STMicroelectronics - All Rights Reserved
* Copyright (c) 2019 CREN - Action de Recherche Tifaifai
*
* Author: Guyot Tifaine <tifaine.guyot@univ-lemans.fr> <tifaine.guyot@st.com>
*/


#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include "vector.h"

#define INIT 					1
#define MESSAGE					2

typedef struct Plateforme
{
	
	// Declaration of thread condition variable
	pthread_cond_t condPF;

	// declaring mutex 
	pthread_mutex_t lockPF;

	vector tabMessage;

	char* nom;
}
Plateforme;

/**
 * \fn char** str_split(char* a_str, const char a_delim, int* nbItem)
 * \brief Fonction permettant de séparer une chaîne de caractères en sous chaînes séparées par le délimiteur passé en paramètre
 *
 * \param a_str Adresse de la chaîne de caractères à diviser.
 * \param a_delim Délimiteur à utiliser pour diviser la chaîne de caractère
 * \param nbItem Adresse d'un int où indiquer le nombre de sous chaînes de caractères générées.
 *
 * \return un tableau de chaîne de caractères contenant toutes les sous^chaînes, NULL sinon.
 */
char** str_split(char* a_str, const char a_delim, int* nbItem);

/**
 * \fn int findSubstring(char* input, char* toFind, char*** result)
 * \brief Fonction similaire à str_split mais qui divise une chaîne de caractères selon une autre chaîne de caractère et un non un simple délimiteur
 *
 * \param input Adresse de la chaîne de caractères à diviser.
 * \param toFind Chaîne de caractère à utiliser pour diviser la chaîne de caractère
 * \param result Adresse d'un tableau de chaîne de caractères où stockées les sous chaîne de caractères
 *
 * \return le nombre de sous chaînes générées.
 */
int findSubstring(char* input, char* toFind, char*** result);

#endif //UTIL_H