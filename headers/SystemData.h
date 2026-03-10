#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include "Utilis.h"
#include "BinaryTree.h"

// STRUCT & Enums declarations
typedef struct
{
  BinaryTree carTree;     //  cars binary tree
  BinaryTree stationTree; //  stations binary tree
} SystemData;

// file loader functions pointers definition
typedef void *(*ParseLineFunc)(const char *line);
typedef void (*PostProcessFunction)(void *obj, void *context);

// File Loader config struct
typedef struct
{
  const char *filename;             // file to load
  BinaryTree *targetTree;           // tree to insert to
  ParseLineFunc parser;             // function to parse a line
  PostProcessFunction processor;    // optional post processor
  void *context;                    // additional data for process
  void (*destroyObject)(void *obj); // object destroy
  int skipHeader;                   // skip header?
} FileLoaderConfig;



typedef struct LineOfCarsEntry
{
  unsigned int stationId;
  char license[LICENSE_SIZE];
} LineOfCarsEntry;

// FUNCTIONS
// 
// init systemData struct to handle data
SystemData* loadFiles();
int loadStations(BinaryTree *stationTree);
int loadCars(BinaryTree *carTree);
int loadPorts(SystemData *sys);
int loadLineOfCars(SystemData *sys);

// update files
int updateFiles(SystemData* sys);
int saveStationToFile(TreeNode* root,const char* fileName);
int saveCarToFile(TreeNode* root,const char* fileName,BinaryTree* stationTree);
int savePortsToFile(TreeNode* root,const char* fileName);
int saveLinesOfCars(TreeNode* root, const char* fileNmae);

// clean trees from memory
void cleanupSystem(SystemData *sys);


#endif