#include "SystemData.h"
#include "ErrorHandler.h"
#include "Menu.h"

#include <stdio.h>

int main() {

  SystemData *sys = loadFiles();
  
  if(!sys){
    displayError(ERR_LOADING_DATA,"Failed to load data");
    return 1;
  }
  
  mainMenu(sys);
  
  cleanupSystem(sys);
  return 0;
}