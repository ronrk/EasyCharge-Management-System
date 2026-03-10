#ifndef QUEUE_H
#define QUEUE_H

#include "Utilis.h"

// STRUCT & Enums declarations
typedef struct Car Car;
typedef struct CarNode
{
  Car *data;
  struct CarNode *next;
} CarNode;

typedef struct qCar
{
  CarNode *front;
  CarNode *rear;
} qCar;

// FUNCTIONS
qCar *createQueue();
BOOL enqueue(qCar *queue, Car *car);
void destroyQueue(qCar*queue);
int isEmpty(const qCar *queue);
Car *getFront(const qCar *queue);
int countQueueItems(const qCar* queue);
Car* dequeueByPortType(qCar* queue,PortType portType);

#endif