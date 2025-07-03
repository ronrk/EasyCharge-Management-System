#ifndef UTILIS_H
#define UTILIS_H
#include "BinaryTree.h"
#include <stdio.h>

// function pointer for parsing line from text file to struct
typedef void *(*ParseLineFunction)(const char *line);

typedef enum
{
  FAST,
  MID,
  SLOW
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

// handle enums
const char *portTypeToStr(PortType type);
PortType parsePortType(const char *str);
const char *statusToStr(PortStatus status);
PortStatus parsePortStatus(const char *str);

// skip file headers
BOOL skipHeader(FILE *file);

void loadFile(const char *path, BinaryTree *tree, ParseLineFunction parser);
#endif // UTILIS_H