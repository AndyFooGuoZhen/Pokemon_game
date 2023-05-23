#include "pc.h"
void pokemonBattleHandler(Pokemon &spawnedPokemon, Pc *player);
int pokemonDamageCompute(Pokemon &atkPokemon, Pokemon &defPokemon, int movesSelected);
char capturePokemon(Pokemon &spawnedPokemon, Pc *player);
char bagHandler(Pokemon &spawnedPokemon, int currPokemon, Pc *player, char isWildBattle);
char escapeHandler(Pokemon &spawnedPokemon,Pokemon pcPokemon, int &attempts);
void switchPokemonHandler(Pc *player, int &currPokemon);
char captureHandler(Pokemon &spawnedPokemon, Pc *player);
char battleStatus(Pokemon &spawnedPokemon, Pc *player);
void pokemonAtkMovesOrderHandler(int pcAtkDamage, int enemyAtkDamage, char firstPokemonToAtk, Pokemon &enemyPokemon, Pokemon &playerPokemon, int pcChosenMove, int randomEnemyMove);
void printKnockOutDetails(Pokemon atkPokemon, Pokemon knockedPokemon);
void pokemonMovesBattleHandler(Pokemon &enemyPokemon, Pc *player, int &currPokemon);
char npcBattleStatus(Pokemon &spawnedPokemon, Pc *player);
char battleInterface(Pokemon &spawnedPokemon, Pc *player, int &attempts, int &currPokemon);
char npcBattleInterface(Npc &npc,Pc *player, int &pcCurrPokemon, int &npcCurrPokemon);
void npcPokemonBattleHandler(Npc &npc, Pc *player);


/*
Handles pokemon battle prompt, print stats and moves
Pokemon battle between PC and wild pokemon
FUTURE NOTE: Capture pokemon here (add vector of PC here)
*/
void pokemonBattleHandler(Pokemon &spawnedPokemon, Pc *player){
    char pokemonBattleStatus = 'T';
    int attempts = 1;
    int currPokemon = 0;

    wclear(win);
    wprintw(win,"Wild %s (LV %d) appeared!\n", spawnedPokemon.identifier.c_str(), spawnedPokemon.currentLevel);
    wprintw(win,"Pokemon Battle begins!\n\n", spawnedPokemon.identifier.c_str(), spawnedPokemon.currentLevel);
    usleep(400000);
    wrefresh(win);

    //While pokeomBatlle is ongoing
    while(pokemonBattleStatus == 'T'){
        wclear(win);
        pokemonBattleStatus = battleInterface(spawnedPokemon, player, attempts, currPokemon);
        wrefresh(win);
    }

}


/*
Computes attack damage of a pokemon
Formula: (((2 x level/5) + 2 * Power * atk/def)/100 + 2) * Cric * random * STAB 
*/
int pokemonDamageCompute(Pokemon &atkPokemon, Pokemon &defPokemon, int movesSelected){
    double levelPortion = ((2 * atkPokemon.currentLevel)/5) + 2;
    double atkOverDefense = (atkPokemon.stat[1].value)/(defPokemon.stat[2].value);
    double power = atkPokemon.learnedMoves[movesSelected].power;
    double random = rand()%16 + 85;
    int pokemonType = 0;
    
    //Retrieves pokemon type from  to compute STAB
    for(long unsigned int i=0; i< pokemonTypeList.size(); i++){
        if(atkPokemon.id == pokemonTypeList[i].pokemon_id){
            pokemonType = pokemonTypeList[i].type_id;
            break;
        }
    }

    double STAB = atkPokemon.learnedMoves[movesSelected].type_id == pokemonType ? 1.5 : 1.0;

    //Compute Critical
    double Critical = rand()%256 < floor(atkPokemon.stat[3].value/2) ? 1.5: 1.0;

    int totalDamage = (int)((((levelPortion * power * atkOverDefense)/100.0)+2) * Critical * random * STAB);

    return totalDamage;
}

