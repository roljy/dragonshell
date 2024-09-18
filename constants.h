// constants.h
// Tawfeeq Mannan

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define LINE_LENGTH 100  // num of characters per input line
#define MAX_ARGS 5  // num of args per command
#define MAX_LENGTH 20  // num of characters per arg
#define MAX_BG_PROC 1  // num of background processes

typedef enum
{
    EC_SUCCESS,
    EC_CD_NO_ARGS,
    EC_CD_PATH_NOT_FOUND,
    EC_UNKNOWN_CMD,
} ErrCode;

#endif  // _CONSTANTS_H
