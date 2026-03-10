#ifndef STATION_H
#define STATION_H

#include "Utilis.h"

// STRUCT & Enums declarations
typedef struct Port Port;
typedef struct qCar qCar;
typedef struct Car Car;
typedef struct BinaryTree BinaryTree;

typedef struct
{
  unsigned int id;
  char *name;
  int nPorts;
  Coord coord;
  Port *portsList;
  int nCars;
  qCar *qCar;
} Station;

// FUNCTIONS
Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord);
void printStation(const void *data);
int compareStationById(const void *a, const void *b);
void StationDestroy(void *data);
void *Station_parseLine(const char *line);
void addPortToStation(Station *station, Port *port, BOOL increment);
BOOL enqueueCarToStationQueue(Station *station, Car *car);
Station *searchStation(const BinaryTree *tree, SearchKey *key);
Station *findStationByCar(BinaryTree *stationTree, Car *car);
Station *findStationByPort(const BinaryTree *tree, const Port *port);
void printStationSummary(const void *data);

#endif