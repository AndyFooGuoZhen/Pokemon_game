#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "npc.h"

#define WHITE_BLACK 1
#define GREEN_BLACK 2
#define YELLOW_BLACK 3
#define MAGENTA_BLACK 4
#define RED_BLACK 5
#define BLUE_BLACK 6
#define CYAN_BLACK 7
#define BLACK_WHITE 8

//Top section       : Struct Maps and related functions
//Middle section    : Functions to build terrains and borders
//Bottom section    : Struct World (connecting maps to form world)

struct Map{
    char** grid;    
    int *exits;     
    int *mapCoor; 
    MinHeap *hikerHeap;  
    Node hikerNodes[21][80];
    MinHeap *rivalHeap;
    Node rivalNodes[21][80];
    MinHeap *moveOrderHeap;
    Node *npcNodes;
    Npc *npcArray;
    int trainerCountOnMap;

    //coordinate of pc on map
    int mapPlayerX;
    int mapPlayerY;
};

void initializeArray(struct Map *map);
void printCharTerrain(struct Map *map);
void printCharTerrainWithOverlay(struct Map *map, char overlayyGrid[21][80]);
void copyMap(struct Map *map, char terrainGrid[21][80]);
void copyExitDetails(struct Map *map, int exitsCoordinates[8]);
void assignMapCoor(struct Map *map, int xCoor, int yCoor);
int scanMapForCharacterCounts(char **grid , char symbol);
int scanMapForCharacterCoordinates(char **mapGrid, char symbol, int ***coordinates);
void randomSpawnerForSelectedSymbol(char **mapGrid, char symbol, int ***coordinates, int arrLength, int coorForSymbol[2]);


//Header for terrain generation
//Code for creating a 2d array, representing a map
time_t time( time_t *second );
void randomGenerator(char seed);
void initializeTerrain(char terrainGrid[21][80]);
void printTerrain(char terrainGrid[21][80]);
int printWithColor(char terrainElement);
void initColors();

char randomTerrainSelector();
void randomTerrainFiller(char terrainGrid[21][80], char regionSpawnCoordinates[]);
void growRegion(char terrainGrid[21][80], int xCoor, int yCoor, char terrain, int expansionWidth, int overrrideRegion);
int  checkMapForEmptyTerrain(char terrainGrid[21][80]);
void randomRegionCoordinateGenerator(char regionSpawnCoordinates[]);
void buildTerrainBorder(char terrainGrid[21][80]);
void buildRandomExits(char terrainGrid[21][80], int exitsCoordinates[8]);
void buildRandomPath(char terrainGrid[21][80], int exitsCoordinates[8], struct Map *map);
void extendPathWithDirection(char terrainGrid[21][80], int xCoor, int yCoor, char direction, int pathLengthToExtend, struct Map *map);
void connectPaths(char terrainGrid[21][80], int coor1, int coor2, int randomLengthOfPath, int direction, struct Map *map);
int pokeCenterandMartRandomizer(struct Map *map);
int rows = 21;
int cols = 80;
int regions = 30;

/*
Initializes the 2d array for map by allocating space dynamically
Also initializes array for exit coordinates
*/
void initializeArray(struct Map *map){

    map->grid = (char **)malloc(21 * sizeof(char*));

    for(int i=0; i<21; i++){
        map->grid[i] = (char *)malloc( 80 * sizeof(char));
    }

    map->exits = (int *)malloc(8 * sizeof(int));
}

/*
Print out map with character overlays
*/
void printCharTerrainWithOverlay(struct Map *map, char overlayGrid[21][80]){
    int colorPairNumber = 0;
    initColors();
    for(int i=0; i<21; i++){
        for(int j=0; j<80; j++){
            if(overlayGrid[i][j] != 'I' && overlayGrid[i][j] !='e'){

                colorPairNumber=  printWithColor(overlayGrid[i][j]);
                wattron(win, COLOR_PAIR(colorPairNumber));
                wprintw(win, "%c", overlayGrid[i][j]);
                wattroff(win,COLOR_PAIR(colorPairNumber));

            }else{
                colorPairNumber = printWithColor(map->grid[i][j]);
                wattron(win,COLOR_PAIR(colorPairNumber));
				wprintw(win, "%c", map->grid[i][j]);
                wattroff(win,COLOR_PAIR(colorPairNumber));
            }
          

        }wprintw(win, "\n");
    }wprintw(win, "\n");
}

