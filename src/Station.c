#include "../headers/Station.h"
#include "../headers/Queue.h"
#include "../headers/Port.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord)
{
  // printf("[StationCreate] Creating station %u\n", id);
  Station *s = malloc(sizeof(Station));
  if (!s)
  {
    perror("Failed Allocation Memory Create Station\n");
    return NULL;
  }
  // printf("[Create3] Allocated station struct\n");
  // copy name
  s->name = strdup(name);
  if (!s->name)
  {
    fprintf(stderr, "Failed copy name to station for : %s\n", name);
    free(s);
    return NULL;
  }
  // printf("[Create5] Name duplicated: %s\n", name);

  // creat Queue
  s->qCar = createQueue();
  if (!s->qCar)
  {
    fprintf(stderr, "Queue creataion failed for Station: %u\n", id);
    free(s->name);
    free(s);
    return NULL;
  }
  // printf("[Create6] Creating queue\n");
  s->id = id;
  s->nPorts = nPorts;
  s->coord = coord;
  s->portsList = NULL;
  s->nCars = 0;
  s->left = NULL;
  s->right = NULL;

  // printf("[Create8] Station created successfully\n");
  return s;
}

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

int compareStation(const void *a, const void *b)
{
  // printf("[compareStation] Start\n");
  const Station *s1 = a;
  const Station *s2 = b;

  // Add null checks
  if (!s1 || !s2)
  {
    // printf("[compareStation] ERROR: Null station pointer!\n");
    return 0;
  }

  // printf("[compareStation] Comparing %u and %u\n", s1->id, s2->id);
  int result = (s1->id > s2->id) - (s1->id < s2->id);
  // printf("[compareStation] Result: %d\n", result);

  // return (s1->id > s2->id) - (s1->id < s2->id);
  return result;
}

void printStation(const void *data)
{
  const Station *station = data;
  printf("Station ID: %u  |  Name: %s |  Ports: %d  |  Cars: %d\n", station->id, station->name, station->nPorts, station->nCars);
}

Station *insertStation(Station *root, Station *newStation)
{
  if (root == NULL)
    return newStation;

  int cmp = compareStation((const void *)newStation, (const void *)root);
  if (cmp < 0)
    root->left = insertStation(root->left, newStation);
  else if (cmp > 0)
    root->right = insertStation(root->right, newStation);
  else
  {
    // duplicate id
  }
  return root;
}
void inorderStationTraversal(Station *root)
{
  if (root == NULL)
    return;
  inorderStationTraversal(root->left);
  printStation(root);
  inorderStationTraversal(root->right);
}
void destroyStationTree(Station *root)
{
  if (root == NULL)
    return;
  destroyStationTree(root->left);
  destroyStationTree(root->right);
  StationDestroy(root);
}

void *parseStationLine(const char *line)
{
  // printf("[Parse1] Parsing line: %s\n", line);
  unsigned int id;
  char name[100];
  int nPorts;
  double x, y;

  int scan = sscanf(line, "%u,%99[^,],%d,%lf,%lf", &id, name, &nPorts, &x, &y);
  if (scan != 5)
  {
    fprintf(stderr, "Failed to parse line: %s\n", line);
    return NULL;
  }
  // printf("[Parse3] Parsed values: id=%u, name=%s, nPorts=%d, x=%.2f, y=%.2f\n", id, name, nPorts, x, y);
  Coord coord = {x, y};
  Station *station = StationCreate(id, name, nPorts, coord);
  // printf("[Parse4] Station created\n");
  return station;
}
