#include "../headers/Station.h"
#include "../headers/Queue.h"
#include "../headers/Port.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord)
{
  Station *s = malloc(sizeof(Station));
  if (!s)
  {
    perror("Failed Allocation Memory Create Station\n");
    return NULL;
  }
  s->id = id;
  s->name = strdup(name);
  s->nPorts = nPorts;
  s->coord = coord;
  s->portsList = NULL;
  s->nCars = 0;
  s->left = NULL;
  s->right = NULL;

  s->qCar = createQueue();
  if (!s->qCar)
  {
    free(s->name);
    free(s);
    return NULL;
  }
  return s;
}

void StationDestroy(Station *station)
{
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
  const Station *s1 = a;
  const Station *s2 = b;
  return (s1->id > s2->id) - (s1->id < s2->id);
}

void printStation(const void *data)
{
  const Station *station = data;
  printf("Station ID: %u  |  Name: %s. |  Ports: %d  |  Cars: %d\n", station->id, station->name, station->nPorts, station->nCars);
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
  Coord coord = {x, y};

  return StationCreate(id, name, nPorts, coord);
}
