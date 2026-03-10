#define _CRT_SECURE_NO_WARNINGS

#include "Menu.h"
#include "ErrorHandler.h"
#include "SystemData.h"
#include "Station.h"
#include "Queue.h"
#include "Port.h"
#include "Car.h"

#include <stdlib.h>
#include <string.h>

// static functions
static void displayMenu(){
  printf(
      "\n\t***** EasyCharge MANAGER APP *****\n"
      "\n*1. Locate Nearest Station\n"
      "*2. Charge Car\n"
      "*3. Check Car Status\n"
      "*4. Stop Charge\n"
      "*5. Display all stations\n"
      "*6. Display Cars at station\n"
      "*7. Report of Stations Statiscs\n"
      "*8. Display Top Customers\n"
      "*9. Add New Port\n"
      "*10. Release Charging Ports\n"
      "*11. Remove Out-ouf-Order Ports\n"
      "*12. Remove Customer\n"
      "*13. Close Station\n"
      "\n"
      "*0. Exit\n"
      "*******************************************\n"
      "\n"
    );

    printf("Enter your choice: ");
}

static void displayChargingCars(const Station* station){
  if(!station) return;
  Port *port = station->portsList;
  Date now = getCurrentDate();
  int foundCars = 0;

  while (port)
  {
    if(port->p2Car){
      int chargeMin = diffInMin(port->tin,now);
      if (chargeMin < 0) chargeMin = 0;
      printf("Charging: Car %s | Port Type: %s | Charging for %d minutes\n",port->p2Car->nLicense,portTypeToStr(port->portType),chargeMin);
      foundCars = 1;
    }

    port = port->next;
  }
  if(foundCars == 0){
    printf("No cars currently in charging\n");
  }
}

static void displayWaitingsCars(const Station* station){
  // cars waiting
  if(!station || !station->qCar || isEmpty(station->qCar)) return;
  if(station->qCar && !isEmpty(station->qCar)){
    CarNode *current = station->qCar->front;
    while (current)
    {
      printf("Waiting car %s | Port type: %s\n",current->data->nLicense,portTypeToStr(current->data->portType));

      current = current->next;
    }
  } else {
    printf("No cars waiting in the queue\n");
  }
}


static int getUserMenuChoice(){
  int choice;
  char input[10];
  if(!getInputFromUser(input,sizeof(input))) {
      return -1;
    } 

    if(!isNumericString(input)) {
      return -1;
    }

    choice = atoi(input);
    return choice;
}

static BOOL getLicenseFromUser(char *input,size_t size) {
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

static Station* getStationFromUser(const BinaryTree *stationTree) {
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
      printf("\nFound station: %s (ID: %u)\n", station->name, station->id);
      return station;
    } else {
      printf("No station found with input '%s'. Try again.\n", input);
    }
  }
  
}

static int printCompatibleFreePorts(Port* head,PortType carType,char * stationName){
  printf("\n%s port types in %s: \n",portTypeToStr(carType),stationName);
  int found = 0;
  while(head) {
    if(isCompatiblePortType(carType,head->portType)) {
      if(isPortAvailable(head)){
        printf("\t#%d. Port \n",head->num);
      } else {
        printf("\t#%d.Port (occupied, enter to queue?)\n",head->num);
      }
      
      found++;
    }
    head = head->next;
  }
  return found;
}

static Port* getPortNumFromUser(Port* portList,const PortType portType){
  char input[16];
  unsigned int portNum;
  Port* port;
  while (1)
  {    
    if(!getInputAndCancel(input,sizeof(input),"Enter port num : ")){
    return NULL;
    }

    if (!isNumericString(input)) {
    printf("Invalid port number. Try again\n");
    continue;
    }

    portNum = (unsigned int)atoi(input);
    port = findPort(portList,portNum);

    if(!port) {
      printf("Port number %u not found. Try again\n",portNum);
      continue;
    }

    if(!isCompatiblePortType(portType,port->portType)){
      printf("Invalid port number. Try again\n");
      continue;
    }

    return port;
  }
 
}

