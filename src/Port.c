#include "../headers/Port.h"
#include "../headers/Cars.h"
// #include "../headers/Utilis.h"
#include <stdlib.h>
#include <stdio.h>

Port *createPort(unsigned int num, PortType type,PortStatus status,Car* car,Date t) {
  Port* newPort = malloc(sizeof(Port));
  if(!newPort) {
    perror("Failed alocate memory on Port");
    return NULL;
  }
  newPort->num = num;
  newPort->portType = type;
  newPort->status = status;
  newPort->p2Car = car;

  // initialize time
  newPort->tin.year=t.year;
  newPort->tin.month=t.month;
  newPort->tin.day = t.day;
  newPort->tin.hour = t.hour;
  newPort->tin.min = t.min;

  newPort->next = NULL;

  return newPort;
}

Port *insertPort(Port* head, Port*newPort) {
  if(!newPort) return head;

  newPort->next = head;
  return newPort;
}

Port *findPort(Port *head, unsigned int num){
  Port* current = head;

  while (current != NULL)
  {
    if(current->num == num) {
      return current;
    }
    current = current->next;
  }
 
  return NULL;
}


Port *removePort(Port *head, unsigned int num) {
  if(head==NULL) return NULL;

  // remove head
  if(head->num == num) {
    Port *next = head->next;
    free(head);
    return next;
  }

  Port *prev = head;
  Port *current = head->next;
  while (current!=NULL)
  {
    if(current->num == num) {
      prev->next = current->next;
      free(current);
      return head;
    }
    prev= current;
    current = current->next;
  }
  
  // cant find port
  return head;
}

void printPortbyNum(Port *head, unsigned int num) {
  const Port* current = head;

  while (current!=NULL)
  {
    if(current->num==num) {
      printPort(current);
      return;
    }
    current = current->next;
  }
  printf("Port number %u was not found\n",num);
}

void printPortList(Port *head) {
  if(head== NULL) 
  {
    printf("No Ports\n");
    return;
  }

  printf("---PortList---\n");
  Port* current = head;
  while (current != NULL) {
    printPort(current);
    current=current->next;
  }
  printf("------\n");
  
}

void printPort(const Port* port) {
  printf("Port Number: %u  |  Type: %s  |  Status: %s\n",port->num,portTypeToStr(port->portType),statusToStr(port->status));

      if(port->p2Car) {
        printf("  Charging Car: %s\n",port->p2Car->nLicense);

        printf("Since: %04d-%02d-%02d %02d:%02d\n",port->tin.year,port->tin.month,port->tin.day,port->tin.hour,port->tin.min);
      } else {
        printf("  Charging Car: None\n");
      }
}

void destroyPortList(Port *head) {
  Port* current = head;
  while (current != NULL)
  {
    Port* tmp = current;
    current = current->next;

    free(tmp);
  }
  
}

void destroyPort(Port *port){
  if(port){
    free(port);
  }
}