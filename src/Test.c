#include "Test.h"
#include "Port.h"
#include "Menu.h"
#include "SystemData.h"
#include "Utilis.h"
#include <assert.h>


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

void printStationPorts(const void *data) {
    if (!data) return;
    const Station *station = (const Station *)data;
    printf("--- Ports for Station %s (ID: %d) ---\n", station->name, station->id);
    printPortList(station->portsList);
}

void printQueueForStation(const Station* station) {
    if (!station) return;

    printf("Station %s (ID: %u):\n", station->name, station->id);

    if (station->qCar && !isEmpty(station->qCar)) {
        CarNode* current = station->qCar->front;
        while (current) {
            printf(" - Car %s | PortType: %s\n", current->data->nLicense, portTypeToStr(current->data->portType));
            current = current->next;
        }
    } else {
        printf(" - No cars in queue\n");
    }
    printf("------\n");
}

// Helper function to print queue contents

// TEST LOAD FILES
// test parsing station to line
void test_stationParser(){
    printf("\n=== Test: stationParser ===\n");
    // valid line
    const char *line = "101,Tel Aviv Center,4,32.0853,34.7818";
    Station *station = stationParser(line);
    if(station){
        printf("[PASS] stationParser: ID=%u, Name=%s, Ports=%d, Location=(%.2f, %.2f)\n",
            station->id, station->name, station->nPorts, station->coord.x, station->coord.y);
        StationDestroy(station);
    } else {
        printf("[FAIL] stationParser: Valid line failed to parse\n");
    }

    // invalid line
    const char* badLine = "bad,data,line";
    if (!stationParser(badLine)) {
        printf("[PASS] stationParser: Detected invalid line correctly\n");
    } else {
        printf("[FAIL] stationParser: Incorrectly accepted bad line\n");
    }
}

// testloading station from file
void test_loadStations(){
    printf("\n=== Test: loadStations ===\n");

    BinaryTree tree = initTree(compareStation,printStation,StationDestroy);

    int count = loadStations(&tree);

    if (count > 0) {
        printf("[PASS] loadStations: Loaded %d stations successfully.\n", count);
        printf("[INFO] Displaying loaded stations (inorder traversal):\n");
        inorderBST(&tree, printStation);  // prints all loaded stations
    } else {
        printf("[FAIL] loadStations: Failed to load stations. Count = %d\n", count);
    }

    destroyTree(tree.root,tree.destroy);
}

// test loading Cars.txt into binary tree
void test_loadCars(){
    printf("\n=== TEST: loadCars ===\n");

  // initialize a binary tree for cars
  BinaryTree carTree = initTree(compareCars, printCar, destroyCar);

  // load car data from file
  int count = loadCars(&carTree);

  // display result
  if (count > 0) {
    printf("[PASS] loadCars: loaded %d cars.\n", count);
  } else {
    printf("[FAIL] loadCars: Failed to load cars or empty file.\n");
  }

  // display loaded cars
  inorderBST(&carTree, printCar);

  // Step 5: Cleanup memory
  destroyTree(carTree.root, carTree.destroy);
}


// test loading ports Ports.txt and link to station
void test_loadPorts(){
    printf("\n=== TEST: loadPorts ===\n");

  // allocate system data
  SystemData *sys = malloc(sizeof(SystemData));
  if (!sys) {
    printf("Memory allocation failed.\n");
    return;
  }

  // initialize trees
  sys->carTree = initTree(compareCars, printCar, destroyCar);
  sys->stationTree = initTree(compareStation, printStation, StationDestroy);

  // load stations and cars data
  int stationsLoaded = loadStations(&sys->stationTree);
  int carsLoaded = loadCars(&sys->carTree);

  if (stationsLoaded <= 0 || carsLoaded <= 0) {
    printf("[FAIL] loadPorts: Failed to load prerequisite data. Cannot continue test\n");
    saveAndCleanupSystem(sys);
    return;
  }

  // load ports and link them
  int portsLoaded = loadPorts(sys);
  if (portsLoaded > 0) {
    printf("Successfully loaded and linked %d ports.\n", portsLoaded);
  } else {
    printf("[FAIL] loadPorts : Failed to load ports or none found.\n");
  }

  // display ports for each station
  printf("\n--- Ports per Station ---\n");
  inorderBST(&sys->stationTree, printStationPorts);

  // clean
  saveAndCleanupSystem(sys);
}