static PortType getPortTypeFromUser(char* prompt) {
  char input[20];
  while (1)
  {
    if(!getInputAndCancel(input,sizeof(input),prompt)) {
      return -1;
    }


    PortType type = portTypeFromStr(input);
    if(type != INVALID_PORT) return type;

    // printf("Invalid input. (| FAST | MID | SLOW |)\n");
  } // end while
  
};

static void processStopCharge(Car *car,BinaryTree* stationTree) {
  // init variable, duration, date, payed
  Port* port = car->pPort;
  Date now = getCurrentDate();
  int minutesCharged = diffInMin(car->pPort->tin,now);
  if(minutesCharged<1) minutesCharged = 1;
  double bill = minutesCharged*RATE_CHARGE;

  // unlink car and Port
  unlinkCarPort(car,bill);


  printf("Charging stopped for car %s after %d minutes, bill: %.2f\n", car->nLicense, minutesCharged,bill);
  printf("Total payment updated: %.2f\n", car->totalPayed);

  // find station
  Station* station = findStationByCar(stationTree,car);


  // check queue for compatible cars if yes dequeue and assignCar2Port
  tryAssignNextCarFromQueue(station,port,now);
}

static Port* getOODPortNumFromUser(Port* portList){
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
  if(port->status !=OOD){
    printf("Invalid Port Num\n");
    continue;
  }
  return port;
  }
 
}

static void yesOrNoInput(char* input){
  char buffer[16];
  while (1)
  {
    if(!getInputAndCancel(buffer,sizeof(buffer),"Are you sure? Y/N : ")){
      displayError(UI_WARNING,"Canceld");
      return;
    }

    if(strcmp(buffer, "y") == 0 || strcmp(buffer,"Y") == 0 || strcmp(buffer,"n") == 0 || strcmp(buffer,"N") == 0) {
      *input = buffer[0];
      return;
    }

    printf("Invalid input\n");

  }
  
}


// public functions

void mainMenu(SystemData* sys) {
  int choice;
  do
  {
    displayMenu();
    choice = getUserMenuChoice();

      switch (choice)
      {
      case 1:
        locateNearSt(&sys->stationTree);
        break;
      case 2:
        chargeCar(&sys->stationTree,&sys->carTree);
        updateFiles(sys);
        break;
      case 3:
        checkCarStatus(&sys->stationTree,&sys->carTree);
        break;
      case 4:
        stopCharge(&sys->stationTree,&sys->carTree);
        updateFiles(sys);
        break;
      case 5:
        dispAllSt(&sys->stationTree);
        break;
      case 6:
        dispCarsAtSt(&sys->stationTree);
        break;
      case 7:
        reportStStat(&sys->stationTree);
        break;
      case 8:
        dispTopCustomers(&sys->carTree);
        break;
      case 9:
        addNewPort(&sys->stationTree);
        updateFiles(sys);
        break;
      case 10:
        releasePorts(&sys->stationTree);
        updateFiles(sys);
        break;
      case 11:
        removeOutOrderPort(&sys->stationTree);
        updateFiles(sys);
        break;
      case 12:
        remCustomer(&sys->carTree);
        updateFiles(sys);
        break;
      case 13:
        closeSt(&sys->stationTree);
        updateFiles(sys);
        break;
      case 0:
        updateFiles(sys);
        printf("Exiting system...\n");
        break;
      default:
        displayError(UI_WARNING,"Invalid choice, try again\n");
        break;
      }
      
    }
  while (choice!=0); 
  
}

