#include "util.h"



char** str_split(char* a_str, const char a_delim, int* nbItem)
{
  char** result    = 0;
  size_t count     = 0;
  char* tmp        = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

    /* Count how many elements will be extracted. */
  while (*tmp)
  {
    if (a_delim == *tmp)
    {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

    /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
  count++;

  result = malloc(sizeof(char*) * count);

  if (result)
  {
    size_t idx  = 0;
    char* token = strtok(a_str, delim);

    while (token)
    {
      assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    assert(idx == count - 1);
    *(result + idx) = 0;
  }
  *nbItem = count-1;
  return result;
}

int findSubstring(char* input, char* toFind, char*** result)
{
  int nbMessage = 0;
  int isOk = 1;
  int* tabIndice;
  tabIndice = malloc(sizeof(int));
  for(int i = 0;i<strlen(input)-strlen(toFind);i++)
  {
    isOk=1;
    for(int j=0;j<strlen(toFind);j++)
    {
      if(input[i+j]!=toFind[j])
      {
        isOk = 0;
        break;
      }        
    }
    if(isOk == 1 )
    {
      tabIndice = realloc(tabIndice,(nbMessage+1)*sizeof(int));
      tabIndice[nbMessage] = i+strlen(toFind);
      nbMessage++;
    }
  }
  (*result) = realloc((*result),nbMessage * sizeof(char*));
  for(int i=0;i<nbMessage;i++)
  {
    if(i<nbMessage-1)
    {
      (*result)[i] = malloc(tabIndice[i+1]-tabIndice[i] - strlen(toFind));
      memcpy((*result)[i],&(input[tabIndice[i]]),tabIndice[i+1]-tabIndice[i] -strlen(toFind));
      (*result)[i][tabIndice[i+1]-tabIndice[i] - strlen(toFind)]='\0';
    }else
    {
      (*result)[i] = malloc(strlen(input)-tabIndice[i]);
      memcpy((*result)[i],&(input[tabIndice[i]]),strlen(input)-tabIndice[i] );
      (*result)[i][strlen(input)-tabIndice[i]]='\0';
    }
  }
  return nbMessage;
}