#include <math.h>
#include "pathFinding.h" //includes implementations for min heap as well
#include "pokeDetails.h"

class Npc{
    public:
    int currentXPos;
    int currentYPos;
    char npcType;
    char overlayedTerrainSymbol;
    int direction; //no initial direction when it is 100
    Pokemon pokemonByNPC[6]; //store pokemon for NPC
    int pokemonCount;
};

//Used for overlaying npc's and pc and projecting it to actual map grid
char overlayGrid[21][80];

//0 : Hiker Count 
//1 : Rival Count 
//2 : Pacers Count
//3 : Wanderer Count
//4 : Sentries Count
//5 : Explorers Count
int trainerArrCount[6];
// int trainerCount;

void updateNpcCoordinates(Npc **npc, int npcElement, int xCoor, int yCoor);
int initHikerAndRivalCount(int argc, char* argv[], Npc **npc, Node hCost[21][80], Node rCost[21][80], char **mapGrid);
void initializeOverlayGrid(int pcXCoor, int pcYCoor);
void randomSpawnHikers(int hikerCount, Node hCost[21][80], Npc **npc,  char **mapGrid);
void randomSpawnRivalsAndOthers(int trainerArrCount[6], Node rCost[21][80], Npc **npc, char **mapGrid);
int randomSpawnerHandler(int trainerCount, char symbol, Npc **npc, int npcIndex,char **mapGrid );
int randomDirectionGenerator();
void moveToCoor(int direction, int xCoor, int yCoor, int nextMoveCoor[2]);
void initializeNPCNodes(Node **nodes,Npc **npc, MinHeap *minHeap, int trainerCount);
void moveAndUpdateNpc(Node *minNode, Node hikerCostMap[21][80], Node rivalCostMap[21][80], Npc **npc, MinHeap *minHeap, char **mapGrid);
void updateNPCandMinodeCoor(Npc **npc, int npcIndex, int newXCoor, int newYCoor,Node *minNode);
void updateOverlayGrid(Npc **npc, Node hCost[21][80], Node rCost[21][80], int trainerCount);
void movePacers(Npc **npc, int npcIndex, int *newXCoor, int *newYCoor);
void moveWanderer(Npc **npc, int npcIndex, int *newXCoor, int *newYCoor, char **mapGrid);
void moveExplorer(Npc **npc, int npcIndex, int *newXCoor, int *newYCoor);
void printHeapAndNpcArrayDetails(Npc **npcArray, MinHeap *moveOrderHeap, int trainerCount);
int removeNPC(MinHeap* heap, int tempDetectedNPCXCoor, int tempDetectedNPCYCoor, Npc **npc, int trainerCount);
void generatePokemonForNPC(Npc *npc, int trainerCount, int currentMapX, int currentMapY);
/*
Updates npc's current coordinates
*/
void updateNpcCoordinates(Npc **npc, int npcElement, int xCoor, int yCoor){
    (*npc)[npcElement].currentXPos = xCoor;
     (*npc)[npcElement].currentYPos = yCoor;
}

/*
Initilizes hikers and rivals based on switch argument,
also initializes other npc's, setup overlay grid.
Returns no of trainers.
*/
int initHikerAndRivalCount(int argc, char* argv[], Npc **npc, Node hCost[21][80], Node rCost[21][80], char **mapGrid){

    int npcCount;

    for(int i=0; i<6; i++){
        trainerArrCount[i] = 0;
    }
    if(argc == 1 || argc == 2){
        printw("Trainer count argument not provided, default values will be used\n");
        npcCount = 4;
    }else{
        npcCount = atoi(argv[2]);

        if(npcCount == 0){
            printw("Trainer count argument cannot be 0, default values will be used\n");
            npcCount = 4;
        }
        // printf("There are %d trainers given \n", atoi(argv[2]));
    }


    //Allocation of space for npc 1d array
    (*npc) = (Npc *) malloc ((npcCount) * sizeof(Npc));

    for(int i=0; i<npcCount; i++){
        trainerArrCount[i%6]++;
    }
 
    randomSpawnHikers(trainerArrCount[0], hCost, npc, mapGrid);
    randomSpawnRivalsAndOthers(trainerArrCount, rCost, npc, mapGrid);

    return npcCount;
}

