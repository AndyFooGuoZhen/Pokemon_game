
 #define ESC 27
 #include "worldAndMap.h"

/*
Used for representing items in a bag
*/
class Item{
    public:
    string itemName;
    int qty;

    Item(string givenItemName, int givenQty){
        itemName = givenItemName;
        qty = givenQty;
    }
};

/*
Used to represent specific pokeballs
*/
class PokeBall{
    public: 
    string ballName;
    int qty;

    PokeBall(string givenBallName, int givenQty){
        ballName = givenBallName;
        qty = givenQty;        
    }
};

 class Pc{
    public:
    int currentXPos;
    int currentYPos;
    vector<Pokemon> pokemonByPC;
    vector<Item> items;
    vector<PokeBall> pokeBalls; //0 :Normal 1: Great 2:Ultra 3:Master
    int pokeBucks = 50;
};

/*
Captured pokemons stored into pokedex,
Allows PC to swap pokemon, make team compositions in PokeCenters
*/
vector<Pokemon> universalPokedex; 

void updatePcCoordinates( Pc *player, int xCoor, int yCoor);
void moveToCoor(int direction, int xCoor, int yCoor, int nextMoveCoor[2]);
char keyHandlers(char keyPressed,  Pc *player, World *world,char overlayGrid[21][80], Npc **npc, int trainerCount);
char validateMovementOnMap(Pc *player,int tempNextMoveCoor[2], World *world, char overlayGrid[21][80]);
char validateEntryToExits(World *world, Pc *player, int tempNextMoveCoor[2]);
char returnMapExitDirection(int i, World *world , Pc *player);
char enterMartOrCenterChecker(Pc *player, char **mapGrid);
void potionPurchaseHandler(Pc *player);
void revivePurchaseHandler(Pc *player);
void pokeballPurchaseHandler(Pc *player);
void martHandler(Pc *player);
void handlePokemonPokedexInterface(Pc *player);
void centerHandler(Pc *player);
char scanAndinitiateBattle(Pc *player, Map *map, char overlayGrid[21][80], int tempDetectedNPCCoor[2]);
char scanSurroundings(Pc *player, char overlayGrid[21][80], int detectedNPCCoor[2]);
void displayTrainers(Pc *player, Npc **npc, int trainerCount);
void calculateDistanceFromPC(Pc *player, int npcX, int npcY ,int distanceXY[2]);
void pcSpawnerHandler(Pc * player, char **grid);
void npcDjiktraCostHandler(Map *map, Pc * player);
void generateStartingPokemon(Pc *player);
void initializeBagItems(Pc *player);
void potionHandler(int currPokemon, Pc *player);
void reviveHandler(int currPokemon, Pc *player);
void useBagItems(Pc *player);

/*
Updates player current coordinates
*/
void updatePcCoordinates(Pc *player, int xCoor, int yCoor){
    player->currentXPos = xCoor;
    player->currentYPos = yCoor;
}

