#include "Utilis.h"
#include "ErrorHandler.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// STATIC

// check if cancel input (0) by user
static BOOL isCancelInput(const char *str){
  if(!str) return FALSE;
  return strcmp(str, "0") == 0 ? TRUE : FALSE;
}

static int strEqualsIgnoreCase(const char* a, const char* b){
  while(*a&&*b) {
    char ca = *a;
    char cb = *b;

    // 
    if(ca >= 'A' && ca <= 'Z') ca += 32;
    if(cb >= 'A' && cb<= 'Z') cb += 32;

    if(ca!=cb) return 0;

    a++;
    b++;
  }
  return *a == *b;
}

// clear input buffer to avoid new lines or empty inputs
static void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

// remove trail line when using files 
static void removeTrailingNewline(char *str){
  if(!str) return;
  size_t len = strlen(str);
  
  if(len > 0 && str[len-1] == '\n')
    str[len - 1] = '\0';
}

// get and check if user enter a double
static BOOL getDoubleFromUser(double* outValue,const char* prompt) {
  char input[64];
  while (TRUE)
  {
    printf("%s",prompt);
    if(!getInputFromUser(input,sizeof(input))) {
      return FALSE;
    }
    if(isCancelInput(input)) {
      return FALSE;
    }

    char* endptr;
    double val = strtod(input,&endptr);

    if(*endptr!='\0') {
      displayError(UI_WARNING,"Invalid number, please try again");
      continue;
    }
    *outValue = val;
    return TRUE;
  }
  
}

// handle getting input from user
static BOOL getLineFromUser(char* buffer,size_t size){

  if (!fgets(buffer, size, stdin)) {
    clearInputBuffer();
    displayError(UI_WARNING,"Error getting input\n");
    return FALSE;
    }

  size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] != '\n') {
      clearInputBuffer();
    }

  removeTrailingNewline(buffer);


  if (buffer[0] == '\0') {
    // empty input
    return FALSE;
  }

  return TRUE;
}


// FUNCTIONS
// convert port type from string to number
PortType portTypeFromStr (const char* str) { 
  if(!str) return SLOW;
  if(strEqualsIgnoreCase(str,"Fast")) return FAST;
  if(strEqualsIgnoreCase(str,"Slow")) return SLOW;
  if(strEqualsIgnoreCase(str,"Mid")) return MID;

  // if invalid
  fprintf(stderr,"Unknown port type: '%s'\n",str);

  return INVALID_PORT;
}

// convert port type from number to string
const char* portTypeToStr(PortType type) {
  switch (type)
  {
  case FAST: return "FAST";
  case MID: return "MID";
  case SLOW: return "SLOW";
  
  default: return "Unknown";
  }
}

// convert status from number to string
const char* statusToStr(PortStatus status) {
  switch (status)
  {
  case OCC: return "Occupied";
  case FREE: return "Free";
  case OOD: return "Out-Of-Order";
  
  default: return "Unknown";
  }
}

// check if string is numeric
BOOL isNumericString(const char* str) {
  if(!str||!*str) return FALSE;

  while(*str) {
    if(*str < '0' || *str>'9') return FALSE;
    str++;
  }
  return TRUE;
}

// get current new date
Date getCurrentDate() {
  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);

  Date date;
  date.year = tm_now->tm_year + 1900;
  date.month = tm_now->tm_mon + 1;
  date.day = tm_now->tm_mday;
  date.hour = tm_now->tm_hour;
  date.min = tm_now->tm_min;

  return date;
}

// calculate distance from coords
double calculateDistance(Coord c1, Coord c2) {
  double dx = c1.x - c2.x;
  double dy = c1.y - c2.y;
  return sqrt(dx*dx + dy*dy);
}

// calculate difference in minutes
int diffInMin(Date start,Date end) {
  struct tm tm_start = {0};
  tm_start.tm_year = start.year - 1900;
  tm_start.tm_mon = start.month - 1;
  tm_start.tm_mday = start.day;
  tm_start.tm_hour = start.hour;
  tm_start.tm_min = start.min;

  struct tm tm_end = {0};
  tm_end.tm_year = end.year - 1900;
  tm_end.tm_mon = end.month - 1;
  tm_end.tm_mday = end.day;
  tm_end.tm_hour = end.hour;
  tm_end.tm_min = end.min;

  time_t time_start = mktime(&tm_start);
  time_t time_end = mktime(&tm_end);

  if (time_start == (time_t)(-1) || time_end == (time_t)(-1)) {
        // Invalid time conversion
      return 0;
  }

  double diff_seconds = difftime(time_end, time_start);

  if (diff_seconds < 0) return 0;

  int diff_minutes = (int)(diff_seconds / 60);

  return diff_minutes;
}

// check that line not to long
int checkLineOverflow(FILE* file,char* line,size_t maxLen, int lineNum,const char* filename){
  if(strlen(line) == maxLen - 1 && line[maxLen - 2] != '\n'){
    fprintf(stderr,"Line %d too long in %s\n", lineNum, filename);
    int ch;
    while ((ch = fgetc(file)) != EOF && ch != '\n');
    return 1; //overflow
    
  }
  return 0; //no overflow
}

// HANDLE USER INPUT
// trim line
void trimNewLine(char* line) {
  if (!line) return;
  line[strcspn(line, "\r\n")] = '\0';
}

// handle generic input
BOOL getInputFromUser(char* buffer,size_t size){

  if (!fgets(buffer, size, stdin)) {
    clearInputBuffer();
    displayError(UI_WARNING,"Error getting input\n");
    return FALSE;
    }

  size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] != '\n') {
      clearInputBuffer();
    }

  removeTrailingNewline(buffer);


  if (buffer[0] == '\0') {
    // empty input
    return FALSE;
  }

  return TRUE;
}

// getting coordianates from user
BOOL getCoordFromUser(Coord *coord, const char* promptX, const char* promptY){
  if(!coord 
    || !getDoubleFromUser(&coord->x,promptX) 
    || !getDoubleFromUser(&coord->y,promptY)) 
      return FALSE;

  return TRUE;

}

// helper for check valid input for station search
BOOL parseStationInput(const char* input,SearchKey* key,SearchType *outType){
  if(!input || !key || !outType) return FALSE;

  if(isNumericString(input)) {
    key->id = atoi(input);
    *outType = SEARCH_BY_ID;
  } else {
    strncpy(key->name,input,sizeof(key->name) - 1);
    key->name[sizeof(key->name) - 1] = '\0';
    *outType = SEARCH_BY_NAME;
  }
  return TRUE;
}

//handle generic input with cancel option 
BOOL getInputAndCancel(char* buffer, size_t size, const char* prompt) {
  printf("%s",prompt);
  if(!getLineFromUser(buffer,size)) return FALSE;
  return !isCancelInput(buffer);
}