// 1. Locate Nearest Station
void locateNearSt(const BinaryTree * stationTree) {

    if(!stationTree||!stationTree->root) {
      displayError(ERR_LOADING_DATA,"No Stations loaded\n");
      return;
    }

    printf("\n\t*** Nearest Station ***\t\n");
    // get user location
    Coord userLoc;

    printf("\n");
    
    if(!getCoordFromUser(&userLoc,"Enter your X : ","Enter your Y : ")) {
      displayError(UI_WARNING,"Cancled\n");
      return;
    }

    // search
    SearchKey key = {.type = SEARCH_BY_DISTANCE,.location={.userX = userLoc.x,.userY=userLoc.y}};
    Station* nearest = searchStation((BinaryTree*)stationTree,&key);
    // print result
    if(nearest) {
      double dist = calculateDistance(userLoc,nearest->coord);
      printf("\nName: %s\n",nearest->name);
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

// 2. Charge Car
void chargeCar(BinaryTree *stationTree,BinaryTree* carTree){
  
  printf("\n\t*** Charge Car ***\t\n");

  char license[100];
  Car* car = NULL;
  Port* port = NULL;
  Station* station = NULL;
  PortType pType;
  int availablePorts;

  printf("\n");
  // get license
  if(!getLicenseFromUser(license,sizeof(license))) {
    return; // Canceled
  }

  car = searchCar(carTree,license);

  if(car) {
    port = getCarPort(car);
    if(port !=NULL) {
      printf("Car %s is already connected on port %d\n", car->nLicense, port->num);
      return;
    }
    if(car->inqueue){
      printf("Car %s is already in queue.\n", car->nLicense);
      return;
    }
  }

  // get station
  station = getStationFromUser(stationTree);
  printf("\n");
  if (!station) {
    displayError(UI_WARNING,"Station selection canceled");
    return;
  }

  if(car) { //found car
    printCar(car);
    availablePorts = printCompatibleFreePorts(station->portsList,car->portType,station->name);
    if(availablePorts == 0) {
      printf("\nSorry no available port for %s type\n",portTypeToStr(car->portType));
      return;
    }
    port = getPortNumFromUser(station->portsList,car->portType);
    if(!port){
      displayError(UI_WARNING,"Canceled port selection");
      return;
    }
  } else { //create new car
    // get port type
    printf("Car with licesne %s was not found in the system, create new customer:\n",license);
    pType = getPortTypeFromUser("\nEnter Port Type for new car(| FAST | MID | SLOW |): ");
    if (pType == INVALID_PORT || pType == -1) {
      displayError(UI_WARNING,"Canceled port type selection");
      return;
    }

    availablePorts = printCompatibleFreePorts(station->portsList,pType,station->name);

    if(availablePorts == 0) {
      printf("Sorry no available port for %s type\n",portTypeToStr(pType));
      return;
    }

    port = getPortNumFromUser(station->portsList,pType);
    if(!port) {
      displayError(UI_WARNING,"Canceled port selection");
      return;
    }


    car = createCar(license,pType);
    if(!car) {
      printf("Failed to creat car\n");
      return;
    }

    // inser to tree
    if(!insertBST(carTree,car)){
      displayError(ERR_MEMORY, "Failed to insert car into system\n");
      CarDestroy(car);
      return;
    }
  }
 
  // link car port
  if(isPortAvailable(port)){
    // port free
    Date now = getCurrentDate();
    if(assignCar2Port(port,car,now)) {
      printf("\nCar %s assigned to port %u at station %s\n", car->nLicense, port->num, station->name);
      printf("\t");
      printPort(port);

      } else {
        displayError(UI_WARNING, "Failed to assign car to port\n");
      }

    } else {
      // port occupied
      if(station->qCar) {
        enqueue(station->qCar,car);
        car->inqueue = TRUE;
        printf("Port is occupied. Car %s added to queue at station %s\n", car->nLicense, station->name);
      } else {
        displayError(ERR_LOADING_DATA, "Station queue does not exist");
      }

    }
}

//3. Check car status
void checkCarStatus(const BinaryTree* stationTree,const BinaryTree* carTree) {
  Station* station = NULL;
  Port* port = NULL;

  // get license
  char license[128];
  if(!getLicenseFromUser(license,sizeof(license))) {
    return;
  }

  // search car
  Car* car = searchCar(carTree,license);
  printf("\n\t*** Check Car Status ***\t\n");
  if(car) {
    printCar(car);

    if(car->pPort) {
      port = car->pPort;
      station = findStationByPort(stationTree,port);

      Date now = getCurrentDate();
      int min = diffInMin(port->tin, now);

      printf("\nCar is charging at station %s (%d),at port number #%d\n",station->name,station->id,port->num);
      printf("\tCharging time: %d minutes\n",min);

    } else if (car->inqueue){
      station = findStationOfQueueCar(stationTree->root,car);
      if(station) {
        printf("\ncar is currently waitng at station %s (%d) for type %s\n",station->name,station->id,portTypeToStr(car->portType));
      } else {
        displayError(ERR_LOADING_DATA,"car is queued but not found at station\n");
      }

    } else {
      printf("\ncar is not charging and not in queue.\n");
    }
    return;
  } else {
    displayError(UI_WARNING,"Car was not found in the system");
  }

}

// 4. stop charge
void stopCharge(BinaryTree* stationTree,BinaryTree* carTree) {
  if(!stationTree || !carTree) {
    displayError(ERR_LOADING_DATA,"Error no data!");
    return;
  }
  printf("\n\t*** Stop Charging ***\t\n");

  char license[LICENSE_SIZE];
  if(!getLicenseFromUser(license,sizeof(license))){
    return;
  }

  Car* car = searchCar(carTree,license);
  if(!car) {
    displayError(UI_WARNING,"Car was not found in the system");
    return;
  }
  if(!car->pPort) {
    displayError(UI_WARNING,"This car is not currently chargin");
    return;
  }

  processStopCharge(car, stationTree);
}

// 5. Display all stations
void dispAllSt(BinaryTree* stationTree){
  if(!stationTree||!stationTree->root) {
    printCar("No Stations Loaded\n");
    return;
  }

  printf("\n\t***All Charging Stations ***\t\n");
  inorderBST(stationTree,printStationSummary);
  printf("\n");

}

// 6. Display all cars in station
void dispCarsAtSt(BinaryTree* stationTree){
  if(!stationTree) return;
  
  Station* station = getStationFromUser(stationTree);
  if(!station) return;

  printf("\n\t*** Cars at Station: %s (%d) ***\t\n\n",station->name,station->id);

  displayChargingCars(station);
  printf("\n");
  displayWaitingsCars(station);
  
  printf("\n");
}

// 7. Report Of Station Statictics
static void printStationStatics(Station* station,int totalPorts,int occupiedPorts, int activePorts,
  int outOfOrderPorts,double utilizationRate,double oodRate,int currentChargingCars,
  int queueSize,double relativeRate) {

  
  printf("\n===== Station Statics: %s (ID: %u) =====\n", station->name, station->id);
    printf("Total Ports: %d\n", totalPorts);
    printf("Occupied Ports: %d\n", occupiedPorts);
    printf("Available Ports: %d\n", activePorts);
    printf("Out-of-Order Ports: %d\n\n", outOfOrderPorts);
    printf("Port Utilization Rate: %.2f%%\n", utilizationRate);
    printf("Out-of-Order Rate: %.2f%%\n", oodRate);
    printf("\nCharging Cars: %d\n", currentChargingCars);
    printf("Cars in Queue: %d\n", queueSize);
    

    if(currentChargingCars !=0)
    {
    printf("Relative Load: %.2f → ", relativeRate);
    //load level
    if (relativeRate < 0.2)
        printf("Heavily Loaded\n");
    else if (relativeRate < 1.0)
        printf("Loaded\n");
    else if (relativeRate == 1.0)
        printf("Balanced\n");
    else
        printf("Underloaded\n");

    }
    printf("===============================================\n\n");
}

void reportStStat(const BinaryTree* stationTree){
  if(!stationTree||!stationTree->root) {
    displayError(ERR_LOADING_DATA,"Empty station tree");
    return;
  };

  Station* station = getStationFromUser(stationTree);
  if(!station) {
    return; //canceled
  }

  Port* cur = station->portsList;
  // init counters
  int totalPorts=0, occupiedPorts=0, activePorts=0,outOfOrderPorts=0;

  // go through port list and update counters
  Port* current = station->portsList;
  while (current)
  {
    totalPorts++;
    if(current->status == OCC) occupiedPorts++;
    if(current->status == FREE || current->status == OCC) activePorts++;
    if(current->status == OOD) outOfOrderPorts++;

    current = current->next;
  }
  
  int queueSize = countQueueItems(station->qCar);
  int currentChargingCars = occupiedPorts;

  // calculate rates
  double utilizationRate = 0, relativeRate = 0, oodRate=0;
  if(activePorts>0) 
    utilizationRate = ((double)occupiedPorts / activePorts) * 100;
  if(totalPorts > 0) 
    oodRate = ((double)outOfOrderPorts/totalPorts) * 100;

  if(queueSize>0) 
    relativeRate = ((double)currentChargingCars/queueSize);
  else if(currentChargingCars > 0) 
    relativeRate = 0.0;
  else 
    relativeRate = 1.0;

  printStationStatics(station,totalPorts,occupiedPorts,activePorts,
    outOfOrderPorts,utilizationRate,oodRate,
    currentChargingCars,queueSize,relativeRate);
}

// 8.DIsplay top customers
void dispTopCustomers(const BinaryTree *carTree){
  if(!carTree || !carTree->root) {
    printf("No cars in the system\n");
    return;
  }

  int capCarArr = 10; //extra buffer
  Car **carArr = (Car**)malloc(sizeof(Car*)*capCarArr);
  if(!carArr){
    printf("Memory allocation error\n");
    return;
  }

  int count = 0;
  collectCarsInArray(carTree->root,carArr,&count);

  if(count == 0) {
    printf("No cars found error?\n");
    free(carArr);
    return;
  }

  qsort(carArr,count,sizeof(Car*),compareCarsByTotalPayed);

  printf("\n\t*** Top %d Customers ***\t\n", count < 5 ? count : 5);
    for (int i = 0; i < count && i < 5; i++) {
      printf("%d. License: %s | Total Paid: %.2lf\n", i + 1, carArr[i]->nLicense, carArr[i]->totalPayed);
    }
  printf("================================================\n");

}

// 9. Add new port
void addNewPort(BinaryTree* stationTree) {
  if(!stationTree || !stationTree->root) {
    displayError(ERR_LOADING_DATA,"Failed access data");
    return;
  }

  printf("\n\t*** Add New Port ***\t\n");

  Station *station = getStationFromUser(stationTree);
  if(!station) {
    {
      displayError(UI_WARNING,"Canceled\n");
      return;
    }
  }

  PortType pType = getPortTypeFromUser("Enter Port Type for new port(| FAST | MID | SLOW |): ");
  if(pType == INVALID_PORT) {
    displayError(UI_WARNING,"Canceled\n");
    return;
  }

  int nextPortNum = getNextPortNum(station);
  if(nextPortNum == 0) {
    displayError(ERR_LOADING_DATA,"Error get new port number");
    return;
  }

  Date now = {0};
  Port* newPort = createPort(nextPortNum,pType,FREE,now);
  if(!newPort) {
    displayError(ERR_MEMORY,"Error creating new port");
    return;
  }

  addPortToStation(station,newPort,TRUE);

  printf("Port #%d (%s) addes to station %s\n",newPort->num,portTypeToStr(newPort->portType),station->name);


  Car* car = dequeueByPortType(station->qCar,newPort->portType);
  if(car) {
    if(assignCar2Port(newPort,car,now))
      printf("Assigned car {%s} from queue to port #%d at station %s\n",car->nLicense,newPort->num,station->name);
    else 
      {
        char msg[128];
        snprintf(msg,sizeof(msg),"ERROR: Found car %s with type %s but unable to assign to new port type %s",
          car->nLicense,portTypeToStr(car->portType),portTypeToStr(newPort->portType));
        displayError(ERR_PARSING,msg);
        return;
      }

  } else {
    printf("No waiting car compatible with port type %s\n",portTypeToStr(newPort->portType));
  }
}

// 10.Realease Charging Ports
void releasePorts(BinaryTree* stationTree){
  printf("\n\t*** Release Ports ***\t\n");

  Station* station = getStationFromUser(stationTree);
  if(!station ) {
    displayError(UI_WARNING,"Canceled\n");
    return;}
  if(!station->portsList || station->nPorts == 0) {
    printf("Cant find ports at station %s\n",station->name);
    return;
  }

  
  
  Date now = getCurrentDate();
  int numReleased = 0;
  int numAssigned = 0;
  Port* currentPort = station->portsList;
  if(!currentPort) {
    printf("Error finding port");
    return;};

  
  while (currentPort)
  { 
    if(currentPort->p2Car !=NULL) {
      int minsCharged = diffInMin(currentPort->tin,now);
      if(minsCharged > 600) {
        double bill =RATE_CHARGE * minsCharged;

        Car* car = currentPort->p2Car;
        unlinkCarPort(car,bill);
        printf("\nCar %s release from Port #%d, after %d mins, bill: %f\n",car->nLicense,currentPort->num,minsCharged,bill);

        numReleased++;
        Car* nextCar = dequeueByPortType(station->qCar,currentPort->portType);
        if(nextCar != NULL) {
          if(assignCar2Port(currentPort,nextCar,now)) {
            printf("New car %s assigned to port %d\n",nextCar->nLicense,currentPort->num);
            numAssigned++;
          }
        }

        tryAssignNextCarFromQueue(station,currentPort,now);
      }
    }
    currentPort = currentPort->next;
  }
  int queueSize = countQueueItems(station->qCar);
  printf("\nDone! Released %d cars Assigned %d new cars, %d cars remain on queue.\n", numReleased, numAssigned,queueSize);
}

// 11.Remove Out Of Order Ports
void removeOutOrderPort(BinaryTree* stationTree){
  if(!stationTree||!stationTree->root){
    displayError(UI_WARNING,"No stations load\n");
  }

  printf("\n\t*** Remove Out-of-Order Ports ***\t\n");
  Station* station = getStationFromUser(stationTree);
  if(!station||!station->portsList){
    printf("No available ports in the station\n");
    return;
  }

  printf("Choose port num to remove: \n");
  int found = 0;
  Port* current = station->portsList;
  while (current)
  {
    if(current->status == OOD) {
      printf("#%d\n",current->num);
      found ++;
    }
    current = current->next;
  }

  if(found ==0){
    printf("No Out of order ports at station %s\n",station->name);
    return;
  }

  Port *portToRemove = getOODPortNumFromUser(station->portsList);

  if(!portToRemove) {
    printf("Cancled\n");
    return;
  }

  printPort(portToRemove);


  char choice;
  yesOrNoInput(&choice);
  if(choice == 'y' || choice == 'Y') {
    removePortFromStation(station,portToRemove->num);
    printf("Port removed\n");
  } else {
    printf("Cancled, port not removed\n");
  }
}

// 12.Remove Customer
void remCustomer(BinaryTree* carTree){
  char license[100];
  printf("\n\t*** Remove Customer ***\t\n");

  if(!getLicenseFromUser(license,sizeof(license))){
    displayError(UI_WARNING,"Canceld\n");
    return;
  }

  Car* car =  searchCar(carTree,license);
  if(!car) {
    displayError(UI_WARNING,"Car is not exist in the system\n");
    return;
  }

  if(car->inqueue == 1 || car->pPort !=NULL) {
    if(car->pPort) {
      printf("Car license %s currently charging on port #%d\ncan't remove customer\n",car->nLicense,car->pPort->num);
    } else {
      printf("Car license %s currently waiting in queue, can't remove customer\n",car->nLicense);
    }
    return;
  }

  char choise;
  int result;
  yesOrNoInput(&choise);

  if(choise == 'y' || choise == 'Y') {
    result = deleteBST(carTree,license);
    if(result == 0) {
      displayError(UI_WARNING,"Error Removing Customer");
      return;
    }
    printf("Car licese %s removed\n",license);
  }
  if(choise == 'n'|| choise == 'N') {
    printf("Canceled, car license %s no removed\n",license);
  }
}

void closeSt(BinaryTree* stationTree){
  Station* station = getStationFromUser(stationTree);
  if(!station) {
    printf("Cancled\n");
    return;
  }

  printStation(station);
  char choice ;
  int result = 0;
  yesOrNoInput(&choice);

  if(choice == 'y'||choice == 'Y') {
    result = deleteBST(stationTree,station);
    if(result == 0) {
      char msg[128];
      snprintf(msg,sizeof(msg),"Error deleting station %s [#%d]",station->name,station->id);
      displayError(UI_WARNING,msg);
      return;
    }
    printf("station removed\n");
  } 
  if(choice == 'n'|| choice=='N') {
    printf("Cancled, station dont removed\n");
  }

  printf("\n=== Close Station ===\n");
}
