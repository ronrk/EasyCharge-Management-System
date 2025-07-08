#ifndef CARS_H
#define CARS_H

#include "Utilis.h"

typedef struct Port Port;
typedef struct Station Station;
typedef struct BinaryTree BinaryTree;
typedef struct TreeNode TreeNode;

typedef struct Car Car;
struct Car
{
  char nLicense[LICENSE_SIZE];
  PortType portType;
  double totalPayed;
  Port *pPort;  // p to port
  BOOL inqueue; // car in queue?
};

int compareCarsByLicense(const void *a, const void *b);

Port *getCarPort(Car *car);

// print car
void printCar(const void *data);

// parses line from .txt and create Car
Car *parseCarLine(const char *line);

// create a new car
Car *createCar(const char *license, PortType type);

// Destroy car
void destroyCar(void *data);

// search for car
Car *searchCar(const BinaryTree *carTree, const char *license);

// validate car
BOOL isLicenseValid(const char *license);

// find station of queue car
Station *findStationOfQueueCar(const TreeNode *node, const Car *car);

#endif