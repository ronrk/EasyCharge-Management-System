#include "Menu.h"
#include "Station.h"
#include "Utilis.h"
#include "Cars.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <time.h>

Station* getStationFromUser(const BinaryTree *stationTree);
BOOL getLicenseFromUser(char *input,size_t size);
PortType getPortTypeFromUser();

void mainMenu(SystemData* sys) {
  int choice;
  char input[10];
  do
  {
    printf(
      "\n***** EV CHARGING MANAGER *****\n"
      "*1. Locate Nearest Station\n"
      "*2. Charge Car\n"
      "*3. Check Car Status\n"
      "*4. Stop Charge\n"
      "*0. Exit\n"
      "********************************\n");

    printf("Enter your choice: ");
    if(!getLineFromUser(input,sizeof(input))) {
      printf("Error reading input\n");
      continue;
    } 

    if(!isNumericString(input)) {
      printf("Invalid input. Enter a number.\n");
      continue;
    }

    choice = atoi(input);

      switch (choice)
      {
      case 1:
        locateNearSt(&sys->stationTree);
        break;
      case 2:
        chargeCar(&sys->stationTree,&sys->carTree);
        break;
      case 3:
        checkCarStatus(&sys->carTree, &sys->stationTree);
        break;
      case 4:
        stopCharge(&sys->stationTree,&sys->carTree);
        break;
      case 0:
        printf("Exiting system...\n");
        break;
      default:
      
      printf("Invalid choice try again\n");
      }
      
    }
  while (choice!=0); 
  
}

// 1.locate nearst station

void locateNearSt(const BinaryTree * stationTree) {

    if(!stationTree||!stationTree->root) {
      displayError(UI_WARNING,"No Stations availabe");
      return;
    }

    // get user location
    Coord userLoc;
    

    if(!getCoordFromUser(&userLoc,"Enter your X : ","Enter your Y : ")) {
      displayError(UI_WARNING,"Invalid coordinates");
      return;
    }

    // search
    SearchKey key = {.type = SEARCH_BY_DISTANCE,.location={.userX = userLoc.x,.userY=userLoc.y}};
    Station* nearest = searchStation((BinaryTree*)stationTree,&key);

    // print result
    if(nearest) {
      double dist = calculateDistance(userLoc,nearest->coord);
      printf("\n\t*** Nearest Station ***\t\n");
      printf("Name: %s\n",nearest->name);
      printf("Id: %u\n",nearest->id);
      printf("Distance: %.2f\n",dist);
      printf("Ports Available: %d/%d\n",
        countFreePorts(nearest->portsList),nearest->nPorts);

        // printQueue
        if(nearest->qCar && !isEmpty(nearest->qCar)) {
          printf("Cars in queue: %d\n", countQueueItems(nearest->qCar));
          if(!getFront(nearest->qCar)){
            return;
          }
          printf("First in queue: %s\n", getFront(nearest->qCar)->nLicense);
        } else {
           printf("No cars in queue\n");
        }
        
    } else {
      displayError(UI_WARNING,"No station found");
      return;
    }
  }

// 2. charge car

