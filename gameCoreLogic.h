#include "pokeBattle.h"
void recomputeAndUpdateNPCPath(World *w1, Pc *player);
void moveAllNPC(World *w1, Node &currMinNode, Pc *player);
char flyToMapHandler(World *w1, Pc *player, int argc, char* argv[], Node &currMinNode, int &trainerCountOriginal ,int &currMapX, int &currMapY);
void exitsHandler(World *w1, Pc *player,int argc, char* argv[], Node &currMinNode, char &keyCommand, int &currMapX, int &currMapY, int trainerCountOriginal);
void trainerMartCenterHandler(World *w1, Pc *player, char &keyCommand, int currMapX, int currMapY);
void gameSetup(World *w1, Pc *player, int argc, char* argv[], int &trainerCountOriginal);
void gameStart(World *w1, Pc *player, int argc, char* argv[], int &trainerCountOriginal);

//Window setup, world setup, initial map, pc initialization 
void gameSetup(World *w1, Pc *player, int argc, char* argv[], int &trainerCountOriginal){

    //Prepares pokemon data
    initializePokemonData();

    //Initializes the world, and initializes map in the middle
    initializeWorldMaps(w1);
    initializeCurrentMapAtCoordinate(w1, 200, 200);
    assignMapCoor(w1->maps[200][200], 200, 200);
    generateRandomMap(w1->maps[200][200], w1, 0);

    //Spawns PC randomly on path, updates PC coordinates on map
    pcSpawnerHandler(player, w1->maps[200][200]->grid);
    w1->maps[200][200]->mapPlayerX = player->currentXPos;
    w1->maps[200][200]->mapPlayerY = player->currentYPos;

    //Compute Djiktra Cost for NPCs
    npcDjiktraCostHandler(w1->maps[200][200]);

    //Place PC on overlay
    initializeOverlayGrid(player->currentXPos, player->currentYPos);

     //Allow PC to choose 1 out of 3 pokemons
    generateStartingPokemon(player);

    //Iniailize bag items for PC
    initializeBagItems(player);

    //Accept inputs and creates number of Npc based on that input, also spawns NPC on overlayGrid
    trainerCountOriginal = initHikerAndRivalCount(argc, argv, &(w1->maps[200][200]->npcArray), w1->maps[200][200]->hikerNodes, w1->maps[200][200]->rivalNodes, w1->maps[200][200]->grid);
    w1->maps[200][200]->trainerCountOnMap = trainerCountOriginal;
   
    //Handles npc node initialization (malloc space for no.trainers for heap) 
    npcHeapAndNodesHandler(w1->maps[200][200]);
    wclear(win);
    wprintw(win, "Map coor: (%d, %d)\n", w1->currentMapYCoor - 200, (w1->currentMapXCoor - 200)/(-1));
    printCharTerrainWithOverlay(w1->maps[200][200], overlayGrid);
    
    //assign properties to nodes and add to min heap
    initializeNPCNodes(&(w1->maps[200][200]->npcNodes),&(w1->maps[200][200]->npcArray),(w1->maps[200][200]->moveOrderHeap), w1->maps[200][200]->trainerCountOnMap); 

    //Assign random pokemons to NPC's
    generatePokemonForNPC(&(w1->maps[200][200]->npcArray), trainerCountOriginal, 200, 200); 
 
}

