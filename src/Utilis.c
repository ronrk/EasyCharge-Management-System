#include "../headers/Utilis.h"
#include <stdio.h>
#include <string.h>

void loadFile(const char *path, BinaryTree *tree, ParseLineFunction parser)
{
  FILE * file =fopen(path,"r");
  if(!file) {
    perror("Error open File %s\n");
    return;
  }

  char line[512];
  // skip header line
  if(fgets(line,sizeof(line),file)==NULL) {
    printf("File is Empty %s\n",path);
    fclose(file);
    return;
  }

  while(fgets(line,sizeof(line),file)) {
    line[strcspn(line,"\r\n")] = 0;
    void *obj = parser(line);
    if(obj) {
      insertBST(tree,obj);
    }
  }
  fclose(file);
}


const char* portTypeToStr(PortType type) {
  switch (type)
  {
  case FAST:
    return "FAST";
  case MID:
    return "MID";
  case SLOW:
    return "SLOW";
  
  default:
    return "Unknow";
  }
}

PortType parsePortType (const char* str) { 

  if(strcmp(str,"FAST")==0||strcmp(str,"fast")==0||strcmp(str,"Fast")==0) return FAST;
  if(strcmp(str,"SLOW")==0||strcmp(str,"slow")==0||strcmp(str,"Slow")==0) return SLOW;
  if(strcmp(str,"MID")==0||strcmp(str,"mid")==0||strcmp(str,"Mid")==0) return MID;

  // if invalid
  fprintf(stderr,"Unknowk prototype: '%s'");
  return -1;
}

const char* statusToStr(PortStatus status) {
  switch (status)
  {
  case OCC:
    return "Occupied";
  case FREE:
    return "Free";
  case OOD:
    return "Out-Of-Order";
  
  default:
    return "Unknow";
  }
}

PortStatus parsePortStatus (const char* str) { 

  if(strcmp(str,"occupied")==0||strcmp(str,"OCCUPIED")==0||strcmp(str,"Occupued")==0) return OCC;
  if(strcmp(str,"free")==0||strcmp(str,"FREE")==0||strcmp(str,"Free")==0) return FREE;
  if(strcmp(str,"out-of-order")==0||strcmp(str,"OUT-OF-ORDER")==0||strcmp(str,"Out-Of-Order")==0) return OOD;

  // if invalid
  return -1;
}

BOOL skipHeader(FILE *file){
  char buffer[256];
  return fgets(buffer,sizeof(buffer),file) != NULL;
}