/*
Helper method to intialize overlay grid with empty values,
also sets pc symbol in overlay map
*/
void initializeOverlayGrid(int pcXCoor, int pcYCoor){
    for(int i=0; i<21; i++){
        for(int j=0; j<80 ; j++){
            overlayGrid[i][j] = 'e';
        }
    }
    overlayGrid[pcXCoor][pcYCoor] = '@';
}

/*
Randomly spawns hikers, according to hiker cost map
*/
void randomSpawnHikers(int hikerCount, Node hCost[21][80], Npc **npc,  char **mapGrid){

    for(int i=0; i<21; i++){
        for(int j=0; j<80 ; j++){
            if(hCost[i][j].distance == INT_MAX || hCost[i][j].distance < -10000){
                overlayGrid[i][j] = 'I';
            }
        }
    }

    randomSpawnerHandler(hikerCount, 'H', npc, 0, mapGrid);
}

/*
Randomly spawns rivals and other npc's, according to rival cost map
*/
void randomSpawnRivalsAndOthers(int trainerArrCount[6], Node rCost[21][80], Npc **npc, char **mapGrid){
    for(int i=0; i<21; i++){
        for(int j=0; j<80 ; j++){
            if((rCost[i][j].distance == INT_MAX || rCost[i][j].distance < -10000 )&& overlayGrid[i][j] == 'e'){
                overlayGrid[i][j] = 'I';
            }
        }
    }

    int npcIndexTracker = 0;
    npcIndexTracker = randomSpawnerHandler(trainerArrCount[1], 'R', npc, trainerArrCount[0], mapGrid);
    npcIndexTracker = randomSpawnerHandler(trainerArrCount[2], 'P', npc, npcIndexTracker , mapGrid);
    npcIndexTracker = randomSpawnerHandler(trainerArrCount[3], 'W', npc, npcIndexTracker , mapGrid);
    npcIndexTracker = randomSpawnerHandler(trainerArrCount[4], 'S', npc, npcIndexTracker , mapGrid);
    npcIndexTracker = randomSpawnerHandler(trainerArrCount[5], 'E', npc, npcIndexTracker , mapGrid);
}

/*
Helper method for randomly spawning trainers 
*/
int randomSpawnerHandler(int trainerCount, char symbol, Npc **npc, int npcElement, char **mapGrid){

    for(int i=npcElement; i<npcElement + trainerCount;){
        int randXCoor = rand()%18 + 1;
        int randYCoor = rand()%78 + 1;

        if(overlayGrid[randXCoor][randYCoor] == 'e'){
            overlayGrid[randXCoor][randYCoor] = symbol;
            (*npc)[i].npcType = symbol;
            (*npc)[i].direction = 100;
            (*npc)[i].overlayedTerrainSymbol = mapGrid[randXCoor][randYCoor];
            updateNpcCoordinates(npc, i, randXCoor, randYCoor);
            i++;
        }
    }

    return npcElement + trainerCount;
}

/*
Initializes NPC nodes for min_heap
Assign properties of trainers to nodes,
Adds nodes to min heap
*/
void initializeNPCNodes(Node **nodes, Npc **npc, MinHeap *minHeap, int trainerCount){

    for(int i=0; i<trainerCount; i++){
            (*nodes)[i].xCoor = (*npc)[i].currentXPos;
            (*nodes)[i].yCoor = (*npc)[i].currentYPos;
            (*nodes)[i].index = i;
            (*nodes)[i].distance = 0;
            (*nodes)[i].charType = (*npc)[i].npcType;

    }

    for(int i=0; i<trainerCount; i++){
        insert_minheap(minHeap, (*nodes)[i]);
    }
}