void chargeCar(BinaryTree *stationTree,BinaryTree* carTree){
  char license[100];
  Car* car = NULL;
  Port* port = NULL;
  Station* station = NULL;
  PortType pType;
  int availablePorts;

  // get license
  if(!getLicenseFromUser(license,sizeof(license))) {
    return; // Canceled
  }

  car = searchCar(carTree,license);

  if(car) {
    port = getCarPort(car);
    if(port !=NULL) {
      printf("Unavailable car %s is connected on port %d\n", car->nLicense, port->num);
      return;
    }
    if(car->inqueue){
      printf("Unavailable car %s is already in queue\n", car->nLicense);
      return;
    }
  }

  // get station
  station = getStationFromUser(stationTree);
  if (!station) {
    displayError(UI_WARNING,"Station selection canceled or failed");
    return;
  }

  if(car) { //found car
    printf("Car found in system:\n");
    printCar(car);
    availablePorts = printCompatibleFreePorts(station->portsList,car->portType);
    if(availablePorts == 0) {
      printf("Sorry no available port for %s type\n",portTypeToStr(car->portType));
      return;
    }
    port = getPortNumFromUser(station->portsList,car->portType);
    if(!port){
      printf("Invalid or canceled port selection\n");
      return;
    }
  } else { //create new car
    // get port type
    pType = getPortTypeFromUser();
    if (pType == INVALID_PORT || pType == -1) {
      printf("Invalid or canceled port type selection.\n");
      return;
    }

    availablePorts = printCompatibleFreePorts(station->portsList,pType);

    if(availablePorts == 0) {
      printf("Sorry no available port for %s type\n",portTypeToStr(pType));
      return;
    }
    port = getPortNumFromUser(station->portsList,pType);
    if(!port) {
      printf("Invalid or canceled port selection.\n");
      return;
    }


    car = createCar(license,pType);
    if(!car) {
      printf("Failed to creat car\n");
      return;
    }

    // inser to tree
    if(!insertBST(carTree,car)){
      displayError(ERR_MEMORY, "Failed to insert car into system");
      destroyCar(car);
      return;
    }
  }
 
  // link car port
  if(isPortAvailable(port)){
    // port free
    Date now = getCurrentDate();
    if(assignCar2Port(port,car,now)) {
      printf("Car %s assigned to port %u at station %s\n", car->nLicense, port->num, station->name);
      printPort(port);

      } else {
        displayError(UI_WARNING, "Failed to assign car to port");
      }

    } else {
      // port occupied
      if(station->qCar) {
        enqueue(station->qCar,car);
        car->inqueue = TRUE;
        printf("Port is occupied. Car %s added to queue at station %s\n", car->nLicense, station->name);
      } else {
        displayError(UI_WARNING, "Station queue does not exist");
      }

    }
}

Station* getStationFromUser(const BinaryTree *stationTree) {
  char input[100];
  SearchKey key;
  SearchType type;
  while (1)
  {
    if (!getInputAndCancel(input,sizeof(input),"Enter Station ID or Name (or '0' to cancel): ")) {
      return NULL;
      }

      if (strlen(input) == 0) {
      displayError(UI_WARNING,"Empty input is invalid. Try again");
      continue; // loop to ask again
      }

      // check valid station
    if(!parseStationInput(input,&key,&type)) {
      printf("Invalid input\n");
      continue;
    };

    key.type = type;
    Station* station = searchStation(stationTree, &key);

    if(station) {
      printf("Found station: %s (ID: %u)\n", station->name, station->id);
      return station;
    } else {
      printf("No station found with input '%s'. Try again.\n", input);
    }
  }
  
}

PortType getPortTypeFromUser() {
  char input[20];
  while (1)
  {
    if(!getInputAndCancel(input,sizeof(input),"Enter Port Type for new car (| FAST | MID | SLOW |): ")) {
      return -1;
    }


    PortType type = Util_parsePortType(input);
    if(type != INVALID_PORT) return type;

    printf("Invalid input. (| FAST | MID | SLOW |)\n");
  } // end while
  
};

Port* getPortNumFromUser(Port* portList,const PortType portType){
  char input[16];
  unsigned int portNum;
  Port* port;
  while (1)
  {    
    if(!getInputAndCancel(input,sizeof(input),"Enter port num : ")){
    return NULL;
  }

    if (!isNumericString(input)) {
    printf("Invalid port number. Try again.\n");
    continue;
  }

  portNum = (unsigned int)atoi(input);
  port = findPort(portList,portNum);
  if(!port) {
    printf("Port number %u not found. Try again\n",portNum);
    continue;
  }
  printf("[DEBUG] Found port number %u\n", portNum);
  if(!isCompatiblePortType(portType,port->portType)){
    printf("Invalid port number. Try again.\n");
    continue;
  }
  return port;
  }
 
}