// test load lines of cars LinesOfCars.txt
void test_loadLinesOfCars(){
    printf("\n=== TEST: loadLineOfCars ===\n");
    // allocate and init system
    SystemData *sys = malloc(sizeof(SystemData));
    if (!sys) {
        printf("Memory allocation failed\n");
        return;
    }

    // init trees
    sys->stationTree = initTree(compareStation, printStation, StationDestroy);
    sys->carTree = initTree(compareCars, printCar, destroyCar);

    // load data stations cars ports
    int stationsLoaded = loadStations(&sys->stationTree);
    int carsLoaded = loadCars(&sys->carTree);
    int portsLoaded = loadPorts(sys);

    if (stationsLoaded <= 0 || carsLoaded <= 0 || portsLoaded <= 0) {
        printf("[FAIL] loadLineOfCars: Failed to load stations/cars/ports\n");
        saveAndCleanupSystem(sys);
        return;
    }

    int queuedCars = loadLineOfCars(sys);
    if (queuedCars > 0) {
        printf("[PASS] loadLineOfCars: Loaded %d queued cars successfully\n", queuedCars);
    } else {
        printf("[FAIL] loadLineOfCars: Failed to load queued cars or none found\n");
    }

    // print car queues for each station
    printf("\n--- Car Queues per Station ---\n");
    inorderBST(&sys->stationTree, (PrintFunc)printQueueForStation);
}

// FEATURES
// 1. locate nearest station
void test_locateNearSt_validateTree(const BinaryTree *stationTree) {
    printf("Test: Validate Station Tree\n");
    if (!stationTree || !stationTree->root) {
        printf("[PASS] Correctly handled empty or null tree\n");
    } else {
        printf("[PASS] Station tree is valid and non-empty\n");
    }
}

void test_locateNearSt_getCoordHelper(Coord coord) {

    if (coord.x != 0 && coord.y != 0) {
        printf("[PASS] Coordinates received: X=%.2f, Y=%.2f\n", coord.x, coord.y);
    } else {
        printf("[FAIL] Invalid coordinates\n");
    }
}

void test_locateNearSt_getCoord() {
    Coord userLoc;

    // valid input
    printf("Testing valid coordinate input simulation...\n");
    test_locateNearSt_getCoordHelper((Coord){32.1,34.8});
    printf("Testing empty input (simulated as zero coords)...\n");
    test_locateNearSt_getCoordHelper((Coord){0, 0});
    printf("Testing input with spaces only (simulated as zero coords)...\n");
    test_locateNearSt_getCoordHelper((Coord){0, 0});

    printf("Testing non-numeric strings (simulated as invalid -1 coords)...\n");
    test_locateNearSt_getCoordHelper((Coord){-1, -1});

    printf("Testing negative coordinates...\n");
    test_locateNearSt_getCoordHelper((Coord){-10, -20});

    printf("Testing boundary coordinates (zero X)...\n");
    test_locateNearSt_getCoordHelper((Coord){0, 34.8});

    printf("Testing boundary coordinates (zero Y)...\n");
    test_locateNearSt_getCoordHelper((Coord){32.1, 0});

    printf("Testing very large coordinates...\n");
    test_locateNearSt_getCoordHelper((Coord){1e9, 1e9});
}

