#ifndef UTILIS_H
#define UTILIS_H

#include "BinaryTree.h"
#include <stdio.h>

// function pointer for parsing line from text file to struct
typedef void *(*FileLineProcessor)(const char *line, void *context);
int processFileLines(const char *filename, FileLineProcessor processor, void *context, int skipHeader);

// file loader types
typedef void *(*ParseLineFunc)(const char *line);
typedef void (*PostProcessFunction)(void *obj, void *context);

typedef struct
{
  const char *filename;             // file to load
  BinaryTree *targetTree;           // tree to insert to
  ParseLineFunc parser;             // function to parse a line
  PostProcessFunction processor;    // optional post processor
  void *context;                    // additional data for process
  void (*destroyObject)(void *obj); // object destroy
  int skipHeader;                   // skip header?
} FileLoaderConfig;

int loadDataFile(const FileLoaderConfig *config);

// enums
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

// clear input buffer avoid new line
void clearInputBuffer();

// handle enumstoStr and opposite
const char *portTypeToStr(PortType type);
PortType parsePortType(const char *str);
const char *statusToStr(PortStatus status);
PortStatus parsePortStatus(const char *str);

// calculate distance
double calculateDistance(Coord c1, Coord c2);

#endif // UTILIS_H