BOOL getLicenseFromUser(char *input,size_t size) {
  while (1)
  {
    /* code */
    if(!getInputAndCancel(input,size,"Enter car license number (8 digits): ")) {
      displayError(UI_WARNING,"Canceled by user");
      return FALSE;
    }

    if(!isLicenseValid(input)) {
      displayError(UI_WARNING,"Invalid license format. Try again");
      continue;
    }
    return TRUE;
  }

}

int printCompatibleFreePorts(Port* head,PortType carType){
  printf("\n %s ports in station: \n",portTypeToStr(carType));
  int found = 0;
  while(head) {
    if(isCompatiblePortType(carType,head->portType)) {
      if(isPortAvailable(head)){
        printf("- Port %d\n",head->num);
      } else {
        printf("- Port %d (occupied)\n",head->num);
      }
      
      found++;
    }
    head = head->next;
  }
  if(found == 0) {
    printf("No compatible ports for type %s\n",portTypeToStr(carType));
  }
  return found;
}

//3. Check car status


void checkCarStatus(const BinaryTree* carTree,const BinaryTree* stationTree) {
  Station* station = NULL;
  Port* port = NULL;

  printf("\n=== Check Car Status ===\n");

  // get license
  char license[128];
  if(!getLicenseFromUser(license,sizeof(license))) {
    return;
  }

  // search car
  Car* car = searchCar(carTree,license);
  
  if(car) {
    printf("Car found in system:\n");
    printCar(car);

    if(car->pPort) {
      port = car->pPort;
      station = findStationByPort(stationTree,port);

      Date now = getCurrentDate();
      int min = diffInMin(port->tin, now);

      printf("Car is charging at station %s (%d), port #%d\n",station->name,station->id,port->num);
      printf("Charging time: %d minutes\n",min);

    } else if (car->inqueue){
      station = findStationOfQueueCar(stationTree->root,car);
      if(station) {
        printf("car is currently waitng at station %s (%d) for type %s\n",station->name,station->id,portTypeToStr(car->portType));
      } else {
        printf("[ERROR]car is queued but not found at station\n");
      }

    } else {
      printf("car is not charging and not in queue.\n");
    }
    return;
  }

}



// 4.

Car * findCarToStopCharge(BinaryTree* carTree, char* license){
  if(!getLicenseFromUser(license,sizeof(license))) {
    printf("Canceled by user.\n");
    return NULL;
  }

  Car* car = searchCar(carTree,license);
  if(!car) {
    printf("Car not found in the system.\n");
    return NULL;
  }
  if(!car->pPort) {
    printf("This car is not currently charging.\n");
    return NULL;
  }

  return car;
}

Car *dequeueNextCarForPort(qCar* queue, PortType portType){
  if(!queue||isEmpty(queue)) {
    return NULL;
  }

  CarNode *current = queue->front;
  CarNode *prev = NULL;

  while (current)
  {
    if(current->data && current->data->portType == portType) {
      // remove node from queue

      if(prev==NULL) {
        queue->front = current->next;
        if(queue->front == NULL) {
          // queue is empty
          queue->rear = NULL;
        } 
      } else {
          prev->next = current->next;
          if(current == queue->rear) {
            queue->rear = prev;
          }
        }
        Car *car = current->data;
        free(current);
        return car;
    }
    prev = current;
    current = current->next;
  }
  return NULL;
  
}



