#include<stdio.h>
#include "../headers/Cars.h"
#include "../headers/Station.h"
#include "../headers/Port.h"
#include "../headers/BinaryTree.h"
#include<string.h>
#include<stdlib.h>


int main () {


  // *********TEST*********

  {
//   // initialize car tree
//   BinaryTree carTree = initTree(compareCars,printCar,destroyCar);

//   // create station
//   Coord coord = {31.7683,35.2137};
//   Station *station = StationCreate(101,"Teset Station",0,coord);
//   if(!station) {
//     perror("Failed creating Station\n");
//     return 1;
//   }

//   // create port and add them to station
//   Port *p1 = createPort(1,FAST,FREE,NULL,(Date){0});
//   Port *p2 = createPort(2,MID,FREE,NULL,(Date){0});
//   Port *p3 = createPort(3,SLOW,FREE,NULL,(Date){0});

//   station->portsList = insertPort(station->portsList,p1);
//   station->portsList = insertPort(station->portsList,p2);
//   station->portsList = insertPort(station->portsList,p3);

//   station->nPorts = 3;

//   // Create cars
//   Car *c1 = createCar("12345678",FAST);
//   Car *c2 = createCar("9765432",MID);
//   Car *c3 = createCar("45612378",SLOW);
//   // enqueue to station queue
//   enqueue(station->qCar,c1);
//   enqueue(station->qCar,c2);
//   enqueue(station->qCar,c3);

//   insertBST(&carTree,c1);
//   insertBST(&carTree,c2);
//   insertBST(&carTree,c3);

//   // prints
//   printf("\n-------Station Info-------\n");
//   printStation(station);

//   printf("\n-------Ports List-------\n");
//   printPortList(station->portsList);

//   printf("\n-------Queue of Cars-------\n");
//   printQueue(station->qCar);

//   printf("\n-------All Cars in Tree In-Order-------\n");
//   inorderTraversal(carTree.root,printCar);

//   //Charge car and Search Car
//   const char* targetLicense = "12345678";
//   Car *foundCar = searchCar(&carTree,targetLicense);
//   if(foundCar) {
//     printf("Found Car: \n");
//     printCar(foundCar);


//     // Search same availaible port
//     Port *current = station->portsList;
//     while (current!=NULL)
//     {
//       if(current->portType == foundCar->portType && current->status == FREE) {
//         current->status = OCC;
//         current->p2Car = foundCar;
//         current->tin = (Date){2025,7,3,10,30};
        
//         foundCar->pPort = current;
//         foundCar->inqueue = FALSE;

//         printf("\nCar '%s' assigned to Port #%u\n",foundCar->nLicense,current->num);
//         break;
//       }

//       current=current->next;
//     }
//     if(!foundCar->pPort) {
//       printf("No availabale ports for car '%s'",foundCar->nLicense);
//     } else {
//       printf("Car with license '%s' not found\n",targetLicense);
//     }
    

//   }


//   // prints updates
//   printf("\n-------Station Update-------\n");
//   printStation(station);

//   printf("\n-------Ports List Update-------\n");
//   printPortList(station->portsList);

//   printf("\n-------Queue of Cars Update-------\n");
//   printQueue(station->qCar);

//   printf("\n-------All Cars in Tree In-Order-------\n");
//   inorderTraversal(carTree.root,printCar);

//   StationDestroy(station);
//   destroyTree(carTree.root,destroyCar);
  
 }
  
return 0;
}