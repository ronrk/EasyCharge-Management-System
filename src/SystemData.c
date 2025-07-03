#include "../headers/SystemData.h"
#include "../headers/BinaryTree.h"
#include "../headers/Cars.h"
#include "../headers/Station.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SystemData* loadFiles(){

  // init systemData struct to handle data
  SystemData *sys = malloc(sizeof(SystemData));
  if(!sys) {
    perror("Failed to allocate memory in SystemData\n");
    return NULL;
  }

  // init car tree
  sys->carTree = initTree(compareCars,printCar,destroyCar);
  // sys->stationTree = initTree(compareStation,printStation,StationDestroy);

  // load stations
  stationsLoad(&sys->stationTree);
  carsLoad(&sys->carTree);

  // load ports and attach to stations
  loadPorts(sys);

  // load cars
  

  // load queue
  loadLineOfCars(sys);

  return sys;
}

void destroyFiles(SystemData *sys) {
  if(!sys) return;

  // free stations
  destroyTree(sys->stationTree.root,StationDestroy);

  // free cars
  destroyTree(sys->carTree.root,destroyCar);

  free(sys);
}

void stationsLoad(BinaryTree *stationTree) {
  printf("[1] Entering stationsLoad\n");
  if(!stationTree) {
    printf("Stations tree is NULL\n");
    return;
  }

  FILE* file = fopen("data/Stations.txt","r");
  if(!file) {
    perror("Failed open Stations.txt\n");
    return;
  }

  printf("Stations file opened success\n");

  if(!skipHeader(file)) {
    printf("[3] Header skipped\n");
    printf("Station.txt is empty or corrupted\n");
    fclose(file);
    return;
  }

  printf("[4] Reading stations\n");
  // read each line and parse
  char line[256];
  while (fgets(line,sizeof(line),file))
  {
    line[strcspn(line,"\r\n")]='\0';  // remove newline
    printf("[5] Read line: %s\n", line);
    // parse line and insert to tree
    Station *station = parseStationLine(line);
    printf("[6] Parsed station\n");
    if(station) {
      insertBST(stationTree,station);
    }
  }
  
  // print for test
  printf("\n----All Station in BST---\n");
  inorderTraversal(stationTree->root,printStation);

  fclose(file);
  printf("[9] Finished loading stations\n");
}

void portsLoad(BinaryTree *stationTree, BinaryTree *carTree) {
  FILE* file = fopen("data/Ports.txt","r");
  if(!file) {
    perror("Error open file Ports.txt\n");
    return;
  }

  if(!skipHeader(file)) {
    fclose(file);
    return;
  }
  char line[256];
  // parse each line
  while (fgets(line,sizeof(line),file))
  {
    line[strcspn(line,"\r\n")] = '\0';  // remove newline

    unsigned int stationId,portNum;
    char portTypeString[10];
    int statusInt,year,month,day,hour,min;
    char license[9];
    int parsed = sscanf(line,"%u,%u,%9[^,],%d,%d,%d,%d,%d,%d,%8s",
      &stationId,&portNum,portTypeString,&statusInt,&year,&month,&day,&hour,&min,license);

    if(parsed !=10) {
      fprintf(stderr,"Failed to pars Ports line: %s\n",line);
      continue;
    }

    // convert string to enum , find station, create port
    PortStatus status = (PortStatus) statusInt;
    PortType type = parsePortType(portTypeString);

    Date tin = {year,month,day,hour,min};
    Port *port = createPort(portNum,type,status,NULL,tin);
    if(!port) continue;

    Station searchKey;
    searchKey.id = stationId;
    Station *station = (Station*)searchBST(stationTree,&searchKey);

    if(!station) {
      fprintf(stderr,"Station %u not found at port %u",stationId,portNum);
      destroyPort(port);
      continue;
    }

    station->portsList = insertPort(station->portsList,port);


    Car *car =NULL;
    if(strcmp(license,"-1")!=0) {
      car = searchCar(carTree,license);

      if(!car) {
        fprintf(stderr,"Warning: Car with license '%s' not found.\n",license);
      } else {
        port->p2Car = car;
        car->pPort = port;
      }
    }
  }

  fclose(file);
  
}

void loadCars(SystemData *sys){}
void loadPorts(SystemData *sys){
  if(!sys) return;
  portsLoad(&sys->stationTree,&sys->carTree);
}
void loadLineOfCars(SystemData *sys){}