void handleStopCharge(Car *car,BinaryTree* stationTree) {
  // Date now = getCurrentDate();
  // int minutesCharged = diffInMin(car->pPort->tin,now);

  // if(minutesCharged<1) minutesCharged = 1;

  // car->totalPayed += (double) minutesCharged * RATE_CHARGE;

  // printf("Charging stopped for car %s after %d minute(s).\n", car->nLicense, minutesCharged);
  // printf("Total payment updated: %.2f\n", car->totalPayed);

  // Station* station = findStationContainingPort(stationTree, car->pPort);
  // unsigned int stationId = station ? station->id : 0;

  // unsigned int portNum = car->pPort->num;
  // PortType pType = car->pPort->portType;


  // car->pPort->status = FREE;
  // car->pPort->p2Car = NULL;
  // car->pPort = NULL;

  // SearchKey key = {.id = stationId};
  // Station* targetStation = searchStation(stationTree, &key, SEARCH_BY_ID);
  // if(!targetStation) {
  //   printf("Warning: Station with ID %u not found.\n", stationId);
  //   return;
  // }


  // Port* port = findPort(targetStation->portsList,portNum);
  // if(!port) {
  //   printf("Error: Port #%u not found in station.\n", portNum);
  //   return;
  // }

  // // dequeue
  // if(targetStation->qCar && !isEmpty(targetStation->qCar)) {
  //   Car* nextCar = dequeueByPortType(targetStation->qCar,pType);
  //   if(nextCar) {
  //     printf("Next compatible car found in queue: %s\n", nextCar->nLicense);
  //     if(assignCar2Port(port,nextCar,now)){
  //       printf("Assigned car %s to port %u at station %s.\n", nextCar->nLicense, port->num, targetStation->name);
  //     } else {
  //       printf("Failed to assign next car to port.\n");
  //       enqueue(targetStation->qCar, nextCar); // enqueuee back
  //       nextCar->inqueue = TRUE;
  //     }
  //   } else {
  //     printf("No compatible car in queue for port type %s.\n", portTypeToStr(pType));
  //   }
  // }

}

void stopCharge(BinaryTree* stationTree,BinaryTree* carTree) {
  if(!stationTree || !carTree) {
    printf("No Data\n");
    return;
  }
  printf("\n=== Stop Charging ===\n");

  char license[LICENSE_SIZE];
  Car* car = findCarToStopCharge(carTree,license);

  handleStopCharge(car, stationTree);
}

// FOR TESTING

void displaySystemStatus(const BinaryTree *stationTree, const BinaryTree *carTree) {
    printf("\n========== SYSTEM STATUS ==========\n");

    printf("\n-- All Stations --\n");
    if (!stationTree || !stationTree->root) {
        printf("No stations found.\n");
    } else {
        inorderBST((BinaryTree *)stationTree, printFullStation);
    }

    printf("\n-- All Registered Cars --\n");
    if (!carTree || !carTree->root) {
        printf("No cars found.\n");
    } else {
        inorderBST((BinaryTree *)carTree, printCar); // you already have printCar
    }

    printf("\n===================================\n");
}

void createStationInteractive(BinaryTree* stationTree) {
  char name[MAX_NAME];
  SearchKey key;
  SearchType type;

  // ask for station details
  printf("---- Create New Station ----\n");
  while (TRUE)
  {
     // prompt for name
    if(!getInputAndCancel(name,sizeof(name),"Enter station name (or 0 to cancel): ")) 
      return;
    
      if(strlen(name) < 2) {
        printf("Name too short. Try again.\n");
        continue;
      }

      // check if name exist
      strncpy(key.name,name,sizeof(key.name) -1);
      key.name[sizeof(key.name) - 1] = '\0';
      key.type = SEARCH_BY_NAME;

      if(searchStation(stationTree,&key)) {
        printf("Station with this name already exist\n");
        continue;
      }

      break; // valid
  }

  // Generate a unique id
  unsigned int id = generateUniqueStationId(stationTree);
  printf("Generated station ID: %u\n", id);

  // create station
  Coord coord;
  if(!getCoordFromUser(&coord, "Enter X coord: ", "Enter Y coord: ")) {
    printf("Canceld");
    return;
  };

  Station *newStation = StationCreate(id,name,0,coord);
  if(!newStation) {
    printf("Failed to create station\n");
    return;
  }

  if(!insertBST(stationTree,newStation)) {
    printf("Failed to insert station into system\n");
    stationTree->destroy(newStation);
    return;
  }
  printf("Station '%s' created successfully with ID %u!\n", name, id);
  
}