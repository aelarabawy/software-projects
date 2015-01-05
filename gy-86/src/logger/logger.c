/*
 * logger.c
 *
 *  Created on: Dec 10, 2014
 *      Author: aelarabawy
 */

#include "logger.h"

#define USE_SYSLOG 0

#if USE_SYSLOG
  #include <syslog.h>
  int facility = LOG_USER;
  static int g_priority = 0;
#else
  static FILE *g_stream = NULL;
#endif

static int g_spaceCount = 0;

int log_init (char *fileName) {
#if USE_SYSLOG
#else
	g_stream = fopen(fileName, "w");
	if(!g_stream) {
		printf("Can not Initialize the Logger\n");
		return -1;
	}
#endif

	return 0;
}


void log_txt (LogType type,
		      const char* logMsg,
			  const char *func,
			  const char* date,
			  const char* time,
			  const char* file,
			  const unsigned int line, ...) {
#if USE_SYSLOG
#else
	if (!g_stream) {
		log_init("log-file.txt");
	}
#endif

	char fullMsg[200];
	va_list arglist;

	char dateTime[100];
#if USE_SYSLOG
	sprintf(dateTime,"");
#else
	sprintf(dateTime,"%s | %s", date,time);
#endif

	switch(type) {
	case LT_ENTER_NO_ARGS:
#if USE_SYSLOG
		g_priority = facility | LOG_NOTICE;
#endif
		g_spaceCount++;
	    sprintf(fullMsg,"%s | ENTER |%*s%s(%d): Entering Function: %s()\n", dateTime,g_spaceCount," ", file, line, func);
		break;

	case LT_ENTER:
#if USE_SYSLOG
		g_priority = facility | LOG_NOTICE;
#endif

		g_spaceCount++;
	    sprintf(fullMsg,"%s | ENTER |%*s%s(%d): Entering Function: %s() :%s\n", dateTime,g_spaceCount," ", file, line,func, logMsg);
		break;

	case LT_EXIT_NO_ARGS:
#if USE_SYSLOG
		g_priority = facility | LOG_NOTICE;
#endif

	    sprintf(fullMsg,"%s | EXIT  |%*s%s(%d): Exiting Function: %s()\n", dateTime,g_spaceCount," ", file, line, func);
		g_spaceCount--;
		break;

	case LT_EXIT:
#if USE_SYSLOG
		g_priority = facility | LOG_NOTICE;
#endif

	    sprintf(fullMsg,"%s | EXIT  |%*s%s(%d): Exiting Function: %s() :%s\n", dateTime, g_spaceCount," ",file, line,func, logMsg);
		g_spaceCount--;
		break;

	case LT_EXIT_WITH_ERROR:
#if USE_SYSLOG
		g_priority = facility | LOG_ERR;
#endif

	    sprintf(fullMsg,"%s | ERROR |%*s%s(%d): Exiting Function On Error: %s() :%s\n", dateTime,g_spaceCount," ", file, line, func, logMsg);
		g_spaceCount--;
		break;

	case LT_EXIT_WITH_ERROR_NO_ARGS:
#if USE_SYSLOG
		g_priority = facility | LOG_ERR;
#endif

	    sprintf(fullMsg,"%s | ERROR |%*s%s(%d): Exiting Function On Error: %s()\n", dateTime,g_spaceCount," ", file, line, func);
		g_spaceCount--;
		break;

	case LT_ERROR:
#if USE_SYSLOG
		g_priority = facility | LOG_ERR;
#endif

	    sprintf(fullMsg,"%s | ERROR |%*s%s(%d): %s \n", dateTime,g_spaceCount," ", file, line, logMsg);
		break;

	case LT_WARNING:
#if USE_SYSLOG
		g_priority = facility | LOG_WARNING;
#endif

	    sprintf(fullMsg,"%s | WARN  |%*s%s(%d): %s \n", dateTime,g_spaceCount," ", file, line, logMsg);
		break;

	case LT_INFO:
#if USE_SYSLOG
		g_priority = facility | LOG_INFO;
#endif

	    sprintf(fullMsg,"%s | INFO  |%*s%s(%d): %s \n", dateTime,g_spaceCount," ", file, line, logMsg);
		break;

	case LT_PROGRESS:
#if USE_SYSLOG
		g_priority = facility | LOG_NOTICE;
#endif

	    sprintf(fullMsg,"%s | PROG  |%*s%s(%d): %s \n", dateTime,g_spaceCount," ", file, line, logMsg);
		break;

	case LT_DEBUG:
#if USE_SYSLOG
		g_priority = facility | LOG_DEBUG;
#endif

	    sprintf(fullMsg,"%s | DEBUG |%*s%s(%d): %s \n", dateTime,g_spaceCount," ", file, line, logMsg);
		break;

	case LT_NULL_POINTER:
#if USE_SYSLOG
		g_priority = facility | LOG_ERR;
#endif

	    sprintf(fullMsg,"%s | ERROR |%*s%s(%d): NULL_POINTER :  %s \n", dateTime, g_spaceCount," ",file, line, logMsg);
		break;

	case LT_ALLOC_FAIL:
#if USE_SYSLOG
		g_priority = facility | LOG_ERR;
#endif

	    sprintf(fullMsg,"%s | ERROR |%*s%s(%d): ALLOCATION FAILURE : %s \n", dateTime,g_spaceCount," ", file, line, logMsg);
		break;

	case LT_FUNCTION_CALL:
#if USE_SYSLOG
		g_priority = facility | LOG_DEBUG;
#endif

	    sprintf(fullMsg,"%s | PROG  |%*s%s(%d): Calling Function:  %s \n", dateTime, g_spaceCount," ",file, line, logMsg);
		break;

	case LT_FUNCTION_CALLED:
#if USE_SYSLOG
		g_priority = facility | LOG_DEBUG;
#endif

	    sprintf(fullMsg,"%s | PROG  |%*s%s(%d): Function Called:  %s \n", dateTime, g_spaceCount," ",file, line, logMsg);
		break;

	default:
		break;
	}

    va_start( arglist, line );
#if USE_SYSLOG
    vsyslog(g_priority,fullMsg,arglist);
#else
    vfprintf(g_stream,fullMsg, arglist);
#endif

    va_end(arglist);
#if USE_SYSLOG
#else
    fflush(g_stream);
#endif
}
