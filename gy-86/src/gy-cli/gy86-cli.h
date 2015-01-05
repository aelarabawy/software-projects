/*
 * gy86-cli.h
 *
 *  Created on: Jan 5, 2015
 *      Author: aelarabawy
 */

#ifndef GY86_CLI_H_
#define GY86_CLI_H_

extern gyHandle g_gyHandle;
#define CONFIG_INVALID   (0xFF)

#define PRINT_CLI(logMsg, argList...)                                                                  \
    printf(logMsg, ##argList);																		   \
    printf("\n")

#endif /* GY86_CLI_H_ */