/*
Randomly generates a direction for npc's movement
0:NW, 1:N, 2:NE, 3:E, 4:SE, 5:S, 6:SW, 7:W
*/
int randomDirectionGenerator(){
    return rand() % 8;
}

/*
Returns a set of coordinates for movement in a certiain direction
*/
void moveToCoor(int direction, int xCoor, int yCoor, int nextMoveCoor[2]){

    //add different switch cases here for different directions
    switch(direction){
        case 0:
        nextMoveCoor[0] = xCoor -1;
        nextMoveCoor[1] = yCoor -1;
        break;

        case 1:
        nextMoveCoor[0] = xCoor -1;
        nextMoveCoor[1] = yCoor;
        break;

        case 2:
        nextMoveCoor[0] = xCoor -1;
        nextMoveCoor[1] = yCoor +1;
        break;

        case 3:
        nextMoveCoor[0] = xCoor;
        nextMoveCoor[1] = yCoor +1;
        break;

        case 4:
        nextMoveCoor[0] = xCoor +1;
        nextMoveCoor[1] = yCoor +1;
        break;

        case 5:
        nextMoveCoor[0] = xCoor +1;
        nextMoveCoor[1] = yCoor;
        break;

        case 6:
        nextMoveCoor[0] = xCoor +1;
        nextMoveCoor[1] = yCoor -1;
        break;

        case 7:
        nextMoveCoor[0] = xCoor;
        nextMoveCoor[1] = yCoor -1;
        break;
    }
}

/*
Update npc coordinate,and minNode's coordinates and distance based on cost maps
*/
void moveAndUpdateNpc(Node *minNode, Node hikerCostMap[21][80], Node rivalCostMap[21][80], Npc **npc,MinHeap *minHeap, char **mapGrid){
    
    char tempSymbol = minNode->charType;
    int npcIndex = minNode->index;
    int newXCoor;
    int newYCoor;

    if(tempSymbol == 'H'){
        newXCoor = hikerCostMap[minNode->xCoor][minNode->yCoor].pred[0];
        newYCoor = hikerCostMap[minNode->xCoor][minNode->yCoor].pred[1];
        minNode ->distance = minNode->distance + costOfTerrain(hikerCostMap[minNode->xCoor][minNode->yCoor].charType, 'H');

        if(overlayGrid[newXCoor][newYCoor]!= '@' && (overlayGrid[newXCoor][newYCoor] == 'e' || overlayGrid[newXCoor][newYCoor] == 'I')){
            updateNPCandMinodeCoor(npc, npcIndex, newXCoor, newYCoor, minNode);
        }
    }
    
    else{
        if(tempSymbol == 'R'){
            newXCoor = rivalCostMap[minNode->xCoor][minNode->yCoor].pred[0];
            newYCoor = rivalCostMap[minNode->xCoor][minNode->yCoor].pred[1];
        }

        else if(tempSymbol == 'P'){
            movePacers(npc, npcIndex, &newXCoor, &newYCoor);
        }

        else if(tempSymbol == 'W'){
            moveWanderer(npc, npcIndex, &newXCoor, &newYCoor, mapGrid);
        }

        else if(tempSymbol == 'S'){
             minNode ->distance = INT_MAX;
            return;
        }

        else if(tempSymbol == 'E'){
            moveExplorer(npc, npcIndex, &newXCoor, &newYCoor);
            minNode ->distance = minNode->distance + costOfTerrain(rivalCostMap[minNode->xCoor][minNode->yCoor].charType,'R');
        }

        minNode ->distance = minNode->distance + costOfTerrain(rivalCostMap[minNode->xCoor][minNode->yCoor].charType,'R');

        if(overlayGrid[newXCoor][newYCoor]!= '@' && overlayGrid[newXCoor][newYCoor] == 'e'){
            updateNPCandMinodeCoor(npc, npcIndex, newXCoor, newYCoor, minNode);
        }
    }
    
  //add code for future here once npc has been defeated  
}

