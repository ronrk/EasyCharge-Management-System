#ifndef QUEUE_H
#define QUEUE_H

#include "Utilis.h"

typedef struct Car Car;
typedef struct CarNode CarNode;
typedef struct qCar qCar;

struct CarNode
{
  Car *data;
  struct CarNode *next;
};

struct qCar
{
  CarNode *front;
  CarNode *rear;
};

// funcytions
void initQueue(qCar *queue);

qCar *createQueue();
void destroyQueue(qCar *queue);

int isEmpty(const qCar *queue);
BOOL enqueue(qCar *queue, Car *car);
Car *dequeue(qCar *queue);

Car *dequeueByPortType(qCar *queue, PortType portType);

Car *getFront(const qCar *queue);
void printQueue(const qCar *queue);

// count queue items
int countQueueItems(const qCar *queue);
#endif