/*
Helper method for capturing pokemon
Returns F - end pokemon battle with capture or fleed pokemon
*/
char capturePokemon(Pokemon &spawnedPokemon, Pc *player){
    
    if(player->pokemonByPC.size()<6){
        wclear(win);
        wprintw(win, "%s has been captured", spawnedPokemon.identifier.c_str());
        wrefresh(win);
        player->pokemonByPC.push_back(spawnedPokemon);
    }

    else{
        wprintw(win, "Pc has too many pokemons, fail to capture %s", spawnedPokemon.identifier.c_str());
    }

    return 'F';
}

char battleInterface(Pokemon &spawnedPokemon, Pc *player, int &attempts, int &currPokemon){

    char userInput = '-';

    //Prints interface
    wprintw(win,"%-40s%s\n", "PC", "Wild Pokemon");
    wprintw(win,"-----------------------------------------------------------\n");
    wprintw(win, "%-40s%s\n", player->pokemonByPC[currPokemon].identifier.c_str(), spawnedPokemon.identifier.c_str());
    wprintw(win, "LV %-37dLV %d\n", player->pokemonByPC[currPokemon].currentLevel, spawnedPokemon.currentLevel);
    wprintw(win, "HP:%-37dHP:%d\n\n\n", player->pokemonByPC[currPokemon].stat[0].value, spawnedPokemon.stat[0].value);
    wprintw(win, "%-40s%s\n\n", "Fight (f)","Bag (b)");
    wprintw(win,"%-40s%s\n\n\n", "Run (r)","Switch Pokemon (s)");
    wprintw(win, "Enter value in brackets for actions: ");
    userInput = wgetch(win);

    switch(userInput){

        case 'f':
            pokemonMovesBattleHandler(spawnedPokemon, player, currPokemon);
            break;
        
        case 'b':
            return bagHandler(spawnedPokemon, currPokemon, player, 'T');
        
        case 'r':
            return escapeHandler(spawnedPokemon, player->pokemonByPC[currPokemon], attempts);
        
        case 's':
            switchPokemonHandler(player, currPokemon);
            break;
        
        default:
            break;

    }

    return battleStatus(spawnedPokemon, player);
}

/*
Use pokemon moves to battle
*/
void pokemonMovesBattleHandler(Pokemon &enemyPokemon, Pc *player, int &currPokemon){

    // p if PC first, e if enemy first
    char firstPokemonToAtk= 'p';

    wclear(win);
    wprintw(win, "Select move to use for attacking by entering value in brackets\n\n");
    for(long unsigned int i =0; i<player->pokemonByPC[currPokemon].learnedMoves.size(); i++){
        wprintw(win, "%-20s --(%d)\n\n", player->pokemonByPC[currPokemon].learnedMoves[i].identifier.c_str(), i);
    }

    char userInput = wgetch(win);
    int pcChosenMove = userInput - '0';

    //Compute atk damage for enemy pokemon
    int randomEnemyMove = rand()%2;
    int enemyAtkDamage = pokemonDamageCompute(enemyPokemon, player->pokemonByPC[currPokemon], randomEnemyMove);

    //Compute atk damage for PC's pokemon
    int pcAtkDamage = pokemonDamageCompute(player->pokemonByPC[currPokemon], enemyPokemon, pcChosenMove);

    //Compare moves priority
    if(enemyPokemon.learnedMoves[randomEnemyMove].priority > player->pokemonByPC[currPokemon].learnedMoves[pcChosenMove].priority){
        firstPokemonToAtk = 'e';
    }

    else if(enemyPokemon.learnedMoves[randomEnemyMove].priority < player->pokemonByPC[currPokemon].learnedMoves[pcChosenMove].priority){
        firstPokemonToAtk = 'p';
    }

    //Compare speed if priority are the same
    else if(enemyPokemon.stat[1].value > player->pokemonByPC[currPokemon].stat[1].value){
        firstPokemonToAtk = 'e';
    }

    else if(enemyPokemon.stat[1].value < player->pokemonByPC[currPokemon].stat[1].value){
        firstPokemonToAtk = 'p';
    }

    //if priority and speed are the same
    else if(rand()%2!=0){
        firstPokemonToAtk = 'e';
    }
    
   pokemonAtkMovesOrderHandler(pcAtkDamage, enemyAtkDamage, firstPokemonToAtk, enemyPokemon, player->pokemonByPC[currPokemon], pcChosenMove, randomEnemyMove);
}

