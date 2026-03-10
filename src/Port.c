#include "Port.h"
#include "ErrorHandler.h"
#include "Utilis.h"
#include "Car.h"
#include "Queue.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// STATIC

// FUNCTIONS

// create new port
Port *createPort(unsigned int num, PortType type,PortStatus status,Date t) {
  Port* port = (Port*)malloc(sizeof(Port));
  if(!port) {
    displayError(ERR_MEMORY,"Failed alocate memory on Port");
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

// insert port to the list
Port *insertPort(Port* head, Port*newPort) {
  if(!newPort) return head;

  newPort->next = head;
  return newPort;
}

// find port
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

// destroy single port
void destroyPort(void *data){
  Port* port = (Port*)data;
  if(port){
    free(port);
  }
}

// destroy ports list
void destroyPortList(Port *head) {
  Port* current = head;
  while (current != NULL)
  {
    Port* tmp = current;
    if(current->p2Car) {
      unlinkCarPort(current->p2Car,0);
    }
    // current->p2Car->pPort = NULL;
    current = current->next;

    free(tmp);
  }
  
}

// print ports list
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

// print single port
void printPort(const Port* port) {

  printf("| Port Number: %u  ,  Type: %s  ,  Status: %s |\n",port->num,portTypeToStr(port->portType),statusToStr(port->status));
}

// port line parser from file
void* Port_parseLine(const char*line) {
  // parse port line
  unsigned int stationId,portNum;
  char typeStr[10],license[LICENSE_SIZE];
  int status,y,m,d,h,min;
  
  //1. parse line
  if (sscanf(line, "%u,%u,%9[^,],%d,%d,%d,%d,%d,%d,%8s",
               &stationId, &portNum, typeStr, &status, &y, &m, &d, &h, &min, license) != 10) {

    char msg[128];
    snprintf(msg, sizeof(msg), "[portParser] Failed to parse line: %s", line);
    displayError(ERR_PARSING, msg);
    return NULL;
  }
  // 2.convert string to portType
  PortType parsedType = portTypeFromStr(typeStr);
  if (parsedType == -1) {
    char msg[128];
    snprintf(msg, sizeof(msg), "[portParser] Invalid port type in line: %s", line);
    displayError(ERR_PARSING, msg);
    return NULL;
  }

  if(status <OCC|| status > OOD){
    char msg[128];
    snprintf(msg, sizeof(msg), "[portParser] invalid status value in line: %s", line);
    displayError(ERR_PARSING, msg);
    return NULL;
  }

  // 3.create port
  Port* port = createPort(portNum,parsedType,(PortStatus)status,(Date){y,m,d,h,min});
  if (!port) {
    char msg[128];
    snprintf(msg, sizeof(msg), "[portParser] Failed to create port line: %s", line);
    displayError(ERR_PARSING, msg);
    return NULL;
  }

  // 4.allocate temp
  PortTempData* temp = malloc(sizeof(PortTempData));
  if (!temp) {
    displayError(ERR_MEMORY,"[portParser] Failed to allocate memory");
    destroyPort(port);
    return NULL;
  }
    

  temp->port = port;
  temp->stationId = stationId;
  strncpy(temp->license, license, LICENSE_SIZE);
  return temp;
};

// count free ports
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

// check if port type is compatible
BOOL isCompatiblePortType(PortType carType, PortType portType) {
  return(carType == portType);
}

// check if port is available 
BOOL isPortAvailable(Port* port){
  char msg[128];
  if(!port) {
    printf("isPortAvailable called with NULL port pointer\n");
    return FALSE;
  }

  if(port->status == OCC || port->status == OOD) {
    return FALSE;
  }
  
  return TRUE;
}

// assisgn car to port
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

// unlink car and port
void unlinkCarPort(Car* car, double bill){
  if(!car||!car->pPort) return;
  Port *port = car->pPort;
  port->status = FREE;
  port->p2Car = NULL;
  car->pPort = NULL;
  car->totalPayed += bill;
}

// try to assign next car in queue cars to port
void tryAssignNextCarFromQueue(Station *station, Port *port, Date now) {
  if( !station|| !port|| !station->qCar) return;
  if(isEmpty(station->qCar)) {
    printf("No cars waiting in station");
    return;
  }

  PortType pType = port->portType;
  Car* nextCar = dequeueByPortType(station->qCar,pType);

  if(nextCar){
    printf("Next compatible car in queue: %s\n", nextCar->nLicense);
    if(assignCar2Port(port,nextCar,now)){
      printf("Assigned car %s to port %u at station %s.\n", nextCar->nLicense, port->num, station->name);
    } else {
      enqueue(station->qCar,nextCar);
      nextCar->inqueue = TRUE;
    }
  } else {
    printf("No compatible car in queue with type %s\n",portTypeToStr(port->portType));
  }

}

// get next number available to assign for new port in station
int getNextPortNum(Station* station){
  if(!station) return 0;
  int nextPortNum = 0;
  Port *current = station->portsList;

  // empty station
  if(station->nPorts == 0 || !station->portsList) return 1;

  // get next valid port num
  while (current)
  {
    if(current->num >= nextPortNum)
      nextPortNum = current->num + 1;

    current = current->next;
  }
  
  return nextPortNum;
}

// remove port from station
void removePortFromStation(Station *station, unsigned int portNum)
{
    if (!station || !station->portsList) return;

    Port *current = station->portsList;
    Port *prev = NULL;

    
    while (current) {      
      if (current->num == portNum) {    
        // found port to remove
        if (prev) {
          prev->next = current->next;
        } else {
          // removing head
          station->portsList = current->next;
        }
        station->nPorts--;
        destroyPort(current); // free memory
        return;
        }
        prev = current;
        current = current->next;
    }

    // if not found, you might print a warning
    // printf("Port #%u not found in station %s\n", portNum, station->name);
}
