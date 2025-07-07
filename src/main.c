#include "BinaryTree.h"
#include "Menu.h"
#include "ErrorHandler.h"
#include "Test.h"

#include <stdio.h>
#include <stdlib.h>



int main() {
    // testQueueOperations();
    SystemData *sys = loadFiles();
    if (!sys) {
        fprintf(stderr, "Failed to load system data\n");
        return 1;
    }
    // testLoadFiles();
    
    // printStationQueues(sys);
    // printStationPorts(sys);

    Date now = getCurrentDate();
    assignCarsToAvailablePorts(sys,now);
    
    
    mainMenu(sys); 
    saveAndCleanupSystem(sys);

    return 0;
}