/*
Helper method for pacer's movement
-determine's initial direction
-turns back if terrain cannot be traversed
*/
void movePacers(Npc **npc, int npcIndex, int *newXCoor, int *newYCoor){

    int pacerCurrentX = (*npc)[npcIndex].currentXPos;
    int pacerCurrentY = (*npc)[npcIndex].currentYPos;
    int newTempCoor[2];
    int direction = (*npc)[npcIndex].direction;

    //pacers with no initial direction
    if(direction == 100){
        direction = randomDirectionGenerator();
        moveToCoor(direction, pacerCurrentX, pacerCurrentY, newTempCoor);

        while(overlayGrid[newTempCoor[0]][newTempCoor[1]] != 'e'){
            direction = randomDirectionGenerator();
            moveToCoor(direction, pacerCurrentX, pacerCurrentY, newTempCoor);
        }
    }

    //pacers with initial direction
    else{
        moveToCoor(direction, pacerCurrentX, pacerCurrentY, newTempCoor);
        
        if(overlayGrid[newTempCoor[0]][newTempCoor[1]] != 'e'){
            direction = (direction + 4) % 8; //reverses direction 
            moveToCoor(direction, pacerCurrentX, pacerCurrentY, newTempCoor);
        }

    }
    (*npc)[npcIndex].direction = direction;
    *newXCoor = newTempCoor[0];
    *newYCoor = newTempCoor[1];
}

/*
Helper method for wanderers movement
*/
void moveWanderer(Npc **npc, int npcIndex, int *newXCoor, int *newYCoor, char **mapGrid){
    int wanderCurrentX = (*npc)[npcIndex].currentXPos;
    int wanderCurrentY = (*npc)[npcIndex].currentYPos;
    int newTempCoor[2];
    int direction = (*npc)[npcIndex].direction;
    char terrainSpawned = (*npc)[npcIndex].overlayedTerrainSymbol;

    if(direction == 100){
        direction = randomDirectionGenerator();
        moveToCoor(direction, wanderCurrentX, wanderCurrentY, newTempCoor);

        while(overlayGrid[newTempCoor[0]][newTempCoor[1]] != 'e' || mapGrid[newTempCoor[0]][newTempCoor[1]] != terrainSpawned){
            direction = randomDirectionGenerator();
            moveToCoor(direction, wanderCurrentX, wanderCurrentY, newTempCoor);
        }
    }

    else{
        moveToCoor(direction, wanderCurrentX, wanderCurrentY, newTempCoor);
        
        while(overlayGrid[newTempCoor[0]][newTempCoor[1]] != 'e' || mapGrid[newTempCoor[0]][newTempCoor[1]] != terrainSpawned){
            direction = randomDirectionGenerator();
            moveToCoor(direction, wanderCurrentX, wanderCurrentY, newTempCoor);
        }
    }

    (*npc)[npcIndex].direction = direction;
    *newXCoor = newTempCoor[0];
    *newYCoor = newTempCoor[1];
}

/*
Helper method for explorer's movement
*/
void moveExplorer(Npc **npc, int npcIndex, int *newXCoor, int *newYCoor){
    int expCurrentX = (*npc)[npcIndex].currentXPos;
    int expCurrentY = (*npc)[npcIndex].currentYPos;
    int newTempCoor[2];
    int direction = (*npc)[npcIndex].direction;

     if(direction == 100){
        direction = randomDirectionGenerator();
        moveToCoor(direction, expCurrentX, expCurrentY, newTempCoor);

        while(overlayGrid[newTempCoor[0]][newTempCoor[1]] != 'e'){
            direction = randomDirectionGenerator();
            moveToCoor(direction, expCurrentX, expCurrentY, newTempCoor);
        }
     }

     else{
        moveToCoor(direction, expCurrentX, expCurrentY, newTempCoor);
        
        if(overlayGrid[newTempCoor[0]][newTempCoor[1]] != 'e'){
            direction = randomDirectionGenerator();
            moveToCoor(direction, expCurrentX, expCurrentY, newTempCoor);
        }

    }
    (*npc)[npcIndex].direction = direction;
    *newXCoor = newTempCoor[0];
    *newYCoor = newTempCoor[1];
}