/*
Handles pokemon atk evasion , checks if 
enemy pokemon is not knoked out before attacking
*/
void pokemonAtkMovesOrderHandler(int pcAtkDamage, int enemyAtkDamage, char firstPokemonToAtk, Pokemon &enemyPokemon, Pokemon &playerPokemon, int pcChosenMove, int randomEnemyMove){

    wclear(win);
    int evasionChance = 0;
    int evasionChance2 = 0;
    
    if(firstPokemonToAtk == 'p'){
        evasionChance = rand()%100;

        if(evasionChance < playerPokemon.learnedMoves[pcChosenMove].accuracy && enemyPokemon.stat[0].value!=0){
           wprintw(win,"PC pokemon : %s uses %s!\n", playerPokemon.identifier.c_str(), playerPokemon.learnedMoves[pcChosenMove].identifier.c_str());
           wrefresh(win);
           enemyPokemon.stat[0].value = MAX(0, enemyPokemon.stat[0].value - pcAtkDamage);
           printKnockOutDetails(playerPokemon, enemyPokemon);
        }

        else if(evasionChance >= playerPokemon.learnedMoves[pcChosenMove].accuracy){
            wprintw(win, "Enemy pokemon evaded the attack!\n\n");
            wrefresh(win);
            usleep(4000000);
        }

        evasionChance2 = rand()%100;

        if(enemyPokemon.stat[0].value != 0 && evasionChance2 < enemyPokemon.learnedMoves[randomEnemyMove].accuracy && playerPokemon.stat[0].value!=0){
            wclear(win);
            wprintw(win,"Enemy pokemon : %s uses %s!\n", enemyPokemon.identifier.c_str(), enemyPokemon.learnedMoves[randomEnemyMove].identifier.c_str());
            wrefresh(win);
            playerPokemon.stat[0].value = MAX(0, playerPokemon.stat[0].value - enemyAtkDamage);
            wrefresh(win);
            usleep(4000000);
            printKnockOutDetails(enemyPokemon, playerPokemon);
        }

        else if(evasionChance2 >= enemyPokemon.learnedMoves[randomEnemyMove].accuracy){
            wclear(win);
            wprintw(win, "PC pokemon evaded the attack!\n\n");
            wrefresh(win);
            usleep(4000000);
        }
    }

    else{
         evasionChance = rand()%100;
         if(evasionChance < enemyPokemon.learnedMoves[randomEnemyMove].accuracy && playerPokemon.stat[0].value!=0){
            wprintw(win,"Enemy pokemon : %s uses %s!\n", enemyPokemon.identifier.c_str(), enemyPokemon.learnedMoves[randomEnemyMove].identifier.c_str());
            playerPokemon.stat[0].value = MAX(0, playerPokemon.stat[0].value - enemyAtkDamage);
            wrefresh(win);
            usleep(4000000);
            printKnockOutDetails(enemyPokemon, playerPokemon);
        }

        else if(evasionChance >= enemyPokemon.learnedMoves[randomEnemyMove].accuracy){
            wprintw(win, "PC pokemon evaded the attack!\n\n");
            wrefresh(win);
            usleep(4000000);
        }

        evasionChance2 = rand()%100;

         if(playerPokemon.stat[0].value!=0 && evasionChance2 < playerPokemon.learnedMoves[pcChosenMove].accuracy && enemyPokemon.stat[0].value!=0){
            wclear(win);
            wprintw(win,"PC pokemon : %s uses %s!\n", playerPokemon.identifier.c_str(), playerPokemon.learnedMoves[pcChosenMove].identifier.c_str());
            enemyPokemon.stat[0].value = MAX(0, enemyPokemon.stat[0].value - pcAtkDamage);
            wrefresh(win);
            usleep(4000000);
            printKnockOutDetails(playerPokemon, enemyPokemon);
        }

        else if(evasionChance2 >= playerPokemon.learnedMoves[pcChosenMove].accuracy){
            wclear(win);
            wprintw(win, "Enemy pokemon evaded the attack!\n\n");
            wrefresh(win);
            usleep(4000000);
        }

    }
}

