#ifndef STATION_H
#define STATION_H
#include "Utilis.h"
#include "Queue.h"
#include "Port.h"

typedef struct Station Station;
struct Station
{
  unsigned int id;
  char *name;
  int nPorts;
  Coord coord;
  Port *portsList;
  int nCars;
  qCar *qCar;
  Station *left;
  Station *right;
};

Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord);

void StationDestroy(void *data);
int compareStation(const void *a, const void *b);
void printStation(const void *data);

Station *insertStation(Station *root, Station *newStation);
void inorderStationTraversal(Station *root);
void destroyStationTree(Station *root);
void *parseStationLine(const char *line);

#endif