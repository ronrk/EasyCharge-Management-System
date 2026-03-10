#ifndef UTILIS_H
#define UTILIS_H

#include <stdio.h>

#define LICENSE_SIZE 9
#define MAX_NAME 100
#define RATE_CHARGE 1.2

// STRUCT & Enums declarations
// search helpers
typedef enum
{
  SEARCH_BY_ID,
  SEARCH_BY_NAME,
  SEARCH_BY_DISTANCE,
  SEARCH_TYPE_COUNT
} SearchType;

typedef struct
{
  SearchType type;
  union
  {
    char name[MAX_NAME];
    int id;
  };
  struct
  {
    double userX;
    double userY;
  } location;
} SearchKey;
// structs
typedef struct
{
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int min;
} Date;
typedef struct
{
  double x;
  double y;
} Coord;
// enums
typedef enum
{
  FAST,
  MID,
  SLOW,
  INVALID_PORT = -1
} PortType;
typedef enum
{
  OCC = 1,
  FREE,
  OOD
} PortStatus;
typedef enum
{
  FALSE,
  TRUE
} BOOL;

// FUNCTIONS
PortType portTypeFromStr (const char* str);
const char* portTypeToStr(PortType type);
void trimNewLine(char* line);
BOOL getInputFromUser(char* buffer,size_t size);
BOOL isNumericString(const char* str);
int checkLineOverflow(FILE* file,char* line,size_t maxLen, int lineNum,const char* filename);
BOOL getCoordFromUser(Coord *coord, const char* promptX, const char* promptY);
double calculateDistance(Coord c1, Coord c2);
const char* statusToStr(PortStatus status);
Date getCurrentDate();
BOOL getInputAndCancel(char* buffer, size_t size, const char* prompt);
BOOL parseStationInput(const char* input,SearchKey* key,SearchType *outType);
int diffInMin(Date start,Date end);

#endif