/*
Depending on key pressed , moves the pc or toggle trainer details
returns char for quititing and resting a turn
 7 : upper left
 8 : top
 9: upper right
 6: right
 3: lower right
 2: down
 1: lower left
 4: left
*/
char keyHandlers(char keyPressed, Pc *player, World *world , char overlayGrid[21][80], Npc **npc, int trainerCount){

    int tempNextMoveCoor[2];

    switch(keyPressed){
         case 'q':
            return 'q';
        
        case '5':
        case ' ':
            return '5';

        case '7':
        case 'y':
            moveToCoor(0,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            // wprintw(win, "(%d, %d)", player->currentXPos, player->currentYPos);
             break;
           
        case '8':
        case 'k':
            moveToCoor(1,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            // wprintw(win, "(%d, %d)", player->currentXPos, player->currentYPos);
            break;

        case '9':
        case 'u':
            moveToCoor(2,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            // wprintw(win, "(%d, %d)", player->currentXPos, player->currentYPos);
            break;

        case '6':
        case 'l':
             moveToCoor(3,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            //  wprintw(win, "(%d, %d)", player->currentXPos, player->currentYPos);
             break;

        case '3':
        case 'n':
            moveToCoor(4,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            wprintw(win, "(%d, %d)", player->currentXPos, player->currentYPos);
            break;        

        case '2':
        case 'j':
             moveToCoor(5,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            //   wprintw(win,"(%d, %d)", player->currentXPos, player->currentYPos);
            break;  

        case '1':
             moveToCoor(6,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            //   wprintw(win, "(%d, %d)", player->currentXPos, player->currentYPos);
             break;

        case '4':
        case 'h':
            moveToCoor(7,player->currentXPos, player->currentYPos, tempNextMoveCoor);
            // wprintw(win, "(%d, %d)", player->currentXPos, player->currentYPos);
            break;
        
        case 't':
            displayTrainers(player, npc, trainerCount);
            return 'J';

        case 'b':
            useBagItems(player);
            return 'J';

        case '>':
            return  enterMartOrCenterChecker(player, world->maps[world->currentMapXCoor][world->currentMapYCoor]->grid);
            
        default:
            return '-';
    }

     return validateMovementOnMap(player, tempNextMoveCoor, world, overlayGrid);
}

/*
Checks if next pc movement is allowed,
if allowed, updates pc's coordinates and returns T
if exits are crossed , updates player's current map coordinate and world coordinate
N : goes to top map
E : goes to right map
S : goes to bottom
W : goes to left
else, returns F
*/
char validateMovementOnMap(Pc *player,int tempNextMoveCoor[2], World *world, char overlayGrid[21][80]){

    int xCoor = tempNextMoveCoor[0];
    int yCoor = tempNextMoveCoor[1];

    //Check for boundary, dont allow pc to move to edge
    //Unless PC moves to next map through exits
    if(xCoor == 0 || xCoor == 20 || yCoor == 0 || yCoor == 79){
        return validateEntryToExits(world, player, tempNextMoveCoor);
    } 

    //Check for npc, if e, pc can move
    else if(overlayGrid[xCoor][yCoor] == 'e'){
        updatePcCoordinates(player, xCoor, yCoor);
        return 'T';
    }

    return 'F';
}

/*
Helper method to check it pc can enter exits 
*/
char validateEntryToExits(World *world, Pc *player, int tempNextMoveCoor[2]){

    int worldMapX = world->currentMapXCoor;
    int worldMapY = world->currentMapYCoor;
    int xCoorOnMap = tempNextMoveCoor[0];
    int yCoorOnMap = tempNextMoveCoor[1];

    for(int i = 0; i<4; i++){
        
        //if next move reaches exit coordinate of current map, return a map direction (n, s, w, e)
        if(xCoorOnMap == world->maps[worldMapX][worldMapY]->exits[i * 2] &&  yCoorOnMap == world->maps[worldMapX][worldMapY]->exits[(i * 2) + 1]){
            if(world->maps[worldMapX][worldMapY]->grid[xCoorOnMap][yCoorOnMap] == '#'){
                return returnMapExitDirection(i, world, player);
            }
        }
    }

    return 'F';
}

/*
Helper method for returning direction of map from exits
- update pc coor on new map
*/
char returnMapExitDirection(int i, World *world , Pc *player){

    switch(i){
        case 0:
            // world->currentMapXCoor = world->currentMapXCoor - 1;
            player->currentXPos = 19;
            return 'n';
        
        case 1:
            // world->currentMapXCoor = world->currentMapXCoor + 1;
             player->currentXPos = 1;
            return 's';
        
        case 2:
            // world->currentMapYCoor = world->currentMapYCoor - 1;
             player->currentYPos = 78;
            return 'w';
        
        case 3:
            // world->currentMapYCoor = world->currentMapYCoor + 1;
             player->currentYPos = 1;
            return 'e';
        
        default:
            return 'F';
    }
}

/*
Show mart or center interface when entring
*/
char enterMartOrCenterChecker(Pc *player, char **mapGrid){

    int xCoor = player->currentXPos;
    int yCoor = player->currentYPos;

    if(mapGrid[xCoor][yCoor] == 'M'){
        martHandler(player);
        return 'J';
    }else if(mapGrid[xCoor][yCoor] == 'C'){
        centerHandler(player);
        return 'J';
    }

    return '-';
}

/*
Helper function to handle logic when PC enters pokemart
Replenishes all items of the player
*/
void martHandler(Pc *player){
    char userInput = '-';

    wclear(win);
    wprintw(win,"PC entered pokeMart\n\n");

    while(userInput != '<'){
         for(int i =0; i<3; i++){
        wprintw(win, "%-20sx%d\n", player->items[i].itemName.c_str(), player->items[i].qty);
        }
        wprintw(win, "\nWhich item would you like to purchase?.\n\n");
        wprintw(win, "%-13s $10   --(0)\n", "Potions");
        wprintw(win, "%-13s $20   --(1)\n", "Revives");
        wprintw(win, "%-17s   --(2)\n", "Pokeballs");
        wprintw(win, "\n\nType < to exit pokemart.\n\n");
        wprintw(win, "Enter value in brackets for items: ");
        wrefresh(win);
        userInput = wgetch(win);

        switch(userInput){
            case '0':
                potionPurchaseHandler(player);
                break;
                
            case '1':
                revivePurchaseHandler(player);
                break;

            case '2':
                pokeballPurchaseHandler(player);
                break;

            default:
                break;
        }
    }
}
/*
Handles potion transactions
*/
void potionPurchaseHandler(Pc *player){

    int potionQTY= 0;

    wclear(win);
    wprintw(win, "PokeBucks : %d\n\n", player->pokeBucks);
    wprintw(win,"How many potions ($10) would you like to purchase?\n\n");
    wprintw(win, "Enter quantity of potions to purchase and hit ENTER: ");
    wrefresh(win);
    wscanw(win,"%d", &potionQTY);

    if(potionQTY * 10 > player->pokeBucks){
        wprintw(win, "Player has insuffificient PokeBucks!!\n");
        wprintw(win, "No potions were bought.\n");
        wrefresh(win);
        usleep(3000000);
        wclear(win);
    }else{
        wprintw(win,"\nPlayer potions QTY: %d --> %d", player->items[0].qty, player->items[0].qty + potionQTY);
        wrefresh(win);
        player->items[0].qty += potionQTY;
        player->pokeBucks -= (potionQTY * 10);
        usleep(3000000);
        wclear(win);
    }
}

/*
Handles revives transactions
*/
void revivePurchaseHandler(Pc *player){

    int reviveQTY= 0;

    wclear(win);
    wprintw(win, "PokeBucks : %d\n\n", player->pokeBucks);
    wprintw(win,"How many revives ($20) would you like to purchase?\n\n");
    wprintw(win, "Enter quantity of revives to purchase and hit ENTER: ");
    wrefresh(win);
    wscanw(win,"%d", &reviveQTY);

    if(reviveQTY * 20 > player->pokeBucks){
        wprintw(win, "Player has insuffificient PokeBucks!!\n");
        wprintw(win, "No revives were bought.\n");
        wrefresh(win);
        usleep(3000000);
        wclear(win);
    }else{
        wprintw(win,"\nPlayer revives QTY: %d --> %d", player->items[1].qty, player->items[1].qty + reviveQTY);
        wrefresh(win);
        player->items[1].qty += reviveQTY;
        player->pokeBucks -= (reviveQTY * 20);
        usleep(3000000);
        wclear(win);
    }
}

/*
Handles pokeBalls transactions
*/
void pokeballPurchaseHandler(Pc *player){
    char userInput = '-';
    int pokeBallQTY = 0;
    int pricePerBall = 0;

    wclear(win);
    wprintw(win, "PokeBucks : %d\n\n", player->pokeBucks);
    wprintw(win, "Select Pokeball to buy from below.\n\n");
    wprintw(win, "%-40s ($20)  --(0)\n", "Normal PokeBall");
    wprintw(win, "%-40s ($30)  --(1)\n", "Great Ball");
    wprintw(win, "%-40s ($40)  --(2)\n", "Ultra Ball");
    wprintw(win, "%-40s ($200) --(3)\n", "Master Ball");
    wprintw(win,"\n\nWhich PokeBall would you like to buy:");
    wrefresh(win);
    userInput = wgetch(win);

    switch(userInput){
        case '0':
            pricePerBall = 20;
            break;
        case '1':
            pricePerBall = 30;
            break;
        case '2':
            pricePerBall = 40;
            break;
        case '3':
            pricePerBall = 200;
            break;
        default:
            break;
    }

    wprintw(win, "\n\nEnter quantity of PokeBalls to purchase and hit ENTER: ");
    wrefresh(win);
    wscanw(win,"%d", &pokeBallQTY);

    if(pokeBallQTY* pricePerBall > player->pokeBucks){
        wprintw(win, "Player has insuffificient PokeBucks!!\n");
        wprintw(win, "No PokeBalls were bought.\n");
        wrefresh(win);
        usleep(3000000);
        wclear(win);
    }else{
        wprintw(win,"\nPlayer pokeballs QTY: %d --> %d", player->items[2].qty, player->items[2].qty + pokeBallQTY);
        wrefresh(win);
        player->items[2].qty += pokeBallQTY;
        player->pokeBalls[userInput - '0'].qty += pokeBallQTY;
        player->pokeBucks -= (pokeBallQTY * pricePerBall);
        usleep(3000000);
        wclear(win);
    }
}

/*
Helper function to handle logic when PC enters pokecenter
*/
void centerHandler(Pc *player){
    char userInput = '-';

    while(userInput != '<'){
        wclear(win);
        wprintw(win, "PC entered pokeCenter");
        wprintw(win, "All pokemons restored to full HP.\n\n");

        //Heals all pokemon to full HP
        for(long unsigned int i=0; i<player->pokemonByPC.size(); i++){
            player->pokemonByPC[i].stat[0].value = player->pokemonByPC[i].stat[6].value;
            wprintw(win, "%-20sHP:%d\n", player->pokemonByPC[i].identifier.c_str(), player->pokemonByPC[i].stat[0].value);
        }

        wprintw(win, "\nType s to swap pokemons, change party composition");
        wprintw(win, "\nType < to exit pokecenter.");
        wrefresh(win);
        userInput = wgetch(win);

        if(userInput== 's'){
            handlePokemonPokedexInterface(player);
        }
    }
    
}

/*
Handles swapping of pokemons from pc inventory to pokedex
*/
void handlePokemonPokedexInterface(Pc *player){
    
    char userInput = '-';
    int originalPcPokemonIndex = 0;
    wclear(win);
    wprintw(win, "Pokemons in PC party shown below.\n\n");

    int scrollLength = 0;

     for(long unsigned int i =0; i<player->pokemonByPC.size(); i++){
            wprintw(win, "%-40s (LV%d) --(%d)\n", player->pokemonByPC[i].identifier.c_str(), player->pokemonByPC[i].currentLevel, i);
    }

    wprintw(win, "\nChoose pokemon to swap by entering value in brackets and hit ENTER: ");
    wrefresh(win);
    userInput = wgetch(win);
    originalPcPokemonIndex = userInput - '0';
    Pokemon pokemonToSwap = player->pokemonByPC[originalPcPokemonIndex];

    while(userInput != 27 || (userInput == 2 || userInput == 3)){
        wclear(win);
        wprintw(win,"Select pokemon to swap with. Use arrow up and arrow down to scroll.\n");
        wprintw(win,"Press ESC to quit.\n");
        wprintw(win,"NOTE: Enter - then the value in brackets to swap and hit ENTER.\n\n");

         if(userInput == 2){
            scrollLength--;
        }

        else if(userInput == 3){    
             scrollLength++;
        }

        scrollLength = MAX(scrollLength, 0);
        scrollLength = MIN(scrollLength, (int)universalPokedex.size());

        for(int i=scrollLength; i<MIN((int)universalPokedex.size(), 17 + scrollLength);  i++){
            wprintw(win, "%-40s (LV %d) --(%d)\n", universalPokedex[i].identifier.c_str(), universalPokedex[i].currentLevel, i);
        }
        wrefresh(win);
        userInput = wgetch(win);

        //Swaps pokemon upon receiving input
        if(userInput == '-'){
            int selectedPokemon = 0;
            wscanw(win, "%d", &selectedPokemon);
            Pokemon selectedPokemonFromPokedex = universalPokedex[selectedPokemon];
            player->pokemonByPC[originalPcPokemonIndex] = selectedPokemonFromPokedex;
            universalPokedex[selectedPokemon] = pokemonToSwap;
            wclear(win);
            break;
        }
    }
   
}

/*
Method for checking surroundings of pc,
if surrounding of pc is NPC, initiate batle and return T
else return F
*/
char scanAndinitiateBattle(Pc *player, Map *map, char overlayGrid[21][80], int tempDetectedNPCCoor[2]){

    if(scanSurroundings(player, overlayGrid, tempDetectedNPCCoor) == 'T'){
        wclear(win);
        printCharTerrainWithOverlay(map, overlayGrid);
        wrefresh(win);
        usleep(900000);
        wclear(win);
        wprintw(win, "Battle initiated !!");
        return 'T';
    }

    return 'F';
}

/*
Helper method to check surroundings, if NPC present
return T and coordinates of NPC, else return F
*/
char scanSurroundings(Pc *player, char overlayGrid[21][80], int detectedNPCCoor[2]){

    int xCoor = player->currentXPos;
    int yCoor = player->currentYPos;

    for(int i = xCoor -1 ; i<= xCoor + 1 ; i++){
        for(int j = yCoor -1; j <= yCoor + 1; j++){
            if(i == xCoor && j == yCoor){
                continue;
            }

            else if(overlayGrid[i][j] != 'I' && overlayGrid[i][j]!= 'e'){
                detectedNPCCoor[0] = i;
                detectedNPCCoor[1] = j;
                return 'T';
            }
        }
    }

    return 'F';
}

/*
Method for displaying trainer details on screen
*/
void displayTrainers(Pc *player, Npc **npc, int trainerCount){
    char buffer[180];
    int buffSize = 100;
    char npcType;
    char *tempString1;
    char *tempString2;
    char trainerList[trainerCount][200];
    int distanceXY[2];
    char userInput = '-';

    wclear(win);
    int k=0;

    for(int i=0; i<trainerCount;){
        if((*npc)[i + k].npcType == '-'){
            k++;
            continue;
        }

        calculateDistanceFromPC(player, (*npc)[i+k].currentXPos, (*npc)[i+k].currentYPos, distanceXY);

        if(distanceXY[0] != 0){

            if(distanceXY[0] >0){
                tempString1 =(char *) "South";
            }

            else{
                tempString1 = (char *) "North";
            }
        }

        if(distanceXY[1] != 0){
            if(distanceXY[1] > 0){
                tempString2 = (char *)"East";
            }

            else{
                tempString2 = (char *)"West";
            }
        }

        npcType = (*npc)[i + k].npcType;

        if(distanceXY[0] == 0){
             snprintf(buffer, buffSize, "%c , %d %s", npcType, abs(distanceXY[1]), tempString2);
        }

        else if(distanceXY[0] == 0){
             snprintf(buffer, buffSize, "%c , %d %s", npcType, abs(distanceXY[0]), tempString1);
        }

        else{
            snprintf(buffer, buffSize, "%c , %d %s , %d %s", npcType, abs(distanceXY[0]), tempString1, abs(distanceXY[1]), tempString2);
        }

        strcpy(trainerList[i], buffer);
        wrefresh(win);

        i++;
    }

    int scrollLength = 0;

    while(userInput != 27 || (userInput == 2 || userInput ==3)){

        if(userInput == 2){
            scrollLength--;
        }

        else if(userInput == 3){    
             scrollLength++;
        }

        scrollLength = MAX(scrollLength, 0);
        scrollLength = MIN(scrollLength, trainerCount);

        for(int i=scrollLength; i<MIN(trainerCount, 21 + scrollLength); i++){
            wprintw(win, "%s\n", trainerList[i]);
        }
        wrefresh(win);

        userInput = wgetch(win);
        wclear(win);

    }
}

/*
Helper method for calculating distance of trainer relative to PC
*/
void calculateDistanceFromPC(Pc *player, int npcX, int npcY ,int distanceXY[2]){
    distanceXY[0] = npcX - player->currentXPos;
    distanceXY[1] = npcY - player->currentYPos;
}

/*Method to spawn pc on map*/
/*Used for spawning pc on center of world and map when flying*/
void pcSpawnerHandler(Pc * player, char **grid){
    int** coorForPath;
    int pathArray = scanMapForCharacterCoordinates(grid, '#', &coorForPath);
    int coordinateForPC[2];
    randomSpawnerForSelectedSymbol(grid, '@', &coorForPath, pathArray, coordinateForPC);
    updatePcCoordinates(player, coordinateForPC[0], coordinateForPC[1]);
    free(coorForPath);
}

/*Method to compute Djiktra cost map for npc (Hiker + Rivals/Others) on given map of world*/
/*@parameter world (to change properties of world)*/
void npcDjiktraCostHandler(Map *map){
    //Djikstra algorithm for shortest paths (Hiker)
    map->hikerHeap = init_minheap(1680);
    buildDjiktraCostMap(map->hikerHeap, map->grid, map->hikerNodes, map->mapPlayerX, map->mapPlayerY, 'H');

    //Djikstra algorithm for shortest paths (Rival)
    map->rivalHeap = init_minheap(1680);
    buildDjiktraCostMap(map->rivalHeap, map->grid, map->rivalNodes, map->mapPlayerX, map->mapPlayerY, 'R');
}

/*
Generate pokemon and allows PC to choose 1 of 3 pokemons 
at the beginning of the game
*/
void generateStartingPokemon(Pc *player){

    int input;
    vector<Pokemon>tempPokemonChoices;

    //Randomly generates 3 pokemons
    for(int i=0; i<3; i++){
        tempPokemonChoices.push_back(initiateSpawnPokemonSequence(200,200));
    }

    for(int i=0; i<3; i++){
        wprintw(win,"%s (LV%d) (HP:%d) ---- (%d)\n", tempPokemonChoices[i].identifier.c_str(),tempPokemonChoices[i].currentLevel,tempPokemonChoices[i].stat[0].value, i);
    }

    wprintw(win,"\n\nSelect a pokemon to begin with (Type 0, 1 , or 2 and hit ENTER):");
    wrefresh(win);

    wscanw(win,"%d", &input);
    wrefresh(win);

    //Adds pokemon to PC's pokemon vector
    player->pokemonByPC.push_back(tempPokemonChoices[input]);
}

/*
Initializes bag for PC at start of game
- potions, reviveves and pokeballs
*/
void initializeBagItems(Pc *player){
    Item potions("Potions", 3);
    Item revives("Revives", 3);
    Item pokeballs("PokeBall", 11);
    player->items.push_back(potions);
    player->items.push_back(revives);
    player->items.push_back(pokeballs);

    PokeBall normalBall("Normal PokeBall", 5);
    PokeBall greatBall("Great PokeBall", 3);
    PokeBall ultraBall("Ultra PokeBall", 2);
    PokeBall masterBall("Master PokeBall", 1); 
    player->pokeBalls.push_back(normalBall);
    player->pokeBalls.push_back(greatBall);
    player->pokeBalls.push_back(ultraBall);
    player->pokeBalls.push_back(masterBall);
}


/**Code for handling the use of items outside of battles, methods reused in actual battles as well*/

/*
Helper method for potion application
-Knockedout pokemon cant use potions
- cant heal more than max hp
*/
void potionHandler(int currPokemon, Pc *player){
    wclear(win);

    if(player->pokemonByPC[currPokemon].stat[0].value == 0){
        wprintw(win,"Pokemon is knocked out, cant use potion on this pokemon");
    }

    else if(player->pokemonByPC[currPokemon].stat[0].value == player->pokemonByPC[currPokemon].stat[6].value){
        wprintw(win,"Pokemon hp is full. Cant use potion.");
    }

    else if(player->items[0].qty == 0){
        wprintw(win,"No potions left.");
    }

    else{
        int oldHp = player->pokemonByPC[currPokemon].stat[0].value;
        int maxHP = player->pokemonByPC[currPokemon].stat[6].value;
        player->pokemonByPC[currPokemon].stat[0].value = MIN(oldHp + 20, maxHP);
        wprintw(win, "%s hp  has gone from %d --> %d \n", player->pokemonByPC[currPokemon].identifier.c_str(), oldHp, player->pokemonByPC[currPokemon].stat[0].value);
        player->items[0].qty--;
    }

    wrefresh(win);
    usleep(2000000);
}

/*
Helper method for revive application
- Can only be used on knocked out pokemon
*/
void reviveHandler(int currPokemon, Pc *player){
    wclear(win);

    if(player->pokemonByPC[currPokemon].stat[0].value > 0){
        wprintw(win,"Pokemon is not knocked out, cant use revive on this pokemon");
    }

    else if(player->items[0].qty == 0){
        wprintw(win,"No revives left.");
    }

    else{
        int oldHp = player->pokemonByPC[currPokemon].stat[0].value;
        int reviveHP = player->pokemonByPC[currPokemon].stat[6].value / 2;
        player->pokemonByPC[currPokemon].stat[0].value = reviveHP;
        wprintw(win, "%s hp  has gone from %d --> %d \n", player->pokemonByPC[currPokemon].identifier.c_str(), oldHp, player->pokemonByPC[currPokemon].stat[0].value);
        player->items[1].qty--;
    }

    wrefresh(win);
    usleep(2000000);
}

/*
Display bag items, allow pc to heal/revive pokemon 
out of battles
*/
void useBagItems(Pc *player){
    char userInput = '-';

    while(userInput != 'b'){
        wclear(win);

        wprintw(win, "PokeBucks : %d\n\n", player->pokeBucks);

        //Display potions and revives
        for(int i=0; i<2; i++){
            wprintw(win, "%-40sx%d---(%d)\n", player->items[i].itemName.c_str(), player->items[i].qty, i);
        }
        wprintw(win, "\n\nPress b again to exit.");
        wprintw(win, "\n\nEnter value in brackets for items: ");
        wrefresh(win);
        userInput = wgetch(win);

        if(userInput != 'b'){
            wclear(win);

            for(long unsigned int i =0; i<player->pokemonByPC.size(); i++){
                wprintw(win, "%-15s HP:%d -- (%d) \n", player->pokemonByPC[i].identifier.c_str(), player->pokemonByPC[i].stat[0].value, i);
            }

            switch(userInput){
                case '0':
                    wprintw(win, "\nSelect pokemon to use potion on: ");
                    userInput = wgetch(win);
                    potionHandler(userInput - '0', player);
                    break;

                case '1':
                    wprintw(win, "\nSelect pokemon to revive:  ");
                    userInput = wgetch(win);
                    reviveHandler(userInput- '0', player);
                    break;

                default:
                    break;  
            }
        }
    }
    
}