/*
Init color pairs
*/
void initColors(){
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    init_pair(8, COLOR_BLACK, COLOR_WHITE);
}

/*
Copies generated 2d array to char** of map
*/
void copyMap(struct Map *map, char terrainGrid[21][80]){
    for(int i=0; i<21; i++){
			for(int j=0; j<80; j++){
				map->grid[i][j] = terrainGrid[i][j];
			}
		}
}

/*
Copies exit coordinates 
*/
void copyExitDetails(struct Map *map, int exitsCoordinates[8]){
    for(int i=0; i<8; i++){
        map->exits[i] = exitsCoordinates[i];
    }
}

/*
Assign map coor from Worlds
*/
void assignMapCoor(struct Map *map, int xCoor, int yCoor){
    map->mapCoor = (int *)malloc(2 * sizeof(int));
    map->mapCoor[0] = xCoor;
    map->mapCoor[1] = yCoor;
}

/*
Scans maps and returns counts of given symbol
Used for allocating space for symbol coordinates
*/
int scanMapForCharacterCounts(char **mapGrid , char symbol){
    int count = 0;
    
    for(int i=1; i<20; i++){
        for(int j= 1; j<79; j++){
            if(mapGrid[i][j] == symbol){
                count++;
            }
        }
    }
    return count;
}

/*
NOTE : To pass in dynamically allocated 2d pointer arr (to be modified), use triple *
https://stackoverflow.com/questions/1918120/c-segmentation-fault-when-function-modifies-dynamically-allocated-2d-array
Scans maps and returns an array of coordinates for selected symbol
Also returns length of 2d pointer array
*/
int scanMapForCharacterCoordinates(char **mapGrid, char symbol, int ***coordinates){

    int count = scanMapForCharacterCounts(mapGrid, symbol);
    int arrayIndexCounter = 0;

    *coordinates = (int **)malloc(count * sizeof(int *));
    for(int i=0; i<count ; i++){
        (*coordinates)[i] = (int *)malloc(2 * sizeof(int));
    }

    for(int i=1; i<20 ; i++){
        for(int j=1; j<79; j++){

            if(mapGrid[i][j] == symbol){
                (*coordinates)[arrayIndexCounter][0] = i;
                (*coordinates)[arrayIndexCounter][1] = j;
                arrayIndexCounter++;
            }
        }
    }

    return count;
}

/*
Randomly chooses a set of coordinate to spawn given symbol
@param coordinates (coordinates generated using scanMApForCharacterCoordinates)
Returns coordinate of the spawed symbol
*/
void randomSpawnerForSelectedSymbol(char **mapGrid, char symbol, int ***coordinates, int arrLength, int coorForSymbol[2]){

    int randomSelectedCoordinate = random()%arrLength + 1;
    // mapGrid[(*coordinates)[randomSelectedCoordinate][0]][(*coordinates)[randomSelectedCoordinate][1]] = symbol;
    coorForSymbol[0] = (*coordinates)[randomSelectedCoordinate][0];
    coorForSymbol[1] = (*coordinates)[randomSelectedCoordinate][1];
}

/*---------------- CODE FOR TERRAIN GENERATION BELOW---------------------------------------------------------*/

/*
Method used to print elements in the map with color
Trees and forest    --> green
Boulders            --> white
Long Grass          --> yellow
Clearings           --> black
Paths and exits     --> magenta
Pokemarts           --> red
PokemonCenters      --> blue
*/
int printWithColor(char terrainElement){

    switch(terrainElement){
        case '%':
        case '0':
        case '.':
            return 1;
            break;
        
        case '^':
            return 2;
            break;
        
        case ':':
        case 'E':
            return 3;
            break;

        case 'W':
            return 8;           
             break;
        
        case '#':
        case 'R':
            return 4;
            break;
        
        case 'M':
        case 'S':
            return 5;
            break;

        case 'C':
        case 'H':
            return 6;
            break;
        
        case 'P':
            return 7;
            break;
        
        default:
            return 1;
    }
}


/*
Initializes the map with 0 terrain elements
*/
void initializeTerrain(char terrainGrid[21][80]){

    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            terrainGrid[i][j] = '0';
        }
    }
}

