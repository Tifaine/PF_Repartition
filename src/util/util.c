#include "util.h"
#include <stdint.h>


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
    if(idx != count - 1)
    {
      printf("ERREUR %s\n",a_str);
      return NULL;
    }
    *(result + idx) = 0;
  }
  *nbItem = count-1;

  return result;
}

int findSubstring(char* input, char* toFind, char*** result)
{
  int nbMessage = 0;
  const int lengtOfFind = strlen( toFind );
  int* tabIndice = malloc( sizeof( int ) );
  if ( !tabIndice )
  {
    return( -1 );
  }

  for ( int i = 0; i < strlen( input ) - lengtOfFind; i++ )
  {
    uint8_t isOk = 1;
    for ( int j = 0; j < lengtOfFind; j++ )
    {
      if ( input[ i + j ] != toFind[ j ] )
      {
        isOk = 0;
        break;
      }        
    }
    if ( isOk == 1 )
    {
      void *tmp = realloc( tabIndice, (nbMessage+1) * sizeof( int ) );
      if ( !tmp )
      { //failure case
        // TODO
        continue;
      }
      tabIndice = tmp;
      tabIndice[ nbMessage ] = i + lengtOfFind;
      nbMessage++;
      i += lengtOfFind;
    }
  }

  void *tmp = realloc( (*result), nbMessage * sizeof( char* ) );
  if ( !tmp )
  {
    return ( -1 );
  }
  (*result) = tmp;

  for ( int i = 0; i < nbMessage; i++ )
  {
    if ( i < ( nbMessage - 1 ) )
    {
      int msgLength = tabIndice[ i + 1 ] - tabIndice[ i ] - lengtOfFind;
      (*result)[ i ] = malloc( msgLength + 1 );
      if ( (*result)[ i ] )
      {
        memcpy( (*result)[ i ], input + tabIndice[ i ], msgLength );
        (*result)[ i ][ msgLength ] = 0;
      }
      else
      { // failure case
        // TODO
      }
    }
    else
    {
      int msgLength = strlen( input ) - tabIndice[ i ];
      (*result)[ i ] = malloc( msgLength + 1 );
      if ( (*result)[ i ] )
      {
        strcpy( (*result)[ i ], input + tabIndice[ i ] );
      }
      else
      { // failure case
        // TODO
      }
    }
  }
  free ( tabIndice );

  return nbMessage;
}