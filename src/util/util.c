#include "util.h"
#include <stdint.h>
#include "../log/log.h"

char** str_split(char* a_str, const char a_delim, int* nbItem)
{
  char** result    = 0;
  size_t count     = 1;
  int indiceDebut  = 0;
  int indiceFin    = 0;

  for(int i=0;i<strlen(a_str);i++)
  {
    if(a_str[i]==a_delim)count++; 
  }


  result = malloc(sizeof(char*)*count);
  for(int i=0;i<count;i++)
  {
    for(int j=indiceFin;j<strlen(a_str);j++)
    {
      if(a_str[j]==a_delim)
      {
        indiceFin = j+1;
        result[i] = malloc(indiceFin-indiceDebut+1);
        memcpy(result[i],&(a_str[indiceDebut]),indiceFin-indiceDebut-1);
        result[i][indiceFin-indiceDebut-1] = '\0';
        indiceDebut = j+1;
        break;
      }else if(j==strlen(a_str)-1)
      {
        indiceFin = j;
        result[i] = malloc(indiceFin-indiceDebut+2);
        memcpy(result[i],&(a_str[indiceDebut]),indiceFin-indiceDebut+1);
        result[i][indiceFin-indiceDebut +1] = '\0';

        break;
      }
    }
  }
  *nbItem = count;
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