/*
Prints out the map 
*/
void printTerrain(char terrainGrid[21][80]){
    for(int i=0; i<rows; i++){
			for(int j=0; j<cols; j++){
                printWithColor(terrainGrid[i][j]);
				printf("%c", terrainGrid[i][j]);
                printf("\033[0m");
			}
			printf("\n");
		}
		printf("\n");
}

/*
Generates map with random terrains
*/
void randomTerrainFiller(char terrainGrid[21][80], char regionSpawnCoordinates[]){

    int expansionWidth = 0;
    char terrainSelectedArray[regions];
    
    /// Randomly selects terrain elements
    for(int i=0; i<regions; i++){
        terrainSelectedArray[i] = randomTerrainSelector();
    }

    while(checkMapForEmptyTerrain(terrainGrid) == 1){

        for(int i=0; i<regions * 2; i+=2){
            growRegion(terrainGrid, regionSpawnCoordinates[i], regionSpawnCoordinates[i+1],terrainSelectedArray[i/2],expansionWidth, 0);
        }

        expansionWidth++;
    }

}

/*
Helper function to randomly select a terrain element to be generated
*/
char randomTerrainSelector(){
    int randomNo = random()%4 +1;

    switch(randomNo){

        case 1:
            return ':';
        
        case 2: 
            return '.';
        
        case 3:
            return '^';
        
        case 4:
            return '%';
        
        default:
            return 0;
    }
}

/*
Helper function used to grow a region with specific terrain selection
@parameter overrideregion  
    0   : used for growing normal terrains
    1   : used for growing pokecenters and pokemarts
*/
void growRegion(char terrainGrid[21][80], int xCoor, int yCoor, char terrain, int expansionWidth, int overrrideRegion){

        for(int i = xCoor - expansionWidth; i<= xCoor + expansionWidth ; i++){
            if(overrrideRegion == 0){
                if(i < 0 || i >rows -1){
			continue;
            }}
            else if(overrrideRegion == 1){ 
                if(i < 1 || i >rows - 2){
			continue;
            }}
                    
           

            for(int j = yCoor - expansionWidth; j<= yCoor + expansionWidth ; j++){
                if(overrrideRegion == 0){
                    if(j < 0 || j>cols -1 || terrainGrid[i][j] !=  '0'){
				continue;
                }}
                else if(overrrideRegion == 1){
                    if(j < 2 || j>cols -2 || terrainGrid[i][j] == '#' || terrainGrid[i][j] == 'M' || terrainGrid[i][j] == 'C'){
				continue;
                }
			}

            terrainGrid[i][j] = terrain;
            }
        }
}

/*
Checks the whole map for empty terrain
returns 1 -> empty terrain detected in map
returns 0 -> map is fully filled with terrains
*/
int checkMapForEmptyTerrain(char terrainGrid[21][80]){
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            if (terrainGrid[i][j] == '0'){
                return 1;
            }
        }
    }

    return 0;
}
/*
Function that generates coordinates for multiple regions,
used to spawn random terrains
*/
void randomRegionCoordinateGenerator(char regionSpawnCoordinates[]){

    for(int i=0; i<regions * 2; i++){
        if(i%2 == 0){
            regionSpawnCoordinates[i] = random()%21;
        }else{
            regionSpawnCoordinates[i] = random()%80;
        }
    }
}

/*
Function to build borders around the map
*/
void buildTerrainBorder(char terrainGrid[21][80]){

    for(int i=0; i<cols; i++){
        terrainGrid[0][i] = '%';
        terrainGrid[rows-1][i] = '%';
    }

    for(int j=0; j<rows; j++){
        terrainGrid[j][0] = '%';
        terrainGrid[j][cols -1] = '%';
    }
}

/*
Function to build random exits around the map
*/
void buildRandomExits(char terrainGrid[21][80],int exitsCoordinates[8]){
    int colsBoundary = cols - 2;
    int rowsBoundary = rows -2;
    int randomValue = 0;
   
    randomValue= (random()%colsBoundary) + 1;
    terrainGrid[0][randomValue] = '#';
    exitsCoordinates[0] = 0;
    exitsCoordinates [1] = randomValue;

    randomValue = (random()%colsBoundary) + 1;
    terrainGrid[rows -1][randomValue] = '#';
    exitsCoordinates[2] = rows - 1;
    exitsCoordinates [3] = randomValue;

    randomValue = (random()%rowsBoundary) + 1;
    terrainGrid[randomValue][0] = '#';
    exitsCoordinates[4] = randomValue;
    exitsCoordinates [5] = 0;

    randomValue = (random()%rowsBoundary) + 1;
    terrainGrid[randomValue][cols-1] = '#';
    exitsCoordinates[6] = randomValue;
    exitsCoordinates [7] = cols - 1;
}

