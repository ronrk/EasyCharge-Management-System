#ifndef PORT_H
#define PORT_H

#include "Utilis.h"
#include "Station.h"


// STRUCT & Enums declarations
typedef struct Car Car;
typedef struct Port Port;

struct Port
{
  unsigned int num;
  PortType portType;
  PortStatus status;
  Car *p2Car; // car in port
  Date tin;   // time for start charge
  struct Port *next;
};

// using while loading files
typedef struct 
{
  Port* port;
  unsigned int stationId;
  char license[LICENSE_SIZE];
} PortTempData;


// FUNCTIONS
Port *createPort(unsigned int num, PortType type,PortStatus status,Date t);
Port *insertPort(Port* head, Port*newPort);
Port *findPort(Port *head, unsigned int num);
void destroyPort(void *data);
void destroyPortList(Port *head);
void printPort(const Port* port);
void printPortList(Port *head);
void* Port_parseLine(const char*line);
int countFreePorts(const Port* head);
BOOL assignCar2Port(Port* port, Car* car, Date startTime);
void unlinkCarPort(Car* car, double bill);
BOOL isCompatiblePortType(PortType carType, PortType portType);
BOOL isPortAvailable(Port* port);
void tryAssignNextCarFromQueue(Station *station, Port *port, Date now);
int getNextPortNum(Station* station);
void removePortFromStation(Station *station, unsigned int portNum);

#endif