#include "../headers/SystemData.h"
#include "../headers/Station.h"
#include "Menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    SystemData *sys = loadFiles();
    if(!sys) return 1;

    printf("DEBUG: Loaded %d stations\n", countNodes(sys->stationTree.root));

    mainMenu(sys);

    destroyFiles(sys);
    return 0;
}