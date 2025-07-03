#ifndef MENU_H
#define MENU_H

#include "SystemData.h"

typedef struct menu
{
  SystemData *sys;
} MenuSystem;

// init main menu
MenuSystem initMenu(SystemData *sys);

// main menu
void mainMenu(MenuSystem *menu);
// clean menu
void destroyMenu(MenuSystem *menu);

// 1: Locate nearest station
void locateNearSt(const BinaryTree *stationTree);

#endif