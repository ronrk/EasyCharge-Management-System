#include "../headers/Utilis.h"
#include <stdio.h>
#include <string.h>

int processFileLines(const char* filename,FileLineProcessor processor,void* context,int skipHeader) {
  FILE * file = fopen(filename,"r");
  if(!file) {
    perror(filename);
    return 0;
  }

  char line[512];
  int lineCount = 0;

  // SkipHeader
  if(skipHeader ) {
    if(!fgets(line,sizeof(line),file)){
    fclose(file);
    return 0;
    }
  } 

  // each line
  while (fgets(line, sizeof(line), file))
  {
    line[strcspn(line,"\r\n")] = '\0';    //remove newline

    // processor
    processor(line,context);
    lineCount++;
  }
  
  fclose(file);
  return lineCount;

}

int loadDataFile(const FileLoaderConfig *config) {
  // open file
  FILE* file= fopen(config->filename,"r");
  if(!file) {
    perror("Error opening the file");
    return 0;
  }

  // skip header
  char header[256];
  if(!fgets(header,sizeof(header),file)) {
    fclose(file);
    return 1;
  };

  // each line
  char line[512];
  int count = 0;
  while(fgets(line,sizeof(line),file)) {
    // remove new line
    line[strcspn(line,"\r\n")] = '\0';

    // parse the line
    void* obj = config->parser(line);
    if(!obj) {
      fprintf(stderr,"Parse error: %s\n",line);
      continue;
    }

    if(config->targetTree) {
      insertBST(config->targetTree,obj);
    }

    if(config->processor) {
      config->processor(obj,config->context);
    }

    count++;
  }
  fclose(file);
  return count;
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
  if(!str) return SLOW;
  if(strcmp(str,"FAST")==0||strcmp(str,"fast")==0||strcmp(str,"Fast")==0) return FAST;
  if(strcmp(str,"SLOW")==0||strcmp(str,"slow")==0||strcmp(str,"Slow")==0) return SLOW;
  if(strcmp(str,"MID")==0||strcmp(str,"mid")==0||strcmp(str,"Mid")==0) return MID;

  // if invalid
  fprintf(stderr,"Unknowk port type: '%s'\n",str);
  return SLOW;
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

  if(!str) return FREE;

  if(strcmp(str,"occupied")==0||strcmp(str,"OCCUPIED")==0||strcmp(str,"Occupued")==0) return OCC;
  if(strcmp(str,"free")==0||strcmp(str,"FREE")==0||strcmp(str,"Free")==0) return FREE;
  if(strcmp(str,"out-of-order")==0||strcmp(str,"OUT-OF-ORDER")==0||strcmp(str,"Out-Of-Order")==0) return OOD;

  fprintf(stderr,"Unknowk port status: '%s'\n",str);
  return FREE;
}
