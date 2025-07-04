#include "Menu.h"
#include "Station.h"
#include "Utilis.h"
#include "Cars.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

Station* getStationFromUser(const BinaryTree *stationTree);
BOOL getLicenseFromUser(char *input,size_t size);
PortType getPortTypeFromUser();

void mainMenu(SystemData* sys) {
  int choice;
  char input[10];
  do
  {
    printf("\n***** EV CHARGING MANAGER *****");
    printf("\n*1. Locate Nearest Station\n");
    printf("\n*2. Charge Car\n");
    printf("\n*3. Display System Status\n");
    printf("\n*4. Create New Station\n");
    printf("\n*0. Exit\n");
    printf("*********************\n");
    printf("Enter your choice: ");

    if(fgets(input,sizeof(input),stdin) == NULL) {
      printf("Error reading input\n");
      continue;
    }

    if(sscanf(input,"%d",&choice)!=1) {
      printf("Invalid input. enter a number\n");
      continue;
    }
      switch (choice)
      {
      case 1:
        locateNearSt(&sys->stationTree);
        break;
      case 2:
        chargeCar(&sys->stationTree,&sys->carTree);
        break;
      case 3:
        displaySystemStatus(&sys->stationTree, &sys->carTree);
        break;
      case 4:
        createStationInteractive(&sys->stationTree);
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


void locateNearSt(const BinaryTree * stationTree) {

    if(!stationTree||!stationTree->root) {
      printf("No Stations availabe\n");
      return;
    }

    // get user location
    Coord userLoc;
    printf("Enter your current location (x y): ");
    if(scanf("%lf %lf",&userLoc.x,&userLoc.y)!=2) {
      printf("Invalid coordinates\n");
      clearInputBuffer();
      return;
    }
    clearInputBuffer();

    // init search
    SearchKey key = {.type = SEARCH_BY_DISTANCE,.location={.userX = userLoc.x,.userY=userLoc.y}};

    // search
    Station* nearest = searchStation((BinaryTree*)stationTree,&key,SEARCH_BY_DISTANCE);
    // print result
    if(nearest) {
      double dist = calculateDistance(userLoc,nearest->coord);
      printf("\n\t*** Nearest Station ***\t\n");
      printf("Name: %s\n",nearest->name);
      printf("Id: %u\n",nearest->id);
      printf("Distance: %.2f\n",dist);
      printf("Ports Available: %d/%d\n",
        countFreePorts(nearest->portsList),nearest->nPorts);
      printf("Cars in Queue %d\n",countQueueItems(nearest->qCar));

        // printQueue
        if(nearest->qCar && !isEmpty(nearest->qCar)) {
          printf("Cars in queue: %d\n", countQueueItems(nearest->qCar));
          printf("First in queue: %s\n", getFront(nearest->qCar)->nLicense);
        } else {
           printf("No cars in queue\n");
        }
        
    } else {
      printf("No station found\n");
    }
  }

void chargeCar(BinaryTree *stationTree,BinaryTree* carTree){
  char license[9];
  Car* car = NULL;
  Port* port = NULL;
  
  // exist car
  while (1)
  {
    // get and check car license
  
    if(!getLicenseFromUser(license,sizeof(license))) {
      return; // Canceled
    }
    // search for cars in tree
    car = searchCar(carTree,license);
    if(car) {
      printf("Car found in system:\n");
      printCar(car);
    } else {
      printf("Car not found. We will register a new one.\n");
    }
      break;  // goode license
  } // end while

  // no car Get new Car, get port type
  if(!car){
    // get Port Type
    PortType pType = getPortTypeFromUser();

    if(pType == -1) {
      printf("Canceled\n");
      return;
    }

    // create car
    car = createCar(license,pType);
    if(!car) {
      printf("Failed to create new car.\n");
      return;
    }

    // insert car into tree
    if (!insertBST(carTree, car)) {
        printf("Failed to insert car into system.\n");
        destroyCar(car);
        return;
    }   
 
  printf("New car registered:\n");
  printCar(car);
 }

  //  get station
  Station* station = getStationFromUser(stationTree);
  if (!station) {
    printf("Station selection canceled or failed.\n");
    return;
  }

  // find port in station
  Port* availablePort = findAvailablePort(station->portsList,car->portType);
  if(!availablePort) {
    printf("No available ports for %s cars at %s\n", 
           portTypeToStr(car->portType), station->name);

    if(station->qCar) {
      enqueue(station->qCar,car);
      car->inqueue = TRUE;
      printf("Car added to queue station: %s\n",station->name);
    } else {
      printf("Queue dont exist\n");
    }
    

    return;
  }

  Date now = {2025,7,4,15,45};
  if(assignCar2Port(availablePort,car,now)){
     printf("Car %s assigned to port %u at station %s\n", car->nLicense, availablePort->num, station->name);
    printPort(availablePort);
    } else {
        printf("Failed to assign car to port.\n");
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

      // check valid station
    if(!parseStationInput(input,&key,&type)) {
      printf("Invalid input\n");
      continue;
    };

    Station* station = searchStation((BinaryTree*)stationTree, &key,type);

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
    printf("Enter Port Type for new car (| FAST | MID | SLOW |): ");

    if(fgets(input,sizeof(input),stdin) == NULL) {
      printf("Input error, try again \n");
      continue;
    }

    removeTrailingNewline(input);

    if(isCancelInput(input)) {
      printf("Canceled by user\n");
      return -1;
    };

    PortType type = parsePortType(input);
    if(type != -1) return type;

    printf("Invalid input. (| FAST | MID | SLOW |)\n)");
  } // end while
  
};


BOOL getLicenseFromUser(char *input,size_t size) {
  while (1)
  {
    /* code */
    printf("Enter car license number (8 digits): ");
    if(!fgets(input,size,stdin)) {
      printf("Input Error.\n");
      return FALSE;
    }
    removeTrailingNewline(input);

    if(isCancelInput(input)) {
      printf("Canceled by user\n");
      return FALSE;
    }
    if(!isLicenseValid(input)) {
      printf("Invalid license format. Try again.\n");
      continue;
    }

    return TRUE;
  }

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
      type = SEARCH_BY_NAME;

      if(searchStation(stationTree,&key,type)) {
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