void printKnockOutDetails(Pokemon atkPokemon, Pokemon knockedPokemon){
    if(knockedPokemon.stat[0].value == 0){
        wclear(win);
        wprintw(win, "%s has been knocked out\n\n", knockedPokemon.identifier.c_str());
        wprintw(win, "Surviving pokemon is %-15s HP:%d\n", atkPokemon.identifier.c_str(), atkPokemon.stat[0].value);
        wrefresh(win);
        usleep(4000000);
    }
}

/*
Handles usage of potion, revives, capturing pokemons
Depending on battleType, allow usage of pokeballs 
*/
char bagHandler(Pokemon &spawnedPokemon, int currPokemon, Pc *player, char isWildBattle){

    char userInput = '-';
    wclear(win);

    wprintw(win, "PokeBucks : %d\n\n", player->pokeBucks);
    //Display potions and revives
    for(int i=0; i<2; i++){
        wprintw(win, "%-40sx%d---(%d)\n", player->items[i].itemName.c_str(), player->items[i].qty, i);
    }

    //Display pokeballs if wild battle occurs
    if(isWildBattle == 'T'){
        wprintw(win, "%-40sx%d---(%d)\n", player->items[2].itemName.c_str(), player->items[2].qty, 2);
    }

    wprintw(win, "Enter value in brackets for items: ");
    wrefresh(win);
    userInput = wgetch(win);

    switch(userInput){
        case '0':
            potionHandler(currPokemon, player);
            break;

        case '1':
            reviveHandler(currPokemon, player);
            break;

        case '2':
            return captureHandler(spawnedPokemon, player);

        default:
            break;
    }

    return 'T';
}

/*
Handles escape mechanism
Returns F to break loop and escape battle , T otherwise
*/
char escapeHandler(Pokemon &spawnedPokemon,Pokemon pcPokemon, int &attempts){
    int trainerSpeed = pcPokemon.stat[3].value * 32;
    int wildSpeed = ((spawnedPokemon.stat[3].value)/4)%256;
    int oddEscape = floor(trainerSpeed/wildSpeed) + 30 * attempts;
    attempts++;

    int random = rand()%256;

    if(random < oddEscape){
        wclear(win);
        wprintw(win, "Player escaped the battle");
        wrefresh(win);
        usleep(2000000);
        return 'F';
    }

    wclear(win);
    wprintw(win, "Escape failed.");
    wrefresh(win);
    usleep(2000000);
    return 'T';
}

/*
Handles switching of pokemon
prints out pokemons and their HP's
*/
void switchPokemonHandler(Pc *player, int &currPokemon){

    wclear(win);
    for(long unsigned int i=0 ; i<player->pokemonByPC.size(); i++ ){
        wprintw(win,"%-20s HP:%d --(%d)\n", player->pokemonByPC[i].identifier.c_str(), player->pokemonByPC[i].stat[0].value, i);
    }
    wprintw(win, "\nEnter value in brackets to select/switch pokemon: ");
    wrefresh(win);

    currPokemon = wgetch(win) - '0';
}

/*
Potion and revive handler moved to pc.h for code reusability purposes
*/

