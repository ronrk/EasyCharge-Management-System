#ifndef QUEUE_H
#define QUEUE_H
typedef struct Car Car;

typedef struct CarNode
{
  Car *data;
  struct CarNode *next;
} CarNode;

typedef struct
{
  CarNode *front;
  CarNode *rear;
} qCar;

// funcytions
void initQueue(qCar *queue);
qCar *createQueue();
void destroyQueue(qCar *queue);

int isEmpty(const qCar *queue);
int enqueue(qCar *queue, Car *car);
Car *dequeue(qCar *queue);

Car *getFront(const qCar *queue);
void printQueue(const qCar *queue);
#endif