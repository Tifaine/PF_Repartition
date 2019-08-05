#ifndef __PARSERXML_H__
#define __PARSERXML_H__

#include "../mxml/mxml.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct _message
{
	char* emitterName;
	int type;
	vector listArg;
}_message;

int openMessage(vector * listMessage, char * message);


#endif //__PARSERXML_H__