/*
Helper method to determine battle status
Returns T if battle continues
Returns F to end battle

returns true (continue battle if there are other pokemons alive), 
Returns false if all pokemon is dead
*/
char battleStatus(Pokemon &spawnedPokemon, Pc *player){

    //if wild pokemon is knocked out, end battle
    if(spawnedPokemon.stat[0].value == 0){
        return 'F';
    }

    //if one of the pokemons of the pc is not dead , continue battle
    else{
        for(long unsigned int i =0; i<player->pokemonByPC.size(); i++){
            if(player->pokemonByPC[i].stat[0].value != 0){
                return 'T';
            }
        }
    }

    return 'F';
}

/*
Helper method for capturing pokemon
Capture pokemon chance based on type of pokeBalls used
*/
char captureHandler(Pokemon &spawnedPokemon, Pc *player){
    char userInput = '-';
    int selectedPokeBall = 0;
    int catchProbability = 0;

    wclear(win);
    wprintw(win,"Select PokeBall to use.\n\n");
    
    for(long unsigned int i=0; i<player->pokeBalls.size(); i++){
        wprintw(win, "%-20sx%d --(%d)\n", player->pokeBalls[i].ballName.c_str(), player->pokeBalls[i].qty, i);
    }

    wprintw(win, "\n\nEnter value in brackets to select Pokeball: ");
    wrefresh(win);
    userInput = wgetch(win);
    selectedPokeBall = userInput -'0';

    if(player->pokeBalls[selectedPokeBall].qty != 0){
        switch(selectedPokeBall){
            case 0:
                catchProbability = 5; //(20 % chance)
                break;
            
            case 1:
                catchProbability = 3; //(33% chance)
            
            case 2:
                catchProbability = 2; //(50% chance)

            case 3:
                catchProbability = 1; //(100% chance)
                break;
            
            default:
                break;

        }

        if(catchProbability == 1 || rand()%catchProbability == 1){
            wprintw(win, "\n%-20s (LV%d) has been captured!!", spawnedPokemon.identifier.c_str(), spawnedPokemon.currentLevel);
            wrefresh(win);
            usleep(2000000);

            if(player->pokemonByPC.size()<6){
                player->pokemonByPC.push_back(spawnedPokemon);
            }

            else{
                spawnedPokemon.stat[0].value = spawnedPokemon.stat[6].value; //Heals captured pokemon to full health before transferring to pokedex system
                universalPokedex.push_back(spawnedPokemon); //transfer to universal pokedex if party is full
            }

            player->items[2].qty--;
            player->pokeBalls[selectedPokeBall].qty--;
            return 'F';
        }
        
        else{
            wprintw(win, "\n%s has fleed!! Failed to catch pokemon!", spawnedPokemon.identifier.c_str());
            wrefresh(win);
            usleep(2000000);
            player->items[2].qty--;
            player->pokeBalls[selectedPokeBall].qty--;
            return 'F';
        }
    }
    
    //No pokeballs found
    else {
        wprintw(win, "\nInsufficient pokeBalls. Pokemon capture failed!");
        wrefresh(win);
        usleep(2000000);
        return 'T'; //battle continues
    }
}


