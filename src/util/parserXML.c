#include "parserXML.h"
#define INIT 					1
#define MESSAGE					2


int openMessage(vector * listMessage, char * message)
{
	FILE *fp = NULL;
	int nbMessage = 0;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *nodeBis = NULL;
	char* content;
	tree = mxmlLoadString(NULL, message, MXML_OPAQUE_CALLBACK);
	if(tree == NULL)
	{
		return 0;
	}
	node = mxmlFindElement ( tree, tree, "publish", NULL, NULL, MXML_DESCEND );
	if(node != NULL)
	{
		for ( node = mxmlFindElement ( tree, tree, "publish", NULL, NULL, MXML_DESCEND );
			node != NULL;
			node = mxmlFindElement ( node, tree, "publish", NULL, NULL, MXML_DESCEND ) )
		{
			nodeBis = mxmlFindElement ( node, node, "content", NULL, NULL, MXML_DESCEND );
			if(nodeBis != NULL)
			{
				_message* mess;
				mess = malloc(sizeof(*(mess)));
				vector_init(&(mess->listArg));
				char * type = mxmlElementGetAttr ( nodeBis, "type" );
				if(type != NULL)
				{
					printf("Type : %s\n",type);
					if(atoi(type) == 8)
					{
						mess->type = atoi(type);
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "unique_name" ));
						vector_add(listMessage,mess);
						nbMessage++;
					}else if(atoi(type)==2)
					{
						mess->type = atoi(type);
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbSlot" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "from" ));						
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbPattern" ));
						vector_add(listMessage,mess);
						nbMessage++;
					}
					
				}
			}
		}
	}else
	{
		for ( node = mxmlFindElement ( tree, tree, "message", NULL, NULL, MXML_DESCEND );
			node != NULL;
			node = mxmlFindElement ( node, tree, "message", NULL, NULL, MXML_DESCEND ) )
		{
			_message* mess;
			mess = malloc(sizeof(*(mess)));
			vector_init(&(mess->listArg));
			mess->emitterName = malloc(strlen(mxmlElementGetAttr ( node, "name" )));
			strcpy(mess->emitterName,mxmlElementGetAttr ( node, "name" ));		
			mess->type = atoi(mxmlElementGetAttr ( node, "type" ));
			switch(mess->type)
			{
				case INIT:
				vector_add(&(mess->listArg),mxmlElementGetAttr ( node, "privateName" ));
				break;
			}

			vector_add(listMessage,mess);
			nbMessage++;
		}
	}
	
	
	return nbMessage;

}
