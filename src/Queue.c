#include "../headers/Queue.h"
#include "Cars.h"
#include <stdlib.h>
#include <stdio.h>

void initQueue(qCar*queue) 
{
  queue->front = NULL;
  queue->rear = NULL;
}

qCar *createQueue(){
  qCar *queue = malloc(sizeof(qCar));
  if(!queue) {
    perror("Failed to allocate memory for queue");
    return NULL;
  }
  initQueue(queue);
  return queue;
}

void destroyQueue(qCar*queue) {
  if(!queue) return;
  while (!isEmpty(queue))
  {
    dequeue(queue); //free CarNode without Car
  }
  free(queue);
  
}

int isEmpty(const qCar *queue) 
{
  return(queue->front == NULL);
}

int enqueue(qCar *queue, Car *car) 
{
  CarNode *newNode = malloc(sizeof(CarNode));
  if(!newNode) {
    perror("Failed to allocate memory for QueueNode");
    return 0; // FAILED
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
  return 1; //SUCCESS
}

Car *dequeue(qCar *queue) {
  if(queue->front == NULL) {
    // empty queue
    return NULL;
  }
  CarNode *temp = queue->front;
  Car *car = temp->data;
  queue->front = queue->front->next;

  if(queue->front == NULL){
    // queue empty
    queue->rear = NULL;
  }
  free(temp);
  return car;
}

void printQueue(const qCar *queue) {
  if(queue == NULL || queue->front == NULL) {
    printf("Queue is empty\n");
    return;
  }

  printf("Cars in queue:\n");
  CarNode *current = queue->front;
  while (current!=NULL)
  {
    printCar(current->data);
    current=current->next;
  }
}

Car *getFront(const qCar *queue) {
  if(queue == NULL|| queue->front == NULL) {
    return NULL; //empty queue
  }

  return queue->front->data;
}
