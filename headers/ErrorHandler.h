#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

typedef enum
{
  ERR_LOADING_DATA = 1,
  ERR_MEMORY,
  ERR_PARSING,
  ERR_FILE,
  UI_WARNING
} ErrorCode;

void displayError(ErrorCode code,const char* message);

#endif