void test_locateNearSt_searchStation(BinaryTree *stationTree, Coord userLoc) {
    printf("Testing searchStation with coordinates X=%.2f, Y=%.2f\n", userLoc.x, userLoc.y);
    SearchKey key = {.type = SEARCH_BY_DISTANCE, .location = {.userX = userLoc.x, .userY = userLoc.y}};
    Station* nearest = searchStation(stationTree, &key);

    if (nearest) {
        printf("[PASS] Found nearest station: %s (ID: %u)\n", nearest->name, nearest->id);
    } else {
        printf("[FAIL] No station found\n");
    }
}

void test_locateNearSt_printStationDetails(Station* station) {
    if (!station) {
        printf("[FAIL] No station to print\n");
        return;
    }

    printStationSummary(station);
}

void test_locateNearSt_full(BinaryTree *stationTree) {
    test_locateNearSt_validateTree(stationTree);

    Coord testCoord = {32.09, 34.78};
    test_locateNearSt_getCoord();

    test_locateNearSt_searchStation(stationTree, testCoord);

    Station* nearest = searchStation(stationTree, &(SearchKey){.type = SEARCH_BY_DISTANCE, .location = {.userX = testCoord.x, .userY = testCoord.y}});
    test_locateNearSt_printStationDetails(nearest);
}

// 
// 2.charge car
#include <stdio.h>
#include <string.h>

// Assume SystemData sys is already initialized properly with loaded stations and cars

void test_chargeCarWithInput(SystemData *sys, const char *input) {
    FILE *memInput = fmemopen((void *)input, strlen(input), "r");
    if (!memInput) {
        perror("fmemopen failed");
        return;
    }
    FILE *origStdin = stdin;
    stdin = memInput;
    chargeCar(&sys->stationTree, &sys->carTree);
    stdin = origStdin;
    fclose(memInput);
}

void test_chargeCar_emptyInputs(SystemData *sys) {
    const char *simulatedInput = "\n";  // empty input simulates
    test_chargeCarWithInput(sys, simulatedInput);
}

void test_chargeCar_invalidPort(SystemData *sys) {
    const char *simulatedInput = "29145178\n101\n999\n";
    test_chargeCarWithInput(sys, simulatedInput);
}

void test_chargeCar_shortLicense(SystemData *sys) {
    const char *simulatedInput = "1234\n101\n999\n";
    test_chargeCarWithInput(sys, simulatedInput);
}
void test_chargeCar_noneNumericLicense(SystemData *sys) {
    const char *simulatedInput = "abc\n101\n999\n";
    test_chargeCarWithInput(sys, simulatedInput);
}

void test_chargeCar_success(SystemData *sys) {
    const char *simulatedInput = "25497284\n518\n2\n";
    test_chargeCarWithInput(sys, simulatedInput);
}

void test_chargeCar_alreadyCharge(SystemData *sys) {
    const char *simulatedInput = "66778899\n205\n1\n";
    test_chargeCarWithInput(sys, simulatedInput);
}

void test_chargeCar_alreadyQueue(SystemData *sys) {
    const char *simulatedInput = "29587102\n205\n1\n";
    test_chargeCarWithInput(sys, simulatedInput);
}
void test_chargeCar_noCompPorts(SystemData *sys) {
    const char *simulatedInput = "26054912\n412\n3\n";
    test_chargeCarWithInput(sys, simulatedInput);
}

// 
// 4. stop charge
void mockNextUserInput(const char* str) {
    FILE* file = fopen("mock_input.txt", "w");
    if (!file) {
        printf("Failed to create mock input file.\n");
        return;
    }
    fprintf(file, "%s\n", str);
    fclose(file);

    // Redirect stdin to read from that file
    freopen("mock_input.txt", "r", stdin);
}


