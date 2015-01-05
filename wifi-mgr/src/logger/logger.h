/*
 * logger.h
 *
 *  Created on: Dec 10, 2014
 *      Author: aelarabawy
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "common.h"
#include "wpa_supplicant_common.h"

#define ENABLE_LOGGING 1


#if ENABLE_LOGGING

typedef enum {
	LT_UNKNOWN = 0,
	LT_ENTER,
	LT_ENTER_NO_ARGS,
	LT_EXIT,
	LT_EXIT_NO_ARGS,
	LT_EXIT_WITH_ERROR,
	LT_EXIT_WITH_ERROR_NO_ARGS,
	LT_ERROR,
	LT_WARNING,
	LT_INFO,
	LT_PROGRESS,
	LT_DEBUG,
	LT_NULL_POINTER,
	LT_ALLOC_FAIL,
    LT_FUNCTION_CALL,
	LT_FUNCTION_CALLED,
	LT_LAST,
} LogType;


void log_txt (LogType type,
		      const char* logMsg,
			  const char *func,
			  const char* date,
			  const char* time,
			  const char* file,
			  const unsigned int line, ...);


//Entering Functions
#define ENTER()                                                                                    \
	log_txt(LT_ENTER_NO_ARGS, NULL, __func__ , __DATE__,  __TIME__, __FILE__, __LINE__)

#define ENTER_FUNC(logMsg, argList...)                                                                  \
    log_txt(LT_ENTER, logMsg, __func__ , __DATE__,  __TIME__, __FILE__, __LINE__, ##argList)

//Exiting from Functions
#define EXIT()                                                                                       \
	log_txt(LT_EXIT_NO_ARGS, NULL, __func__ , __DATE__,  __TIME__, __FILE__, __LINE__)

#define EXIT_FUNC(logMsg, argList...)                                                                   \
    log_txt(LT_EXIT, logMsg, __func__ , __DATE__,  __TIME__, __FILE__, __LINE__, ##argList)

#define EXIT_WITH_ERROR()                                                        \
    log_txt(LT_EXIT_WITH_ERROR_NO_ARGS, NULL, __func__ , __DATE__,  __TIME__, __FILE__, __LINE__)

#define EXIT_FUNC_WITH_ERROR(logMsg, argList...)                                                        \
    log_txt(LT_EXIT_WITH_ERROR, logMsg, __func__ , __DATE__,  __TIME__, __FILE__, __LINE__, ##argList)

//Errors
#define ERROR(logMsg, argList...)                                                                  \
    log_txt(LT_ERROR, logMsg, __func__ , __DATE__ , __TIME__, __FILE__, __LINE__, ##argList)

//Warning
#define WARN(logMsg, argList...)                                                                \
    log_txt(LT_WARNING, logMsg, __func__ , __DATE__ , __TIME__, __FILE__, __LINE__, ##argList)

//Progress
#define PROGRESS(logMsg, argList...)                                                               \
	log_txt(LT_PROGRESS, logMsg, __func__ , __DATE__ , __TIME__, __FILE__, __LINE__, ##argList)

//Info
#define INFO(logMsg, argList...)                                                                   \
    log_txt(LT_INFO, logMsg, __func__ , __DATE__, __TIME__, __FILE__, __LINE__, ##argList)

//Debug
#define DEBUG(logMsg, argList...)                                                                  \
    log_txt(LT_DEBUG, logMsg, __func__ , __DATE__, __TIME__, __FILE__, __LINE__, ##argList)


//Special Errors
#define NULL_POINTER(logMsg, argList...)                                                           \
    log_txt(LT_NULL_POINTER, logMsg, __func__ , __DATE__, __TIME__, __FILE__, __LINE__, ##argList)

#define ALLOC_FAIL(logMsg, argList...)                                                            \
    log_txt(LT_ALLOC_FAIL, logMsg, __func__ , __DATE__, __TIME__, __FILE__, __LINE__, ##argList)

//Calling a Function
#define FUNCTION_CALL(fnName, argList...)                                                          \
    log_txt(LT_FUNCTION_CALL, fnName, __func__ , __DATE__, __TIME__, __FILE__, __LINE__, ##argList)

#define FUNCTION_CALLED(fnName, argList...)                                                        \
	log_txt(LT_FUNCTION_CALL, fnName, __func__ , __DATE__, __TIME__, __FILE__, __LINE__, ##argList)



#else

#define ENTER()
#define ENTER_FUNC(logMsg, argList...)

//Exiting from Functions
#define EXIT()
#define EXIT_FUNC(logMsg, argList...)
#define EXIT_WITH_ERROR()
#define EXIT_FUNC_WITH_ERROR(logMsg, argList...)

//Errors
#define ERROR(logMsg, argList...)

//Warning
#define WARN(logMsg, argList...)

//Progress
#define PROGRESS(logMsg, argList...)

//Info
#define INFO(logMsg, argList...)

//Debug
#define DEBUG(logMsg, argList...)


//Special Errors
#define NULL_POINTER(logMsg, argList...)
#define ALLOC_FAIL(logMsg, argList...)

//Calling a Function
#define FUNCTION_CALL(fnName, argList...)
#define FUNCTION_CALLED(fnName, argList...)


#endif


#endif /* _LOGGER_H_ */
