#ifndef TEST_U
#define TEST_U

#include "SystemData.h"
#include "Station.h"
#include "BinaryTree.h"
#include "Queue.h"
#include "Utilis.h"
#include "Cars.h"
#include <stdio.h>

void testLoadFiles();
void printSingleStationQueue(const void *data);
void printStationQueues(SystemData *sys);
void testQueueOperations();
void assignCarsToAvailablePorts(SystemData *sys, Date now);
void printStationPorts(const SystemData *sys);
void printCarQueue(qCar* queue, const char* stationName);

#endif