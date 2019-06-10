// file auto generated

#include "./sharedMem/sharedMem.h"
#include "./freeOnExit/freeOnExit.h"
#include "Client_TCP/clientTCP.h"
// INIT_FUNCTION

int main ( int argc, char ** argv )
{
	// INIT_VAR
	// INIT_CORE
	
	
	// END_CORE
	// INIT_FREEONEXIT
	if ( initFreeOnExit ( ) )
	{ // failure case
	}
	// END_FREEONEXIT


	client_TCP_init_connec("127.0.0.1",12345);

	client_TCP_envoi_message("Linux_x86_PF",01,"Salut !");
	client_TCP_envoi_message("Linux_x86_PF",02,"A Bientôt");
	while(1);
	return ( 0 );
}

