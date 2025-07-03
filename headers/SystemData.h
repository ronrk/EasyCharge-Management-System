#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include "Station.h"
#include "BinaryTree.h"
#include "Cars.h"

typedef struct
{
  BinaryTree carTree;     //  cars binary tree
  BinaryTree stationTree; //  stations binary tree
} SystemData;

// Functions
// load files
SystemData *loadFiles();
// destroy files
void destroyFiles(SystemData *sys);

// load individual files
void stationsLoad(BinaryTree *stationTree);
void portsLoad(BinaryTree *stationTree, BinaryTree *carTree);
void loadCars(SystemData *sys);

void loadLineOfCars(SystemData *sys);
#endif