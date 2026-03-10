#ifndef MENU_H
#define MENU_H

#include "SystemData.h"

struct MenuSystem
{
  SystemData *sys;
} MenuSystem;

void mainMenu(SystemData *menu);
void locateNearSt(const BinaryTree * stationTree);
void chargeCar(BinaryTree *stationTree,BinaryTree* carTree);
void checkCarStatus(const BinaryTree* stationTree,const BinaryTree* carTree);
void stopCharge(BinaryTree* stationTree,BinaryTree* carTree);
void dispAllSt(BinaryTree* stationTree);
void dispCarsAtSt(BinaryTree* stationTree);
void reportStStat(const BinaryTree* stationTree);
void dispTopCustomers(const BinaryTree *carTree);
void addNewPort(BinaryTree* stationTree);
void releasePorts(BinaryTree* stationTree);
void removeOutOrderPort(BinaryTree* stationTree);
void remCustomer(BinaryTree* carTree);
void closeSt(BinaryTree* stationTree);

#endif