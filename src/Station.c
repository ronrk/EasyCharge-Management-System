#include "Station.h"
#include "ErrorHandler.h"
#include "BinaryTree.h"
#include "Port.h"
#include "Car.h"
#include "Queue.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// STATIC

// search helpers
typedef Station *(*SearchFunc)(const BinaryTree *tree, SearchKey *key);

// search station by name logic
static Station *searchByNameHelper(TreeNode *node, const char *name)
{
  if (node == NULL)
    return NULL;
  Station *station = (Station *)node->data;

  // search correct node
  if (strstr(station->name, name) != NULL)
    return station;

  // search lef
  Station *found = searchByNameHelper(node->left, name);
  if (found)
    return found;

  // search right
  return searchByNameHelper(node->right, name);
}

static Station *searchByName(const BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  return searchByNameHelper(tree->root, key->name);
}

// search station by id logic
static Station *searchById(const BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  Station temp = {.id = key->id};
  return (Station *)searchBST((BinaryTree *)tree, &temp);
}

// search station by distance logic
typedef struct
{
  Station *closest;
  double minDistance;
  double userX;
  double userY;
} DistanceSearchHelper;

static void searchByDistanceHelper(TreeNode *node, DistanceSearchHelper *helper)
{
  if (node == NULL)
    return;
  Station *station = (Station *)node->data;

  double dx = station->coord.x - helper->userX;
  double dy = station->coord.y - helper->userY;
  double distance = sqrt(dx * dx + dy * dy);

  if (distance < helper->minDistance)
  {
    helper->minDistance = distance;
    helper->closest = station;
  }

  searchByDistanceHelper(node->left, helper);
  searchByDistanceHelper(node->right, helper);
}

static Station *searchByDistance(const BinaryTree *tree, SearchKey *key)
{

  if (!tree)
    return NULL;

  DistanceSearchHelper helper = {
      .closest = NULL,
      .minDistance = 999999999999999999,
      .userX = key->location.userX,
      .userY = key->location.userY};

  searchByDistanceHelper(tree->root, &helper);
  return helper.closest;
}

// array to collect all search function options [BY NAME | BY ID | BY DISTANCE]
SearchFunc searchFunctions[SEARCH_TYPE_COUNT] = {
    searchById,
    searchByName,
    searchByDistance};

// FUNCTIONS
// search station entry function
Station *searchStation(const BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  if (key->type < 0 || key->type >= SEARCH_TYPE_COUNT)
    return NULL;
  return searchFunctions[key->type](tree, key);
}

// create new dynamic station
Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord)
{

  Station *s = (Station *)malloc(sizeof(Station));
  if (!s)
  {
    displayError(ERR_MEMORY, "Failed Allocation Memory Create Station");
    return NULL;
  }

  // copy name
  s->name = strdup(name);
  if (!s->name)
  {
    char msg[128];
    snprintf(msg, sizeof(msg), "Failed copy name to station for : %s\n", name);
    displayError(ERR_PARSING, msg);
    free(s);
    return NULL;
  }
  // printf("[Create5] Name duplicated: %s\n", name);

  // creat Queue
  s->qCar = createQueue();
  if (!s->qCar)
  {
    char msg[128];
    snprintf(msg, sizeof(msg), "Queue creataion failed for Station: %u\n", id);
    displayError(ERR_PARSING, msg);
    free(s->name);
    free(s);
    return NULL;
  }

  s->id = id;
  s->nPorts = nPorts;
  s->coord = coord;
  s->portsList = NULL;
  s->nCars = 0;

  return s;
}

// print station
void printStation(const void *data)
{
  const Station *station = (Station *)data;
  printf("Station ID: %u  |  Name: %s |  Ports: %d  |  Cars: %d\n", station->id, station->name, station->nPorts, station->nCars);
}