/*
Helper method to update npc and minNode coordinates
*/
void updateNPCandMinodeCoor(Npc **npc, int npcIndex,int newXCoor, int newYCoor,Node *minNode){
        (*npc)[npcIndex].currentXPos = newXCoor;
        (*npc)[npcIndex].currentYPos = newYCoor;
        minNode->xCoor = newXCoor;
        minNode->yCoor = newYCoor;
}

/*
Updates overlay grid, maintains rival and hiker boundary areas
*/
void updateOverlayGrid(Npc **npc, Node hCost[21][80], Node rCost[21][80], int trainerCount){
    int npcXCoor;
    int npcYCoor;

    for(int i=0; i<21; i++){
        for(int j=0; j<80 ; j++){
            if(hCost[i][j].distance == INT_MAX || hCost[i][j].distance < -10000){
                overlayGrid[i][j] = 'I';
            }
        }
    }

     for(int i=0; i<21; i++){
        for(int j=0; j<80 ; j++){
            if((rCost[i][j].distance == INT_MAX || rCost[i][j].distance < -10000 )&& overlayGrid[i][j] == 'e'){
                overlayGrid[i][j] = 'I';
            }
        }
    }

    int k = 0;
    for(int i=0; i<trainerCount;){

        if((*npc)[i + k].npcType == '-'){
            k++;
            continue;
        }

        npcXCoor = (*npc)[i + k].currentXPos;
        npcYCoor = (*npc)[i + k].currentYPos;
        overlayGrid[npcXCoor][npcYCoor] = (*npc)[i+k].npcType;
        i++;
    }
}

/*
Method used for debugging
*/
void printHeapAndNpcArrayDetails(Npc **npcArray, MinHeap *moveOrderHeap, int trainerCount){
    wprintw(win, "Heap size: %d", moveOrderHeap->size);
    wprintw(win, "Npc array : ");

    for(int i=0; i<4; i++){
        wprintw(win," %c ", (*npcArray)[i].npcType);
    }
}


/*Method to remove NPC from moveOrderHeap, npcArray*/
int removeNPC(MinHeap* heap, int tempDetectedNPCXCoor, int tempDetectedNPCYCoor, Npc **npc, int trainerCount){

    //set npc distance to int min
    decreaseKey(heap, tempDetectedNPCXCoor, tempDetectedNPCYCoor, INT_MIN);

     //extract node from heap for index info, so that we can delete from npc array
     Node npcToRemoveNode = get_min(heap);
     int npcToRemoveIndex = npcToRemoveNode.index;
     
     //Changing npc symbol to -, npc to no longer exists
     (*npc)[npcToRemoveIndex].npcType = '-';

      //removing npc from heap
      delete_minimum(heap);
      trainerCount--;

      return trainerCount;
}



/*
Assign randomly generated pokemons for NPC's
60% chance trainer gets n+1 pokemons
*/
void generatePokemonForNPC(Npc **npc, int trainerCount, int currentMapX, int currentMapY){

  
    // Generates random pokemon, assigns pokemon to NPC's
    for(int i=0; i<trainerCount; i++){
       
        int j =0;
        int randomPokemonCount = rand()%5 + 1;

        while(j<randomPokemonCount){
            Pokemon p = initiateSpawnPokemonSequence(currentMapX,currentMapY);
            ((*npc)[i]).pokemonByNPC[j] = p;
            ((*npc)[i]).pokemonCount++;
            j++;
        }
    

        // // 60% chance for NPC to have additional pokemon
        if(rand()%6 == 1){
            Pokemon p2 = initiateSpawnPokemonSequence(currentMapX,currentMapY);
            ((*npc)[i]).pokemonByNPC[j] = p2;
            ((*npc)[i]).pokemonCount++;
        }
    }

   
}



