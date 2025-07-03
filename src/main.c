
#include "../headers/SystemData.h"
#include "../headers/Station.h"
#include "../headers/Cars.h"
#include "../headers/Port.h"
#include "../headers/BinaryTree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void testCriticalLinks(SystemData *sys);
void printStationDetails(Station *station);

int main() {
    printf("\n\t**** EV CHARGING SYSTEM START ****\t\n\n");
    
    // Load all data
    SystemData *sys = loadFiles();
    if(!sys) {
        fprintf(stderr, "CRITICAL ERROR: System initialization failed\n");
        return 1;
    }
    printf("\t**** System data loaded successfully ****\t\n\n");
    
    // Basic statistics
    printf("System Statistics:\n");
    printf("---------------------------------\n");
    printf("Stations: %d\n", countNodes(sys->stationTree.root));
    printf("Cars:     %d\n", countNodes(sys->carTree.root));
    printf("---------------------------------\n\n");
    
    // Critical functionality tests
    testCriticalLinks(sys);
    
    // Resource cleanup
    destroyFiles(sys);
    printf("\n\t**** SYSTEM SHUTDOWN COMPLETE ****\t\n");
    return 0;
}

void testCriticalLinks(SystemData *sys) {
    if(!sys || !sys->stationTree.root) {
        fprintf(stderr, "ERROR: No stations available for testing\n");
        return;
    }
    
    // Get first station
    Station *station = (Station*)sys->stationTree.root->data;
    printf("\nTESTING STATION: %s (ID: %u)\n", station->name, station->id);
    printf("=========================================\n");
    
    // Test 1: Verify port loading
    if(!station->portsList) {
        fprintf(stderr, "CRITICAL ERROR: No ports loaded for station\n");
    } else {
        printf("Loaded Ports:\n");
        printPortList(station->portsList);
    }
    
    // Test 2: Verify car-port linking
    if(station->portsList) {
        Port *port = station->portsList;
        printf("\nTesting car-port linking for port %u:\n", port->num);
        
        if(port->p2Car) {
            printf("SUCCESS: Port linked to car %s\n", port->p2Car->nLicense);
        } else if(strcmp(port->license, "-1") != 0) {
            fprintf(stderr, "ERROR: Port has license '%s' but no car link!\n", port->license);
        } else {
            printf("Port %u has no car assignment (as expected)\n", port->num);
        }
    }
    
    // Test 3: Verify queue initialization
    printf("\nTesting station queue:\n");
    if(!station->qCar) {
        fprintf(stderr, "CRITICAL ERROR: Station queue not initialized\n");
    } else {
        printf("Queue status: %s\n", isEmpty(station->qCar) ? "EMPTY" : "OPERATIONAL");
        printf("Queue contents:\n");
        printQueue(station->qCar);
    }
    printf("=========================================\n");
}