// compare station by id
int compareStationById(const void *a, const void *b)
{
  const Station *s1 = (const Station *)a;
  const Station *s2 = (const Station *)b;

  if (!s1 || !s2)
  {
    return 0;
  }

  int result = (s1->id > s2->id) - (s1->id < s2->id);
  return result;
}

// destroy station
void StationDestroy(void *data)
{
  Station *station = (Station *)data;
  if (!station)
    return;
  // free dynamic name
  free(station->name);
  // free dynamic qCar
  if (station->qCar)
    destroyQueue(station->qCar);

  // free dynamic ports linked list
  if (station->portsList)
    destroyPortList(station->portsList);
  free(station);
}

// station parser line from file
void *Station_parseLine(const char *line)
{

  unsigned int id;
  char name[100];
  int nPorts;
  double x, y;

  // scan file to get the line data
  int scan = sscanf(line, "%u,%99[^,],%d,%lf,%lf", &id, name, &nPorts, &x, &y);
  if (scan != 5)
  {
    char msg[128];
    snprintf(msg, sizeof(msg), "Error parse line in parseStationLine line: %s", line);
    displayError(ERR_PARSING, msg);
    return NULL;
  }

  Coord coord = {x, y};
  // create new station
  Station *station = StationCreate(id, name, nPorts, coord);

  return station;
}

// add port to station port list
void addPortToStation(Station *station, Port *port, BOOL increment)
{
  if (!station || !port)
  {
    return;
  }

  station->portsList = insertPort(station->portsList, port);
  if (increment)
    station->nPorts++;

  // count how many ports really in the linked list now
  int actualCount = 0;
  Port *cur = station->portsList;
  while (cur)
  {
    actualCount++;
    cur = cur->next;
  }
}

// enqueue a car to car queue
BOOL enqueueCarToStationQueue(Station *station, Car *car)
{
  if (!station || !car)
    return FALSE;

  if (enqueue(station->qCar, car))
  {
    car->inqueue = TRUE;
    station->nCars++;
    return TRUE;
  }

  return FALSE;
}

// find station by a car
Station *findStationByCar(BinaryTree *stationTree, Car *car)
{
  if (!stationTree || !car || !car->pPort)
    return NULL;

  return findStationByPort(stationTree, car->pPort);
}

// find station by port
Station *findStationByPort(const BinaryTree *tree, const Port *port)
{
  if (!tree || !tree->root || !port)
    return NULL;

  // in-order traversal stack
  TreeNode *stack[1000];
  int top = -1;
  TreeNode *current = tree->root;

  while (current || top >= 0)
  {

    while (current)
    {
      stack[++top] = current;
      current = current->left;
    }

    current = stack[top--];
    Station *station = (Station *)current->data;

    // check if the port exists in this station list
    Port *p = station->portsList;
    while (p)
    {

      if (p == port)
      { // compare by pointer memory address
        return station;
      }
      p = p->next;
    }

    current = current->right;
  }
  return NULL;
}

// print station summary
void printStationSummary(const void *data)
{
  const Station *station = (const Station *)data;
  if (!station)
    return;

  printf("\n\t\t| %s (%u) |\n", station->name, station->id);

  // Count ports
  int totalPorts = 0, occupied = 0, ood = 0, fast = 0, mid = 0, slow = 0;
  Port *p = station->portsList;
  while (p)
  {
    totalPorts++;
    if (p->status == OOD)
      ood++;
    if (p->status == OCC)
      occupied++;

    if (p->portType == SLOW)
      slow++;
    else if (p->portType == MID)
      mid++;
    else if (p->portType == FAST)
      fast++;

    p = p->next;
  }

  // Count queue length
  int queueCount = countQueueItems(station->qCar);
  printf("| Total Ports: %d | Out-Of-Order: %d | Occupied: %d | Queue: %d |\n", totalPorts, ood, occupied, queueCount);
  printf("| Port Types: FAST: %d | MID: %d | SLOW: %d |\n", fast, mid, slow);
}
