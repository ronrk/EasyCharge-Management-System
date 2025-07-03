#include "../headers/Utilis.h"
#include <stdio.h>
#include <string.h>


int loadDataFile(const FileLoaderConfig *config) {
  // 1. Validate configuration
  if(!config || !config->filename || !config->parser || !config->destroyObject) {
    fprintf(stderr, "Invalid loader configuration\n");
    return -1;
  }

  // 2. Open file
  FILE* file = fopen(config->filename, "r");
  if(!file) {
    perror(config->filename);
    return -1;
  }

  // 3. Skip header if requested
  if(config->skipHeader) {
    char header[256];
    if(!fgets(header, sizeof(header), file)) {
      fclose(file);
      return 0;  // Empty file is not an error
    }
  }

  // 4. Process lines
  char line[512];
  int count = 0;
  int lineNum = 0;
  
  while(fgets(line, sizeof(line), file)) {
    lineNum++;
    
    // Remove newline
    line[strcspn(line, "\r\n")] = '\0';
    
    // Check for overflow
    if(strlen(line) == sizeof(line)-1 && line[sizeof(line)-2] != '\n') {
      fprintf(stderr, "Line %d too long in %s\n", lineNum, config->filename);
      // Consume remaining line
      int ch;
      while((ch = fgetc(file)) != EOF && ch != '\n');
      continue;
    }

    // 5. Parse line
    void* obj = config->parser(line);
    if(!obj) {
      fprintf(stderr, "Parse error at %s:%d\n", config->filename, lineNum);
      continue;
    }

    // 6. Insert to tree if requested
    if(config->targetTree) {
      if(!insertBST(config->targetTree, obj)) {
        fprintf(stderr, "Insertion failed at %s:%d\n", config->filename, lineNum);
        config->destroyObject(obj);
        continue;
      }
    }

    // 7. Post-process
    if(config->processor) {
      config->processor(obj, config->context);
    }
    
    count++;
  }

  // 8. Cleanup
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
    return "Unknown";
  }
}

PortType parsePortType (const char* str) { 
  if(!str) return SLOW;
  if(strcasecmp(str,"FAST")==0) return FAST;
  if(strcasecmp(str,"SLOW")==0) return SLOW;
  if(strcasecmp(str,"MID")==0) return MID;

  // if invalid
  fprintf(stderr,"Unknowkn port type: '%s'\n",str);
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
    return "Unknown";
  }
}

PortStatus parsePortStatus (const char* str) { 

  if(!str) return FREE;

  if(strcasecmp(str,"occupied")==0||strcasecmp(str,"occ")==0) return OCC;
  if(strcasecmp(str,"free")==0) return FREE;
  if(strcasecmp(str,"out-of-order")==0||strcasecmp(str,"OUT")==0||strcasecmp(str,"ood")==0) return OOD;

  fprintf(stderr,"Unknown port status: '%s'\n",str);
  return FREE;
}