void test_stopCharge_scenario1(SystemData *sys) {
    printf("==== TEST: stopCharge scenario 1 ====\n");
    
    // station with 1 FAST port
    Date now = getCurrentDate();
    Date time = createDate(2025, 7, 8, 7, 30);

    Station* station = StationCreate(101, "TestStation", 0,(Coord){2,3});
    Port* port = createPort(10, FAST,FREE,now);
    printf("Station ports list head: %p, inserted port: %p\n", (void*)station->portsList, (void*)port);
    station->portsList = insertPort(station->portsList, port);
    printf("Station ports list head: %p, inserted port: %p\n", station->portsList, port);

    // Car A, already charging on port
    Car* carA = createCar("18273645", FAST);
    assignCar2Port(port,carA,time);

    insertBST(&sys->carTree,carA);

    // Car B, waiting in queue (also FAST)
    Car* carB = createCar("23178922", FAST);
    enqueue(station->qCar, carB);  // add to queue
    insertBST(&sys->carTree, carB);

    //build trees
    assert(sys->stationTree.cmp != NULL);
    insertBST(&sys->stationTree,station);
    printf("[DEBUG] Inserted station %s at %p with portsList %p\n", station->name, (void*)station, (void*)station->portsList);


    //mock user input for license (we’ll simulate the input)
    mockNextUserInput("18273645");

    // run stopCharge
    stopCharge(&sys->stationTree, &sys->carTree);

    // 7. Assertions (manual for now)
    assert(carA->pPort == NULL);  // should be unlinked
    assert(port->p2Car == carB);   // should now have carB
    assert(carB->pPort == port);  // carB assigned
    assert(carA->totalPayed > 0); // was updated
    printf("==== PASSED stopCharge scenario 1 ====\n");
}

void test_stopCharge_not_charging(SystemData *sys) {
    printf("==== TEST: stopCharge - car not charging ====\n");

    // 1. Create station and car
    Station* station = StationCreate(102, "EmptyStation", 3, (Coord){1, 1});
    insertBST(&sys->stationTree, station);

    Car* car = createCar("99988877", FAST);
    // NOT assigning the car to any port (pPort == NULL)
    insertBST(&sys->carTree, car);

    // 2. Mock input: license of car
    mockNextUserInput("99988877");

    // 3. Call stopCharge
    stopCharge(&sys->stationTree, &sys->carTree);

    // 4. Assertion: car should still be unlinked
    assert(car->pPort == NULL);
    assert(car->totalPayed == 0.0);  // No charging occurred

    printf("==== PASSED stopCharge - car not charging ====\n");
}

void test_stopCharge_car_not_found(SystemData *sys) {
    printf("==== TEST: stopCharge - car not found ====\n");

    // 1. Add a dummy station just to satisfy stationTree requirement
    Station* station = StationCreate(103, "GhostStation", 1, (Coord){0, 0});
    insertBST(&sys->stationTree, station);

    // 2. carTree is empty or doesn't contain the license we input
    mockNextUserInput("00000000");  // nonexistent license

    // 3. Call stopCharge
    stopCharge(&sys->stationTree, &sys->carTree);

    // No assertion possible, just checking it exits cleanly and prints correct message
    printf("==== PASSED stopCharge - car not found ====\n");
}

void test_stopCharge_incompatible_queue(SystemData *sys) {
    printf("==== TEST: stopCharge - incompatible car in queue ====\n");

    // 1. Setup FAST port and assign Car A to it
    Date now = getCurrentDate();
    Date tin = createDate(2025, 7, 8, 7, 00);

    Station* station = StationCreate(104, "MismatchStation", 2, (Coord){4, 4});
    Port* port = createPort(11, FAST, FREE, now);
    station->portsList = insertPort(station->portsList, port);

    Car* carA = createCar("11223344", FAST);
    assignCar2Port(port, carA, tin);
    insertBST(&sys->carTree, carA);

    // 2. Incompatible car in queue (requires SLOW)
    Car* carB = createCar("55667788", SLOW);
    enqueue(station->qCar, carB);
    insertBST(&sys->carTree, carB);

    // 3. Insert station into system
    insertBST(&sys->stationTree, station);

    // 4. Simulate license input
    mockNextUserInput("11223344");

    // 5. Run stopCharge
    stopCharge(&sys->stationTree, &sys->carTree);

    // 6. Assert that no new assignment was made
    assert(port->p2Car == NULL);            // Port is empty
    assert(carB->pPort == NULL);           // Still in queue
    assert(carA->pPort == NULL);           // Unlinked
    assert(carA->totalPayed > 0);          // Payed updated

    printf("==== PASSED stopCharge - incompatible car in queue ====\n");
}


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