/*
Function to connect exits by forming a path
*/
void buildRandomPath(char terrainGrid[21][80], int exitsCoordinates[8], struct Map *map){

    int midVerticalLength= (rows - 2) / 2;
    int midHorizontalLength = (cols -2)/2;

    //Extends path of top and bottom exit with random length
    int randomLengthOfPath = ceil((random()%midVerticalLength + 1));
    int remainingLengthOfPath = rows - 1 - randomLengthOfPath ;
    extendPathWithDirection(terrainGrid, exitsCoordinates[0], exitsCoordinates[1], 'b', randomLengthOfPath, map);
    extendPathWithDirection(terrainGrid, exitsCoordinates[2], exitsCoordinates[3], 't', remainingLengthOfPath, map);
    connectPaths(terrainGrid, exitsCoordinates[1], exitsCoordinates[3], randomLengthOfPath, 1, map);

    //Extends path of left and right exit with random length
    randomLengthOfPath = ceil((random()%midHorizontalLength + 1));
    remainingLengthOfPath = cols - 1 - randomLengthOfPath ;
    extendPathWithDirection(terrainGrid, exitsCoordinates[4], exitsCoordinates[5], 'r', randomLengthOfPath, map);
    extendPathWithDirection(terrainGrid, exitsCoordinates[6], exitsCoordinates[7], 'l', remainingLengthOfPath, map);
    connectPaths(terrainGrid, exitsCoordinates[4], exitsCoordinates[6], randomLengthOfPath, 2, map);
}

/*
Helper function to extend path according to given direction,
Used for connecting paths as well, and building pokemarts, pokecenters
Guranteed to have at least one pokemart and pokecenter
*/
void extendPathWithDirection(char terrainGrid[21][80], int xCoor, int yCoor, char direction, int pathLengthToExtend, struct Map *map){

    int continueDirectionOfMovement = 1;
    //top or bottom

    if(direction == 't' || direction == 'b'){

        if (direction == 't'){
            continueDirectionOfMovement = -1;
        }
        for(int i = 0; i<=pathLengthToExtend; i++){
        terrainGrid[xCoor + i*(continueDirectionOfMovement)][yCoor] = '#';
        if(i == 3 && pokeCenterandMartRandomizer(map)==1){
            growRegion(terrainGrid, xCoor + i*(continueDirectionOfMovement), yCoor, 'M',1,1);
        }
        }
    }else if(direction == 'r' || direction == 'l'){
        if (direction == 'l'){
            continueDirectionOfMovement = -1;
        }
        for(int i = 0; i<=pathLengthToExtend; i++){
        terrainGrid[xCoor][yCoor + i*(continueDirectionOfMovement)] = '#';
         if(i == 10 && pokeCenterandMartRandomizer(map)==1){
             growRegion(terrainGrid, xCoor, yCoor + i*(continueDirectionOfMovement) , 'C',1,1);
        }
        }
        }
    }
   
/*
Helper function to connect paths from exits
@parameter 
 direction :    1--> connects top to bottom exit
                2--> connects left to right exit
*/
void connectPaths(char terrainGrid[21][80], int coor1, int coor2, int randomLengthOfPath, int direction, struct Map *map){
    int lengthDifference = coor1 - coor2;

    if(direction == 1){
        if(lengthDifference<0){
            extendPathWithDirection(terrainGrid, randomLengthOfPath, coor1, 'r', abs(lengthDifference), map);
        }else{
            extendPathWithDirection(terrainGrid, randomLengthOfPath, coor1, 'l', abs(lengthDifference), map);
        }
    }else{
        if(lengthDifference<0){
            extendPathWithDirection(terrainGrid, coor1, randomLengthOfPath, 'b', abs(lengthDifference), map);
        }else{
            extendPathWithDirection(terrainGrid, coor1, randomLengthOfPath, 't', abs(lengthDifference), map);
        }
    }
}

