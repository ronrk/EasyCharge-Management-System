#include "ErrorHandler.h"
#include <stdio.h>

void displayError(ErrorCode code,const char* message) {

  switch (code)
  {
  case ERR_LOADING_DATA:
    printf("Loading Data Error: %s\n",message);
    break;
  case ERR_MEMORY:
    printf("Memory Error: %s\n",message);
    break;
  case  ERR_FILE:
    printf("File Error: %s\n",message);
    break;
  case ERR_PARSING:
    printf("Parsing Error: %s\n",message);
    break;
  case UI_WARNING:
    printf("%s\n",message);
    break;
  default:
    printf("Unknown error occurred\n");
    break;
  }

};