void gameStart(World *w1, Pc *player, int argc, char* argv[], int &trainerCountOriginal){
    char keyCommand= '-';
    int tempDetectedNPCCoor[2]; 

    //Current map location on World
    int currMapX = 200;
    int currMapY = 200;   

    //Current MinNode is shared across all maps
    Node currMinNode;

    while(keyCommand != 'q'){
    
        //Recomputes shortest paths for hiker and rival if PC moves
        if(keyCommand == 'T'){
            recomputeAndUpdateNPCPath(w1, player);
        }

        //Simulating and moves all npc in one step, and prints map
        moveAllNPC(w1, currMinNode, player);

        //If Surroundings of PC has NPC, enter battle sequence
        //Removes defeated npc from heap , npcArray and overlaymap
        if(scanAndinitiateBattle(player, w1->maps[currMapX][currMapY], overlayGrid, tempDetectedNPCCoor) == 'T'){
            insert_minheap(w1->maps[currMapX][currMapY]->moveOrderHeap, currMinNode); //insert back min node (min node would be the NPC encountered by PC)
            
            //PokemonBattle interface with NPC
            npcPokemonBattleHandler(w1->maps[currMapX][currMapY]->npcArray[currMinNode.index],player);

            //Removes NPC from heap, (DEFEATED NPC)
            w1->maps[currMapX][currMapY]->trainerCountOnMap = removeNPC(w1->maps[currMapX][currMapY]->moveOrderHeap, tempDetectedNPCCoor[0], tempDetectedNPCCoor[1], &(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->trainerCountOnMap);
            continue;
        }

        //Spawn Pokemons if PC move on grass and 10% chance is met
        if(toSpawnPokemon(w1->maps[currMapX][currMapY]->grid, player->currentXPos, player->currentYPos) == 'T'){
            Pokemon spawnedPokemon = initiateSpawnPokemonSequence(currMapX, currMapY);
            pokemonBattleHandler(spawnedPokemon, player);
             wclear(win);
             wprintw(win, "Map coor: (%d, %d)\n", w1->currentMapYCoor - 200, (w1->currentMapXCoor - 200)/(-1));
             printCharTerrainWithOverlay(w1->maps[currMapX][currMapY], overlayGrid);
             wrefresh(win);
        }

        //can add encouter pokemon here
        keyCommand = wgetch(win);

        //If flying command is detected, initiate flying sequence
        if(keyCommand == 'f'){
            keyCommand =  flyToMapHandler(w1, player, argc, argv, currMinNode, trainerCountOriginal, currMapX, currMapY);
        }
        
        keyCommand = keyHandlers(keyCommand, player, w1, overlayGrid, &(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->trainerCountOnMap);
            
        //If exits of maps sucessfully entered, initiate traverse exits sequence
        if(keyCommand == 'n' || keyCommand == 'e' || keyCommand == 's'|| keyCommand == 'w' ){
            exitsHandler(w1,player,argc,argv,currMinNode, keyCommand, currMapX, currMapY, trainerCountOriginal);
        }

        //If user wants to check traier details or enter mart/center
        if(keyCommand == 'J' || keyCommand == 't' || keyCommand == '>' || keyCommand == 'b'){
            trainerMartCenterHandler(w1, player, keyCommand,currMapX, currMapY);
        }
    }
}
// /*--------------------------------------------------------------CORE COMPONENTS OF GAME------------------------------------------------------------------------------------*/

//Recompute and update path for npc after PC moves/ NPC is removed
void recomputeAndUpdateNPCPath(World *w1, Pc *player){
    int currMapX = w1->currentMapXCoor;
    int currMapY = w1->currentMapYCoor;
    w1->maps[currMapX][currMapY]->mapPlayerX = player->currentXPos;
    w1->maps[currMapX][currMapY]->mapPlayerY = player->currentYPos;
    npcDjiktraCostHandler(w1->maps[currMapX][currMapY]);
    initializeOverlayGrid(player->currentXPos, player->currentYPos);
    updateOverlayGrid(&(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->hikerNodes, w1->maps[currMapX][currMapY]->rivalNodes, w1->maps[currMapX][currMapY]->trainerCountOnMap);
}

//All NPC move in single simulation, and prints map
void moveAllNPC(World *w1, Node &currMinNode, Pc *player){
    int counter = 0;
    int currMapX = w1->currentMapXCoor;
    int currMapY = w1->currentMapYCoor;

    while(counter < w1->maps[currMapX][currMapY]->trainerCountOnMap){
            //Insert updated node back into npc min heap
            if(w1->maps[currMapX][currMapY]->moveOrderHeap->size != w1->maps[currMapX][currMapY]->trainerCountOnMap){
                insert_minheap(w1->maps[currMapX][currMapY]->moveOrderHeap, currMinNode);
            }

            //extract min from heap
            currMinNode = get_min(w1->maps[currMapX][currMapY]->moveOrderHeap);
            delete_minimum(w1->maps[currMapX][currMapY]->moveOrderHeap);

            //depending on node type, move according to algorithm/pattern defined
            moveAndUpdateNpc(&currMinNode, w1->maps[currMapX][currMapY]->hikerNodes, w1->maps[currMapX][currMapY]->rivalNodes, &(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->moveOrderHeap, w1->maps[currMapX][currMapY]->grid);

            //redraw overlay
            initializeOverlayGrid(player->currentXPos, player->currentYPos);
            updateOverlayGrid(&(w1->maps[currMapX][currMapY]->npcArray),w1->maps[currMapX][currMapY]->hikerNodes, w1->maps[currMapX][currMapY]->rivalNodes, w1->maps[currMapX][currMapY]->trainerCountOnMap);
            wrefresh(win);
            counter++;
        }

    wclear(win);
    // printHeapAndNpcArrayDetails(&(w1.maps[currMapX][currMapY]->npcArray), w1.maps[currMapX][currMapY]->moveOrderHeap,w1.maps[currMapX][currMapY]->trainerCountOnMap);
    wprintw(win, "Map coor: (%d, %d)\n", w1->currentMapYCoor - 200, (w1->currentMapXCoor - 200)/(-1));
    printCharTerrainWithOverlay(w1->maps[currMapX][currMapY], overlayGrid);
}

//Handles logic for flying to other maps
char flyToMapHandler(World *w1, Pc *player, int argc, char* argv[], Node &currMinNode, int &trainerCountOriginal,int &currMapX, int &currMapY){
    char keyCommand = 'f';

    while(keyCommand == 'f'){
        //update currentmapcoordinate
        insert_minheap(w1->maps[currMapX][currMapY]->moveOrderHeap, currMinNode); //insert back min node
        char toInitPCandNPC = moveMapByMap(w1, keyCommand);

        //Update current map coordinate on world
        currMapX = w1->currentMapXCoor;
        currMapY = w1->currentMapYCoor;
        
        //If map has not been initialized, generate random pc and npc location on new map
        if(toInitPCandNPC == 'T'){
            //update trainercount
            w1->maps[currMapX][currMapY]->trainerCountOnMap = trainerCountOriginal;

            //randomly spawn pc on path
            pcSpawnerHandler(player, w1->maps[currMapX][currMapY]->grid);
            w1->maps[currMapX][currMapY]->mapPlayerX = player->currentXPos;
            w1->maps[currMapX][currMapY]->mapPlayerY = player->currentYPos;
            initializeOverlayGrid(player->currentXPos, player->currentYPos);

            //Randomly spawn npc and setups moveorder node for npc
            npcDjiktraCostHandler(w1->maps[currMapX][currMapY]);
            initHikerAndRivalCount(argc, argv, &(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->hikerNodes, w1->maps[currMapX][currMapY]->rivalNodes, w1->maps[currMapX][currMapY]->grid);
            npcHeapAndNodesHandler(w1->maps[currMapX][currMapY]);
            initializeNPCNodes(&(w1->maps[currMapX][currMapY]->npcNodes),&(w1->maps[currMapX][currMapY]->npcArray),(w1->maps[currMapX][currMapY]->moveOrderHeap), w1->maps[currMapX][currMapY]->trainerCountOnMap);

            //Generate pokemons for NPC's
            generatePokemonForNPC(&(w1->maps[currMapX][currMapY]->npcArray), trainerCountOriginal, currMapX, currMapY); 
        }

        //Else use original position for pc and npc on visited map
        else{
            //update back to original pc and npc position 
            player->currentXPos = w1->maps[currMapX][currMapY]->mapPlayerX;
            player->currentYPos = w1->maps[currMapX][currMapY]->mapPlayerY;
            initializeOverlayGrid(player->currentXPos, player->currentYPos);
            updateOverlayGrid(&(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->hikerNodes, w1->maps[currMapX][currMapY]->rivalNodes, w1->maps[currMapX][currMapY]->trainerCountOnMap);
        }  
            // printHeapAndNpcArrayDetails(&(w1.maps[currMapX][currMapY]->npcArray), w1.maps[currMapX][currMapY]->moveOrderHeap,w1.maps[currMapX][currMapY]->trainerCountOnMap);
            wclear(win);
            wprintw(win, "Map coor: (%d, %d)\n", w1->currentMapYCoor - 200, (w1->currentMapXCoor - 200)/(-1));
            printCharTerrainWithOverlay(w1->maps[currMapX][currMapY], overlayGrid);
            wrefresh(win);
            keyCommand = wgetch(win);
    }

    return keyCommand;
}

//Handles logic for going through exits
void exitsHandler(World *w1, Pc *player,int argc, char* argv[], Node &currMinNode, char &keyCommand, int &currMapX, int &currMapY, int trainerCountOriginal){

     while(keyCommand == 'n' || keyCommand == 'e' || keyCommand == 's'|| keyCommand == 'w'){
            insert_minheap(w1->maps[currMapX][currMapY]->moveOrderHeap, currMinNode); //insert back min node
            //move to new map, update global world current X and Y
            char toInitNPC = moveMapByMap(w1, keyCommand);
            currMapX = w1->currentMapXCoor;
            currMapY = w1->currentMapYCoor;

            //update map's pc coor
            w1->maps[currMapX][currMapY]->mapPlayerX = player->currentXPos;
            w1->maps[currMapX][currMapY]->mapPlayerY = player->currentYPos;
            initializeOverlayGrid(player->currentXPos, player->currentYPos);

            if(toInitNPC == 'T'){
                w1->maps[currMapX][currMapY]->trainerCountOnMap = trainerCountOriginal;
                npcDjiktraCostHandler(w1->maps[currMapX][currMapY]);
                initHikerAndRivalCount(argc, argv, &(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->hikerNodes, w1->maps[currMapX][currMapY]->rivalNodes, w1->maps[currMapX][currMapY]->grid);
                npcHeapAndNodesHandler(w1->maps[currMapX][currMapY]);
                initializeNPCNodes(&(w1->maps[currMapX][currMapY]->npcNodes),&(w1->maps[currMapX][currMapY]->npcArray),(w1->maps[currMapX][currMapY]->moveOrderHeap), w1->maps[currMapX][currMapY]->trainerCountOnMap);

                //Generate pokemons for NPC's
                generatePokemonForNPC(&(w1->maps[currMapX][currMapY]->npcArray), trainerCountOriginal, currMapX, currMapY); 
            }
            else if(toInitNPC == 'F'){
                updateOverlayGrid(&(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->hikerNodes, w1->maps[currMapX][currMapY]->rivalNodes, w1->maps[currMapX][currMapY]->trainerCountOnMap);
            }
                wclear(win);
                // printHeapAndNpcArrayDetails(&(w1.maps[currMapX][currMapY]->npcArray), w1.maps[currMapX][currMapY]->moveOrderHeap,w1.maps[currMapX][currMapY]->trainerCountOnMap);
                wprintw(win, "Map coor: (%d, %d)\n", w1->currentMapYCoor - 200, (w1->currentMapXCoor - 200)/(-1));
                printCharTerrainWithOverlay(w1->maps[currMapX][currMapY], overlayGrid);
                wrefresh(win);
                keyCommand = wgetch(win);
                keyCommand = keyHandlers(keyCommand, player, w1, overlayGrid, &(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->trainerCountOnMap);
        }
    
}

//Handles logic for accessing trainer tab, mart or center
void trainerMartCenterHandler(World *w1, Pc *player, char &keyCommand, int currMapX, int currMapY){

    while(keyCommand == 'J' || keyCommand == 't' || keyCommand == '>' || keyCommand == 'b'){
        keyCommand = keyHandlers(keyCommand, player, w1, overlayGrid, &(w1->maps[currMapX][currMapY]->npcArray), w1->maps[currMapX][currMapY]->trainerCountOnMap);
        wclear(win);
        // printHeapAndNpcArrayDetails(&(w1.maps[currMapX][currMapY]->npcArray), w1.maps[currMapX][currMapY]->moveOrderHeap,w1.maps[currMapX][currMapY]->trainerCountOnMap);
        wprintw(win, "Map coor: (%d, %d)\n", w1->currentMapYCoor - 200, (w1->currentMapXCoor - 200)/(-1));
        printCharTerrainWithOverlay(w1->maps[currMapX][currMapY], overlayGrid);
        wrefresh(win);
        keyCommand = wgetch(win);
    }
}


   
