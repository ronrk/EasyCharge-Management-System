#ifndef CARS_H
#define CARS_H

#include "Utilis.h"
#include "Station.h"
#include "BinaryTree.h"

// STRUCT & Enums declarations
typedef struct Port Port;
typedef struct BinaryTree BinaryTree;

typedef struct Car
{
  char nLicense[LICENSE_SIZE];
  PortType portType;
  double totalPayed;
  Port *pPort;  // ptr to port
  BOOL inqueue; // is car in queue?
} Car;

// FUNCTIONS
void printCar(const void* data);
int compareCarsByLicense(const void *a, const void *b);
void CarDestroy(void *data);
void* Car_parseLine(const char* line);
Car* searchCar(const BinaryTree *carTree,const char *license);
BOOL isLicenseValid(const char* license);
Car *createCar(const char *license, PortType type);
Port* getCarPort(Car* car);
Station* findStationOfQueueCar(const TreeNode* node,const Car* car);
int compareCarsByTotalPayed(const void* a,const void*b);
void collectCarsInArray(TreeNode* node,Car** carrArr,int* count);

#endif