#include "Menu.h"
#include "Station.h"
#include "Utilis.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

void mainMenu(SystemData* sys) {
  int choice;
  char input[10];
  do
  {
    printf("\n***** EV CHARGING MANAGER *****");
    printf("\n*1. Locate Nearest Station\n");
    printf("\n*2. Show Station Status\n");
    printf("\n*3. Advance Simulation Time\n");
    printf("\n*4. Generate Report\n");
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
        printf("Show Station Status selected\n");
        break;
      case 3:
        printf("Advance Time selected\n");
        break;
      case 4:
        printf("Generate Report selected\n");
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

void findNearestStation(TreeNode* node, Coord userLoc,
  Station** nearest,double* minDist) {

    if(node == NULL) return;
    // recursive traversal
    findNearestStation(node->left,userLoc,nearest,minDist);

    Station* current = (Station*)node->data;
    double dist = calculateDistance(userLoc,current->coord);

    if(dist<*minDist) {
      *minDist = dist;
      *nearest = current;
    }

    findNearestStation(node->right,userLoc,nearest,minDist);
  }

  void locateNearSt(const BinaryTree * stationTree) {
    printf("DEBUG: locNearSt function called\n");

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
    Station* nearest = NULL;
    double minDist = __DBL_MAX__;
    findNearestStation(stationTree->root,userLoc,&nearest,&minDist);

    // print result
    if(nearest) {
      printf("\n\t*** Nearest Station ***\t\n");
      printf("Name: %s\n",nearest->name);
      printf("Id: %u\n",nearest->id);
      printf("Distance: %.2f\n",minDist);
      printf("Ports Available: %d/%d\n",
        countFreePorts(nearest->portsList),nearest->nPorts);
      printf("Cars in Queue %d\n",countQueueItems(nearest->qCar));
    } else {
      printf("No station found\n");
    }
  }