/*
Randomizer function to determine if pokemart of pokecenter 
should be generated
Returns 1 : generate 
        0 : don't generate
*/
int pokeCenterandMartRandomizer(struct Map *map){
    
    int currentMapX = map->mapCoor[0];
    int currentMapY = map->mapCoor[1];

    if(currentMapX == 200 && currentMapY == 200){
        return 1;
    }

    if (rand()%(abs(currentMapX - 200) + abs(currentMapY - 200)) + 1 == 1)
    {
        return 1;
    }

    return 0;
}


/*---------------- CODE FOR WORLD GENERATION BELOW---------------------------------------------------------*/
//Header for world

class World{
    public:
    Map *maps[401][401];
    int currentMapXCoor;
    int currentMapYCoor;
};

void initializeWorldMaps(World *world);
void initializeCurrentMapAtCoordinate(World *world, int x, int y);
void generateRandomMap(Map *map, struct World *world,int matchExits );
void matchExitsFromSurroundingMaps(World *world, int exitsCoordinates[], char terrainGrid[21][80]);
int outofMapBoundsChecker(int coordinate);
void sealExits(World *world, char terrainGrid[21][80]);
char moveMapByMap(World *world, char c);
/*
Setting all maps in the world to NULL
*/
void initializeWorldMaps(World *world){
    for(int i=0; i<401; i++){
        for(int j=0; j<401; j++){
            world->maps[i][j] = NULL;
        }
    }
}

/*
Initiliazes map at the current coordinate
*/
void initializeCurrentMapAtCoordinate(World *world, int x, int y){
    world->maps[x][y] = (Map*) malloc(sizeof(Map));
    initializeArray(world->maps[x][y]); 
    world->currentMapXCoor = x;
    world->currentMapYCoor = y;
}

/*
Handles moving between maps (n,e,s,w,f)
*/
char moveMapByMap(World *world, char c){
    wclear(win);
    int tempX = world->currentMapXCoor;
    int tempY = world->currentMapYCoor;
    int flag = 0;
    
    switch(c){
        case 'n':
            tempX--;
            break;
        case 'e':
            tempY++;
            break;
        case 's':
            tempX++;
            break;
        case 'w':
            tempY--;
            break;

        case 'f':
            wclear(win);
            wrefresh(win);
            wprintw(win, "Enter x and y coordinate of map to fly to : ");
            wrefresh(win);
            wscanw(win," %d %d", &tempY, &tempX); //external and internal coordinates work differently 

            if(outofMapBoundsChecker(tempX + 200) == 0 && outofMapBoundsChecker(tempY + 200) == 0){
                wprintw(win,"Flying to map with coordinate (%d, %d)\n",tempY, tempX);
                wrefresh(win);
                usleep(1000000);
                tempX = (-1) * tempX + 200;
                tempY = tempY + 200;
                world->currentMapXCoor = tempX;
                world->currentMapYCoor = tempY;
                flag = 1;

            }else{
                wprintw(win,"Coordinate given is out of bounds\n");
                wrefresh(win);
                usleep(1000000);
                flag = 1;
                return 'F';
            }
            break;
    }

    if((flag == 0) && (outofMapBoundsChecker(tempX)==1 || outofMapBoundsChecker(tempY)==1)){
        wprintw(win,"Out of bounds!!!\n");
        wrefresh(win);
        usleep(10000000);
    }else{
        world->currentMapXCoor = tempX;
        world->currentMapYCoor = tempY;
    }
    
    int currentMapX = world->currentMapXCoor;
    int currentMapY = world->currentMapYCoor;

    if(world->maps[currentMapX][currentMapY] == NULL){
        initializeCurrentMapAtCoordinate(world, currentMapX, currentMapY);
        assignMapCoor(world->maps[currentMapX][currentMapY], currentMapX, currentMapY);
        generateRandomMap(world->maps[currentMapX][currentMapY], world, 1); //generate map, considering matching exits
        return 'T';
    }

    return 'F';

}

/*
Helper method to check if given coordinate is out of bounds (Map)
Returns 1 : out of bounds
        0 : not out of bounds
*/
int outofMapBoundsChecker(int coordinate){

    if(coordinate < 0 || coordinate >400){
        return 1;
    }

    return 0;
}

