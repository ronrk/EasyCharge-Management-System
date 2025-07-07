#ifndef CARS_H
#define CARS_H

#include "Port.h"
#include "Utilis.h"
#include "BinaryTree.h"
#include "Station.h"

typedef struct Car
{
  char nLicense[LICENSE_SIZE];
  PortType portType;
  double totalPayed;
  Port *pPort;  // p to port
  BOOL inqueue; // car in queue?
} Car;

int compareCars(const void *a, const void *b);

Port* getCarPort(Car* car);

// print car
void printCar(const void *data);

// parses line from .txt and create Car
Car *parseCarLine(const char *line);

// create a new car
Car *createCar(const char *license, PortType type);

// Destroy car
void destroyCar(void *data);

// search for car
Car *searchCar(const BinaryTree *carTree, const char *lisence);

// validate car
BOOL isLicenseValid(const char *license);

// find station of queue car
Station* findStationOfQueueCar(const TreeNode* node,const Car* car);

#endif