void testStopCharge(BinaryTree* stationTree, BinaryTree* carTree) {
  printf("\n=== TEST: Stop Charge Simulation ===\n");

  char license[LICENSE_SIZE];
  if (!getLicenseFromUser(license, sizeof(license))) {
    printf("Canceled.\n");
    return;
  }

  Car* car = searchCar(carTree, license);
  if (!car) {
    printf("Car not found.\n");
    return;
  }

  if (!car->pPort) {
    printf("Car is not charging.\n");
    return;
  }

  printf("Found car %s currently charging...\n", car->nLicense);
  printf("Simulating stop charge and checking queue assignment...\n");

  processStopCharge(car, stationTree);
}

void initTestSystem(SystemData *sys) {
    sys->carTree = initTree(compareCars, printCar, destroyCar);
    sys->stationTree = initTree(compareStation, printStation, StationDestroy);
}

void destroyBinaryTree(BinaryTree *tree) {
    if (!tree) return;
    destroyTree(tree->root, tree->destroy);
    tree->root = NULL;  // Optional: avoid dangling pointer
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

void test_stopCharg(SystemData *sharedSys){
    SystemData *sys1 = malloc(sizeof(SystemData));
    initTestSystem(sys1);
    test_stopCharge_scenario1(sys1);
    destroyBinaryTree(&sys1->stationTree);
    destroyBinaryTree(&sys1->carTree);
    free(sys1);

    SystemData *sys2 = malloc(sizeof(SystemData));
    initTestSystem(sys2);
    test_stopCharge_not_charging(sys2);
    destroyBinaryTree(&sys2->stationTree);
    destroyBinaryTree(&sys2->carTree);
    free(sys2);

    SystemData *sys3 = malloc(sizeof(SystemData));
    initTestSystem(sys3);
    test_stopCharge_car_not_found(sys3);
    destroyBinaryTree(&sys3->stationTree);
    destroyBinaryTree(&sys3->carTree);
    free(sys3);

    SystemData *sys4 = malloc(sizeof(SystemData));
    initTestSystem(sys4);
    test_stopCharge_incompatible_queue(sys4);
    destroyBinaryTree(&sys4->stationTree);
    destroyBinaryTree(&sys4->carTree);
    free(sys4);

    freopen("/dev/tty", "r", stdin);  // On Unix-like systems (macOS, Linux)
}

void test_cargeCarWithInput(SystemData* sys){
    printf("\n[TEST] chargeCar: \t|Sucees|\t\n");
    test_chargeCar_success(sys);
    printf("\n[TEST] chargeCar: \t|Short license|\t\n");
    test_chargeCar_shortLicense(sys);
    printf("\n[TEST] chargeCar: \t|Non numeric lincese|\t\n");
    test_chargeCar_noneNumericLicense(sys);
    printf("\n[TEST] chargeCar: \t|No comp ports to car|\t\n");
    test_chargeCar_noCompPorts(sys);
    printf("\n[TEST] chargeCar: \t|invalid port input|\t\n");
    test_chargeCar_invalidPort(sys);
    printf("\n[TEST] chargeCar: \t|empty inputs|\t\n");
    test_chargeCar_emptyInputs(sys);
    printf("\n[TEST] chargeCar: \t|queued|\t\n");
    test_chargeCar_alreadyQueue(sys);
    printf("\n[TEST] chargeCar: \t|already charge|\t\n");
    test_chargeCar_alreadyCharge(sys);
}

void test_loadALl(){
    printf("\n======= TEST: Load Functions =======\n");
    test_stationParser();     // test loading entire system
    test_loadStations();  // test loading stations into BST
    test_loadCars();
    test_loadPorts();
    test_loadLinesOfCars();
    printf("====================================\n");
}
