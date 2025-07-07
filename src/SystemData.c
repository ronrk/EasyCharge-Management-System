#include "../headers/SystemData.h"
#include "../headers/BinaryTree.h"
#include "../headers/Cars.h"
#include "../headers/Station.h"
#include "Menu.h"
#include "ErrorHandler.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
  Port* port;
  unsigned int stationId;
  char license[LICENSE_SIZE];
}PortTempData;

static Station* getStationById(BinaryTree* tree,unsigned int id){
  if(!tree||!tree->root) return NULL;
return findStationById(tree->root,id);
}

// Parsers
void* portParser(const char*line) {
  // parse port line
  unsigned int stationId,portNum;
  char typeStr[10],license[LICENSE_SIZE];
  int status,y,m,d,h,min;
  
  //1. parse line
  if (sscanf(line, "%u,%u,%9[^,],%d,%d,%d,%d,%d,%d,%8s",
               &stationId, &portNum, typeStr, &status, &y, &m, &d, &h, &min, license) != 10) {
      displayError(ERR_PARSING,"[portParser] Failed to parse line");
      return NULL;
    }
  // 2.convert string to portType
  PortType parsedType = Util_parsePortType(typeStr);
  if (parsedType == -1) {
    displayError(ERR_PARSING,"[portParser] Invalid port type in line");
    return NULL;
  }

  // 3.create port
  Port* port = createPort(portNum,parsedType,(PortStatus)status,(Date){y,m,d,h,min});
  if (!port) {
    displayError(ERR_PARSING,"[portParser] Failed to create port");
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
}

void* stationParser(const char* line) {
  return Station_parseLine(line);
}

void* carsParser(const char* line){
  return parseCarLine(line);
}

void* lineOfCarParser(const char* line){
  if(!line) return NULL;

  LineOfCarsEntry* entry = malloc(sizeof(LineOfCarsEntry));
  if(!entry) {
    displayError(ERR_PARSING,"[lineOfCarParser] Memory allocation failed");
    return NULL;
  }

  if(sscanf(line,"%8s,%u",entry->license,&entry->stationId) != 2) {
    displayError(ERR_PARSING, "[lineOfCarParser] Failed to parse line");
    free(entry);
    return NULL;
  }
  return entry;
}

// processors
void linkPortToStation(void*obj, void*context) {
  
  PortTempData* temp = (PortTempData*)obj;
  SystemData* sys = (SystemData*)context;
  if(!temp||!sys) {
    displayError(ERR_LOADING_DATA,"[linkPortToStation]Null Data");
    return;
  }

  Port* port = temp->port;

   printf("🔗 Processing port ID: %u, StationID: %u\n", temp->port->num, temp->stationId);

  // 1.find the station for this port
  SearchKey key = {.type = SEARCH_BY_ID,.id = temp->stationId};
  Station* station = searchStation(&sys->stationTree, &key);
  if (!station) {
    printf("❌ Station with ID %u not found!\n", temp->stationId);
    destroyPort(port);
    free(temp);
    return;
  }
  printf("✅ Found station: %s (ID: %u). Linking port...\n", station->name, station->id);
  
  // 2.add port to station
  addPortToStation(station,temp->port);
  printf("✅ Port linked to station %u\n", station->id);

  // 3.link car to port
  if (isLicenseValid(temp->license)) {
    Car* car = searchCar(&sys->carTree, temp->license);
    if (car) {
      printf("🚗 Assigning car %s to port %u\n", car->nLicense, temp->port->num);
      assignCar2Port(port,car,port->tin);
    } else {
      printf("🚗 Assigning car %s to port %u\n", car->nLicense, temp->port->num);
      displayError(ERR_LOADING_DATA, "[linkPortToStation] Car not found for assignment");
    }
  }
    free(temp);
}

void lineOfCarsProcessor(void* obj,void* context){
  if(!obj||!context) {
    displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Null data");
    return;
  }

  LineOfCarsEntry* entry = (LineOfCarsEntry* )obj;
  SystemData* sys = (SystemData*) context;

  SearchKey key = {.id = entry->stationId, .type = SEARCH_BY_ID};

  Station * station = searchStation(&sys->stationTree,&key);
  if (!station) {
        displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Station not found");
        return;
    }
  Car* car = searchCar(&sys->carTree, entry->license);
    if (!car) {
        displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Car not found");
        return;
    }
    if (!enqueueCarToStationQueue(station, car)) {
        displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Failed to enqueue car");
    }
}

// destroyes
void saveAndCleanupSystem(SystemData *sys) {
  if(!sys) return;

  // free trees
  if (sys->stationTree.root) {
    destroyTree(sys->stationTree.root, sys->stationTree.destroy);
  }
  if (sys->carTree.root) {
    destroyTree(sys->carTree.root, sys->carTree.destroy);
  }
    
  free(sys);
}

void destroyPortTemp(void* obj) {
  PortTempData* temp = (PortTempData*)obj;
  if (temp) {
    destroyPort(temp->port);
    free(temp);
  }
}

void destroyLineOfCars(void* obj){
  free(obj);
}


// Loading files

SystemData* loadFiles(){
  // init systemData struct to handle data
  SystemData *sys = malloc(sizeof(SystemData));
  if(!sys) {
    displayError(ERR_LOADING_DATA,"Failed to allocate memory in SystemData\n");
    return NULL;
  }


  // init car tree
  sys->carTree = initTree(compareCars,printCar,destroyCar);
  // init station tree
  sys->stationTree = initTree(compareStation,printStation,StationDestroy);


  // load all files and checks if fail free sys
  if(loadStations(&sys->stationTree)<=0 || loadCars(&sys->carTree) <=0 || loadPorts(sys)<=0||loadLineOfCars(sys)<=0) {
    saveAndCleanupSystem(sys);
    return NULL;
  }
  

  return sys;
}

int loadStations(BinaryTree *stationTree){
  FileLoaderConfig config = {
    .filename="data/Stations.txt",
    .targetTree =stationTree,
    .parser = stationParser,
    .processor = NULL,
    .context = NULL,
    .destroyObject=StationDestroy,
    .skipHeader = 1
  };
  return loadDataFile(&config);
}

int loadCars(BinaryTree *carTree){
  FileLoaderConfig config = {
    .filename = "data/Cars.txt",
    .targetTree =carTree,
    .parser = carsParser,
    .processor = NULL,
    .context = NULL,
    .destroyObject = destroyCar,
    .skipHeader = 1
  };
  return loadDataFile(&config);
}

int loadPorts(SystemData *sys){
  FileLoaderConfig config = {
    .filename = "data/Ports.txt",
    .targetTree = NULL,
    .parser = portParser,
    .processor = linkPortToStation,
    .context = sys,
    .destroyObject = destroyPortTemp,
    .skipHeader = 1
  };
  return loadDataFile(&config);
}

int loadLineOfCars(SystemData *sys) {
  if(!sys) return 0;
  FileLoaderConfig config = {
    .filename = "data/LineOfCars.txt",
    .targetTree = NULL,
    .parser = lineOfCarParser,
    .processor = lineOfCarsProcessor,
    .context = sys,
    .destroyObject = destroyLineOfCars,
    .skipHeader = 1
  }; 

  return loadDataFile(&config);
}

