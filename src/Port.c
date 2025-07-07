#include "../headers/Port.h"
#include "../headers/Cars.h"
// #include "../headers/Utilis.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>


Port *createPort(unsigned int num, PortType type,PortStatus status,Date t) {
  Port* port = (Port*)malloc(sizeof(Port));
  if(!port) {
    perror("Failed alocate memory on Port");
    return NULL;
  }

  port->num = num;
  port->portType = type;
  port->status = status;
  port->p2Car = NULL;
  port->tin = t;

  port->next = NULL;

  return port;
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

Port* findAvailablePort(Port* portList, PortType type) {
  printf("[DEBUG] Scanning port list for type: %s\n", portTypeToStr(type));
  Port* current = portList;

  while (current)
  {
      if(current->status == FREE && isCompatiblePortType(type,current->portType)) {
        printf("[DEBUG] Found matching port: #%u (Status: %s)\n",
       current->num, statusToStr(current->status));
        return current;
      }
      current = current->next;

  }
  
  return NULL;
}

BOOL assignCar2Port(Port* port, Car* car, Date startTime) {
  if(!port||!car) {
    displayError(ERR_LOADING_DATA, "[assignCar2Port] Invalid port or car pointer");
    return FALSE;
  }

  // Check if port is available
  if (!isPortAvailable(port)) {
    return FALSE;
  }

  //cheack portType compatiblity
  if (!isCompatiblePortType(car->portType, port->portType)) {
    return FALSE;
  }
   // assignCar2Port
  port->p2Car = car;
  port->tin = startTime;
  port->status = OCC;
  car->pPort = port;
  car->inqueue = FALSE;
    
    return TRUE;

}

BOOL isCompatiblePortType(PortType carType, PortType portType) {
  return(carType == portType);
}

BOOL isPortAvailable(Port* port){
  char msg[128];
  if(!port) {
    printf("[DEBUG] isPortAvailable called with NULL port pointer\n");
    return FALSE;
  }

  if(port->status == OCC && port->p2Car != NULL) {
    return FALSE;
  }
  if (port->status == OOD) {
    return FALSE;
  }
  
  return TRUE;
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

void destroyPort(void *data){
  Port* port = (Port*)data;
  if(port){
    free(port);
  }
}

int countFreePorts(const Port* head) {
  int count = 0;
  const Port* current = head;
  while (current)
  {
    if(current->status == FREE) count++;
    current=current->next;
  }
  return count;
}

BOOL isPortTypeValid(const char* pTypeKey) {
  return (
    Util_parsePortType(pTypeKey) != -1
  );
}
