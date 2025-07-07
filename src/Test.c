#include "Test.h"




// Test function
void testQueueOperations() {
    // Create queue
    qCar *queue = createQueue();
    if (!queue) {
        printf("Failed to create queue\n");
        return;
    }

    // Create some test cars
    Car car1 = {.nLicense = "12345678", .portType = FAST};
    Car car2 = {.nLicense = "87654321", .portType = SLOW};
    Car car3 = {.nLicense = "23145768", .portType = FAST};
    Car car4 = {.nLicense = "23513684", .portType = SLOW};

    // Enqueue cars
    enqueue(queue, &car1);
    enqueue(queue, &car2);
    enqueue(queue, &car3);
    enqueue(queue, &car4);

    printf("Queue after enqueueing 4 cars:\n");
    printQueue(queue);

    // Dequeue one car normally (FIFO)
    Car *dCar = dequeue(queue);
    if (dCar) {
        printf("Dequeued car (FIFO): %s\n", dCar->nLicense);
    }

    printf("Queue after dequeue FIFO:\n");
    printQueue(queue);

    // Dequeue by port type FAST
    Car *dCarFast = dequeueByPortType(queue, FAST);
    if (dCarFast) {
        printf("Dequeued car by port type FAST: %s\n", dCarFast->nLicense);
    } else {
        printf("No car with port type FAST found to dequeue.\n");
    }

    printf("Queue after dequeue by port type FAST:\n");
    printQueue(queue);

    // Cleanup
    destroyQueue(queue);
}

// Helper function to print queue contents


void assignCarsToAvailablePorts(SystemData *sys, Date now) {
    if (!sys) return;

    TreeNode *stack[100];
    int top = -1;
    TreeNode *current = sys->stationTree.root;

    while (current || top >= 0) {
        while (current) {
            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];
        Station *station = (Station *)current->data;
        printf("🚏 Checking station: %s (ID: %u)\n", station->name, station->id);

        if (station->portsList == NULL) {
          printf("⚠️  No ports found for station %u\n", station->id);
        }

        Port *p = station->portsList;
        while (p) {
          printf("🔌 Port %u (%s) is %s\n",
                p->num, portTypeToStr(p->portType),
                isPortAvailable(p) ? "AVAILABLE" : "OCCUPIED");
            if (isPortAvailable(p)) {
                Car *car = dequeueByPortType(station->qCar, p->portType);
                if (car) {
                  printCarQueue(station->qCar, station->name);
                    if (assignCar2Port(p, car, now)) {
                        printf("✅ Assigned car %s to port %u (%s) at station %u\n",
                            car->nLicense, p->num, portTypeToStr(p->portType), station->id);
                    } else {
                        printf("❌ Failed to assign car %s to port %u\n", car->nLicense, p->num);
                    }
                } else {
                    printf("⚠️ No car in queue matching port type %s\n", portTypeToStr(p->portType));
                }
            }
            p = p->next;
        }

        current = current->right;
    }
}

void printStationPorts(const SystemData *sys) {
    printf("=== Station Ports ===\n");
    TreeNode *stack[100];
    int top = -1;
    TreeNode *current = sys->stationTree.root;

    while (current || top >= 0) {
        while (current) {
            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];
        Station *station = (Station *)current->data;
        printf("Station %u - %s:\n", station->id, station->name);

        Port *port = station->portsList;
        if (!port) {
            printf("  [No ports linked]\n");
        } else {
            while (port) {
                printf("  - Port %u | Type: %s | Status: %s\n",
                       port->num,
                       portTypeToStr(port->portType),
                       port->status == FREE ? "FREE" : "OCCUPIED");
                port = port->next;
            }
        }

        current = current->right;
    }
    printf("=====================\n");
}

void printCarQueue(qCar* queue, const char* stationName) {
    if (!queue || !queue->front) {
        printf("[Queue] Station %s queue is EMPTY.\n", stationName);
        return;
    }
    CarNode* current = queue->front;
    printf("[Queue] Station %s cars in queue:\n", stationName);
    while (current) {
        printf("  Car %s, PortType %s\n", current->data->nLicense, portTypeToStr(current->data->portType));
        current = current->next;
    }
}




void testLoadFiles(){
  SystemData *data = loadFiles();
  if (!data) {
    printf("❌ loadFiles() failed.\n");
    return;
  }
  printf("✅ System data loaded successfully!\n");

  // 1. number of stations
  int numStations = countNodes(data->stationTree.root);
  printf("Stations loaded: %d\n", numStations);

  // 2. show number of cars
  int numCars = countNodes(data->carTree.root);
  printf("Cars loaded: %d\n", numCars);

  // 3. check if ports and queues exist
  printf("Checking ports and queues:\n");
  inorderBST(&data->stationTree, printStationSummary);

  // 4. Clean up
  saveAndCleanupSystem(data);
  printf("🧹 System cleaned up successfully\n");
}

void printStationQueues(SystemData *sys) {
    if (!sys) return;

    printf("=== Station Queues ===\n");
    inorderBST(&sys->stationTree, printSingleStationQueue);
    printf("======================\n");
}

void printSingleStationQueue(const void *data) {
    const Station *station = (const Station *)data;
    if (!station) return;

    printf("Station %u - %s queue:\n", station->id, station->name);

    qCar *queue = station->qCar;
    if (!queue || isEmpty(queue)) {
        printf("  [Empty]\n");
        return;
    }

    CarNode *current = queue->front;
    while (current) {
        Car *car = current->data;
        if (car) {
            printf("  - %s\n", car->nLicense);
        }
        current = current->next;
    }
}