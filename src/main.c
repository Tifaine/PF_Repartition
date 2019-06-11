// file auto generated

#include "./sharedMem/sharedMem.h"
#include "./log/log.h"
#include "./freeOnExit/freeOnExit.h"
#include <stdint.h>
#include "./config/config_arg.h"
#include "./config/config_file.h"
#include "Client_TCP/clientTCP.h"
#include "plateforme/plateforme.h"
#include "util/util.h"
// INIT_FUNCTION

int main ( int argc, char ** argv )
{
	// INIT_VAR
	Plateforme pf;


	struct
	{
		#ifdef __LOG_H__
		uint8_t help:1,
			quiet:1,
			verbose:1,
			color:1,
			debug:1,
			term:1,
			file:1;
		#else
		uint8_t help:1,
			unused:6; // to have same allignement with and without debug/color/quiet flags
		#endif
	}
	flags;
	char logFileName[ 512 ] = "";
 
	config_el config[] =
	{
		{ NULL }
	};
 
	param_el param[] =
	{
		{ "--help", "-h", 0x01, cT ( bool ), &flags, "help" },
		#ifdef __LOG_H__
		{ "--quiet", "-q", 0x02, cT ( bool ), &flags, "quiet" },
		{ "--verbose", "-v", 0x04, cT ( bool ), &flags, "verbose" },
		{ "--color", "-c", 0x08, cT ( bool ), &flags, "color" },
		{ "--debug", "-d", 0x10, cT ( bool ), &flags, "debug" },
		{ "--term", "-lT", 0x20, cT ( bool ), &flags, "log on term" },
		{ "--file", "-lF", 0x40, cT ( bool ), &flags, "log in file" },
		{ "--logFileName", "-lFN", 1, cT ( str ), logFileName, "log file name" },
		#endif
		{ NULL }
	};
 
	// INIT_CORE
	// INIT_FREEONEXIT
	if ( initFreeOnExit ( ) )
	{ // failure case
	}
	// END_FREEONEXIT
	
	// INIT_CONFIG
	if ( readConfigFile ( "configFilePath", config ) )
	{ // failure case
	}
	else if ( readConfigArgs ( argc, argv, config ) )
	{ // failure case
	}
	else if ( readParamArgs ( argc, argv, param ) )
	{ // failure case
	}
	else if ( flags.help )
	{// configFile read successfully
		helpParamArgs ( param );
		helpConfigArgs ( config );
	}
	// END_CONFIG
	// INIT_LOG
	#ifdef __CONFIG_DATA_H__
	logSetVerbose ( flags.verbose );
	logSetDebug ( flags.debug );
	logSetColor ( flags.color );
	logSetQuiet ( flags.quiet );
	logSetOutput ( flags.term, flags.file );
	logSetFileName ( logFileName );
	#else
	logSetVerbose ( 1 );
	logSetDebug ( 1 );
	logSetColor ( 1 );
	logSetQuiet ( 0 );
	logSetOutput ( 1, 1 );
	logSetFileName ( "log.txt" );
	#endif
	// END_LOG
	
	
	// END_CORE
	initPF(&pf, "Linux_X86_PF");
	client_TCP_init_connec("127.0.0.1",12345,&pf);
	PF_run(&pf);
	
	while(1);
	return ( 0 );
}

