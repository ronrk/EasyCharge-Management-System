#ifndef MENU_H
#define MENU_H

#include "SystemData.h"
#include "Port.h"

typedef struct menu
{
  SystemData *sys;
} MenuSystem;

// init main menu
MenuSystem initMenu(SystemData *sys);

// main menu
void mainMenu(SystemData *menu);
// clean menu
void destroyMenu(MenuSystem *menu);

// 1: Locate nearest station
void locateNearSt(const BinaryTree *stationTree);

// 2: Charge Car
void chargeCar(BinaryTree *stationTree, BinaryTree *carTree);
Station* getStationFromUser(const BinaryTree *stationTree);
PortType getPortTypeFromUser();
Port* getPortNumFromUser(Port* portList,const PortType portType);
BOOL getLicenseFromUser(char *input,size_t size);
int printCompatibleFreePorts(Port* head,PortType carType);

// 3: Check Car Status
void checkCarStatus(const BinaryTree* carTree,const BinaryTree* stationTree);


// 4: Stop Charge
void stopCharge(BinaryTree* stationTree,BinaryTree* carTree);

// TEST OPTIONS
void displaySystemStatus(const BinaryTree *stationTree, const BinaryTree *carTree);
void createStationInteractive(BinaryTree* stationTree);
#endif