/*
Helper method to determine battle status
Returns T if battle continues
Returns F to end battle

returns true (continue battle if there are other pokemons alive), 
Returns false if all pokemon is dead
*/
char npcBattleStatus(Npc &npc, Pc *player,int &npcCurrPokemon){

    char npcPokemonStatus = 'F';
    char pcPokemonStatus = '-';

    //Checks if there are still pokemons for npc to fight
    for(int i=0; i<npc.pokemonCount; i++){
        if(npc.pokemonByNPC[i].stat[0].value != 0){
            npcPokemonStatus = 'T';
            break;
        }
    }

    //Checks if pc still has pokemons to fight
     for(long unsigned int i =0; i<player->pokemonByPC.size(); i++){
            if(player->pokemonByPC[i].stat[0].value != 0){
                pcPokemonStatus = 'T';
            }
        }
    
    if(npcPokemonStatus  == 'T' && pcPokemonStatus == 'T'){
         //if enemy pokemon is knocked out, proceed with next npc pokemon
        if(npcCurrPokemon < npc.pokemonCount-1 && npc.pokemonByNPC[npcCurrPokemon].stat[0].value == 0){
            npcCurrPokemon++;
        }

        return 'T';
    }

    //Pc has successfully defeated all npc's pokemon
    else if(npcPokemonStatus == 'F' && pcPokemonStatus == 'T'){
        player->pokeBucks = (rand()%100 + 1) * npc.pokemonCount;
        return 'F';
    }

    return 'F';
}

char npcBattleInterface(Npc &npc,Pc *player, int &pcCurrPokemon, int &npcCurrPokemon){

    char userInput = '-';

    //Prints interface
    wprintw(win, "%-40s%s\n", "PC", "NPC");
    wprintw(win,"-----------------------------------------------------------\n");
    wprintw(win, "%-40s%s\n", player->pokemonByPC[pcCurrPokemon].identifier.c_str(), npc.pokemonByNPC[npcCurrPokemon].identifier.c_str());
    wprintw(win, "LV %-37dLV %d\n", player->pokemonByPC[pcCurrPokemon].currentLevel, npc.pokemonByNPC[npcCurrPokemon].currentLevel);
    wprintw(win, "HP:%-37dHP:%d\n\n\n", player->pokemonByPC[pcCurrPokemon].stat[0].value, npc.pokemonByNPC[npcCurrPokemon].stat[0].value);
    wprintw(win, "%-40s%s\n\n", "Fight (f)","Bag (b)");
    wprintw(win,"%-40s\n\n\n", "Switch Pokemon (s)");
    wprintw(win, "Enter value in brackets for actions: ");
    userInput = wgetch(win);

    switch(userInput){

        case 'f':
            pokemonMovesBattleHandler(npc.pokemonByNPC[npcCurrPokemon], player, pcCurrPokemon);
            break;
        
        case 'b':
            return bagHandler(npc.pokemonByNPC[npcCurrPokemon], pcCurrPokemon, player, 'F');
        
        case 's':
            switchPokemonHandler(player, pcCurrPokemon);
            break;
        
        default:
            break;

    }

    return npcBattleStatus(npc, player, npcCurrPokemon);
}
void npcPokemonBattleHandler(Npc &npc, Pc *player){
    char pokemonBattleStatus = 'F';
    int pcCurrPokemon = 0;
    int npcCurrPokemon = 0;

    wclear(win);
    wprintw(win,"A trainer has appeared!\n\n");
    wprintw(win,"Trainer's pokemons: \n\n");

    for(int i =0; i<npc.pokemonCount; i++){
        wprintw(win, "%-40s(LV %d)\n", npc.pokemonByNPC[i].identifier.c_str(), npc.pokemonByNPC[i].currentLevel);
    }
    wrefresh(win);
    usleep(4000000);

    for(long unsigned int i=0; i<player->pokemonByPC.size(); i++){
        if(player->pokemonByPC[i].stat[0].value != 0){
            pokemonBattleStatus = 'T';
            break;
        }
    }

    //All of PC's pokemon are knocked out prior to battle
    if(pokemonBattleStatus == 'F'){
        wclear(win);
        wprintw(win, "All of PC's pokemon has been knocked out.\n");
        wprintw(win, "NPC decides to spare you a chance and leave you alone.\n");
        wrefresh(win);
        usleep(4000000);
    }

    //While pokeomBatlle is ongoing
    while(pokemonBattleStatus == 'T'){
        wclear(win);
        pokemonBattleStatus = npcBattleInterface(npc, player, pcCurrPokemon, npcCurrPokemon);
        wrefresh(win);
    }

}