/*
Generates random map, assigns to map struct
    @param matchExits 
        1 : random map generated must have matching exits with other maps
        0 : random map generated can have random exits

NOTE: MUST BE IN MAIN (uses struct world and map)
*/
void generateRandomMap(Map *map,World *world, int matchExits){
    char terrainGrid[21][80];
    char regionSpawnCoordinates[regions * 2];
    int exitsCoordinates[8]; // top->bottom->left->right exits

    srand(time(NULL));
    initializeTerrain(terrainGrid);
    randomRegionCoordinateGenerator(regionSpawnCoordinates);
    randomTerrainFiller(terrainGrid, regionSpawnCoordinates);
    buildTerrainBorder(terrainGrid);
    buildRandomExits(terrainGrid, exitsCoordinates);

    if(matchExits == 1){
        matchExitsFromSurroundingMaps(world, exitsCoordinates, terrainGrid);
    }

    buildRandomPath(terrainGrid, exitsCoordinates, map);
    sealExits(world, terrainGrid);
    copyMap(map, terrainGrid);
    copyExitDetails(map,exitsCoordinates);
}

/*
Checks surrounding maps, match exits of surrounding maps
@Return exitCoordinates that matches surroundings, erases exits to be replaced
*/
void matchExitsFromSurroundingMaps(World *world, int exitsCoordinates[], char terrainGrid[21][80] ){

    int currentMapX = world->currentMapXCoor;
    int currentMapY = world->currentMapYCoor;
    int tempMapX;
    int tempMapY;

    //Top exit of current map == Bottom exit of map above
     currentMapX--;
    if(outofMapBoundsChecker(currentMapX) == 0 && (world->maps[currentMapX][currentMapY]!= NULL) ){
        terrainGrid[exitsCoordinates[0]][exitsCoordinates[1]] = '%';
        tempMapY = world->maps[currentMapX][currentMapY]->exits[3];
        exitsCoordinates[1] = tempMapY;
    }

    currentMapX = world->currentMapXCoor;

    //Bottom exit of current map == Top exit of map below
    currentMapX++;
    if(outofMapBoundsChecker(currentMapX) == 0 && (world->maps[currentMapX][currentMapY]!= NULL)){
        terrainGrid[exitsCoordinates[2]][exitsCoordinates[3]] = '%';
        tempMapY = world->maps[currentMapX][currentMapY]->exits[1];
        exitsCoordinates[3] = tempMapY;
    }

    currentMapX = world->currentMapXCoor;

    //Left exit of current map == right exit of left map
    currentMapY--;
    if(outofMapBoundsChecker(currentMapY) == 0 && (world->maps[currentMapX][currentMapY]!= NULL)){
         terrainGrid[exitsCoordinates[4]][exitsCoordinates[5]] = '%';
        tempMapX = world->maps[currentMapX][currentMapY]->exits[6];
        exitsCoordinates[4] = tempMapX;
    }

    //Right exit of current map == left exit of right map
    currentMapY = world->currentMapYCoor;
    currentMapY++;
    if(outofMapBoundsChecker(currentMapY) == 0 && (world->maps[currentMapX][currentMapY]!= NULL)){
         terrainGrid[exitsCoordinates[6]][exitsCoordinates[7]] = '%';
        tempMapX = world->maps[currentMapX][currentMapY]->exits[4];
        exitsCoordinates[6] = tempMapX;
    }

}

/*
Seal exit(s) of a map depending on location of map in world
Maps at edges of world will have sealed exits
*/
void sealExits(World *world, char terrainGrid[21][80]){

    int currentMapX = world->currentMapXCoor;
    int currentMapY = world->currentMapYCoor;

    if(currentMapX == 0){
        for(int i=0; i<80; i++){
            terrainGrid[0][i] = '%';
        }
    }

    if(currentMapX == 400){
         for(int i=0; i<80; i++){
            terrainGrid[20][i] = '%';
        }
    }

    if(currentMapY == 0){
         for(int i=0; i<21; i++){
            terrainGrid[i][0] = '%';
        }
    }

    if(currentMapY == 400){
         for(int i=0; i<21; i++){
            terrainGrid[i][79] = '%';
        }
    }
}

/*----------------------Handler methods for refactored code of main----------------------------------*/

void npcHeapAndNodesHandler(Map *map);

/*Method to intialize heap, assign npc property to nodes , and add to moveOrder heap on given Map*/
void npcHeapAndNodesHandler(Map *map){
    // initialize all initial node cost to 0
    map->moveOrderHeap = init_minheap(map->trainerCountOnMap);
    map->npcNodes = (Node *) malloc ((map->trainerCountOnMap) * sizeof(Node));
}




