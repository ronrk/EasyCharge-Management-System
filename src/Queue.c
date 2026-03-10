#include "Queue.h"
#include "Utilis.h"
#include "Car.h"
#include "ErrorHandler.h"

#include <stdio.h>
#include <stdlib.h>

// STATIC
// remove car node
static Car* removeCarNode(qCar* queue,CarNode *prev, CarNode *toRemove){
  if(!queue||!toRemove) return NULL;

  if(prev ==NULL) {
    // remove front
    queue->front = toRemove->next;
    if(queue->front == NULL) {
      queue->rear = NULL;
    }
  } else {
    prev->next = toRemove->next;
    if(toRemove == queue->rear) {
      queue->rear = prev;
    }
  }

  Car *car = toRemove->data;
  free(toRemove);
  return car;
}

// FUNCTIONS
// initilize queue
void initQueue(qCar*queue) 
{
  queue->front = NULL;
  queue->rear = NULL;
}

// create dynamic Car queue
qCar *createQueue() {
  qCar *queue = malloc(sizeof(qCar));
  if(!queue) {
    perror("Failed to allocate memory for queue");
    return NULL;
  }
  initQueue(queue);
  return queue;
}

// enqueue car to car queue
BOOL enqueue(qCar *queue, Car *car) 
{
  if(!car||!queue) return FALSE;
  CarNode *newNode = malloc(sizeof(CarNode));
  if(!newNode) {
    displayError(ERR_MEMORY,"Failed to allocate memory for QueueNode [enqueue]");
    return FALSE; // FAILED
  }
  newNode->data = car;
  newNode->next = NULL;

  if(queue->rear == NULL) {
    // empty queue
    queue->front = queue->rear = newNode;
  } else {
    queue->rear->next = newNode;
    queue->rear = newNode;
  }
  return TRUE; //SUCCESS
}

// destroy queue
void destroyQueue(qCar*queue) {
  CarNode* current = queue->front;
  while (current)
  {
    CarNode *tmp = current;
    current->data->inqueue = 0;
    current = current->next;
    free(tmp);
  }
  free(queue);
  
}

// check if queue is empty
int isEmpty(const qCar *queue) 
{
  return(queue->front == NULL);
}

// get the first car in queue
Car *getFront(const qCar *queue) {
  if(queue == NULL|| queue->front == NULL) {
    return NULL; //empty queue
  }

  return queue->front->data;
}

// count queue size
int countQueueItems(const qCar* queue) {
  if(!queue || !queue->front) return 0;

  int count = 0;
  CarNode* current = queue->front;
  while (current)
  {
    count++;
    current = current->next;
  }

  return count;
}

// dequeue car from list by port type
Car* dequeueByPortType(qCar* queue,PortType portType){
  if(!queue||isEmpty(queue)) {
    return NULL;
  }

  CarNode* current = queue->front;
  CarNode *prev = NULL;

  while (current)
  {
    if(current->data&&current->data->portType == portType) {
      return removeCarNode(queue,prev,current);
    }

    prev = current;
    current = current->next;
  }
  return NULL;
}