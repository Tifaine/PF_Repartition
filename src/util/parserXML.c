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
	printf("La\n");
	node = mxmlFindElement ( tree, tree, "publish", NULL, NULL, MXML_DESCEND );
	if(node != NULL)
	{
		for ( node = mxmlFindElement ( tree, tree, "publish", NULL, NULL, MXML_DESCEND );
			node != NULL;
			node = mxmlFindElement ( node, tree, "publish", NULL, NULL, MXML_DESCEND ) )
		{
			nodeBis = mxmlFindElement ( node, tree, "content", NULL, NULL, MXML_DESCEND );
			if(nodeBis != NULL)
			{
				_message* mess;
				mess = malloc(sizeof(*(mess)));
				
				char * type = mxmlElementGetAttr ( nodeBis, "type" );
				if(type != NULL)
				{
					if(atoi(type) == 8)
					{
						mess->type = atoi(type);
						mess->content = malloc(strlen(mxmlElementGetAttr ( nodeBis, "unique_name" )));
						strcpy(mess->content,mxmlElementGetAttr ( nodeBis, "unique_name" ));
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
			printf("La\n");
			_message* mess;
			mess = malloc(sizeof(*(mess)));
			mess->emitterName = malloc(strlen(mxmlElementGetAttr ( node, "name" )));
			strcpy(mess->emitterName,mxmlElementGetAttr ( node, "name" ));		
			mess->type = atoi(mxmlElementGetAttr ( node, "type" ));
			printf("La\n");
			switch(mess->type)
			{
				case INIT:
				mess->content = malloc(strlen(mxmlElementGetAttr ( node, "privateName" )));

				strcpy(mess->content,mxmlElementGetAttr ( node, "privateName" ));

				break;
			}

			vector_add(listMessage,mess);
			nbMessage++;
		}
	}
	
	
	return nbMessage;

}
