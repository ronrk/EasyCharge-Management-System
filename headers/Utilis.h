#ifndef UTILIS_H
#define UTILIS_H

#define LICENSE_SIZE 9
#define MAX_NAME 100
#define RATE_CHARGE 1.2
#define EMPTY_LICENSE "-1"

#include <stdio.h>


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

// enums
typedef enum
{
  FAST,
  MID,
  SLOW,
  INVALID_PORT
} PortType;
typedef enum
{
  FALSE,
  TRUE
} BOOL;
typedef enum
{
  OCC = 1,
  FREE,
  OOD
} PortStatus;

// struct
typedef struct
{
  double x;
  double y;
} Coord;
typedef struct
{
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int min;
} Date;

// functions

Date getCurrentDate();
int diffInMin(Date start, Date end);
Date createDate(int y, int m, int d, int h,int min);

// clear input buffer avoid new line
void clearInputBuffer();
// remove new line
void removeTrailingNewline(char *str);
// if input is a cancel signal
BOOL isCancelInput(const char *str);

// get and validate coords
BOOL getCoordFromUser(Coord *coord, const char *promptX, const char *promptY);
BOOL getDoubleFromUser(double *outValue, const char *prompt);

// handle enums to Str and opposite
const char *portTypeToStr(PortType type);
PortType Util_parsePortType(const char *str);
const char *statusToStr(PortStatus status);
PortStatus Util_parsePortStatus(const char *str);

// calculate distance
double calculateDistance(Coord c1, Coord c2);

int strEqualsIgnoreCase(const char *a, const char *b);

BOOL isNumericString(const char *str);

BOOL getLineFromUser(char *buffer, size_t size);

BOOL getInputAndCancel(char *buffer, size_t size, const char *prompt);

BOOL parseStationInput(const char *input, SearchKey *key, SearchType *outType);

void trimNewLine(char *line);

int checkLineOverflow(FILE *file, char *line, size_t maxLen, int lineNum, const char *filename);

#endif // UTILIS_H