#include <cstring>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <climits>
#include <iostream>
#include <ncurses.h>
using namespace std;

WINDOW *win;

 #define MAX(a,b) ((a) > (b) ? (a) : (b))
 #define MIN(a,b) ((a) < (b) ? (a) : (b))

/*------------------------------Pokemon Class and related Data---------------------------*/
int checkPokedexInShare(string pathToRead);
void convertStringToArray(vector<string> &lineString,string linePerRow);
int verifyEmptyStr(string Input);
string printIntMax(int input);
void initializePokemonData();

template <class T>
void fillDetails(string pathToRead, string fileName, vector<T> &tList){
    vector<string> tempVector;
    string line;
    int count = 0;
    ifstream file (pathToRead + fileName);

    if(file.is_open()){
        while(file.peek()!=EOF){
            if(count == 0){
                getline(file,line);
                count++;
                continue;
            }
            getline(file, line);
            convertStringToArray(tempVector, line);
            T objTemp(tempVector);
            tList.push_back(objTemp);
            count++;
            tempVector.clear();
        }
        file.close();
        return;
    }

    wclear(win);
    wprintw(win, "cant retrieve data");
    wrefresh(win);
    usleep(100000000);
    return;
}

class Moves{
    public:
    // int dataLength = 15;
    int id;
    string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;

    Moves(vector<string> inputVector){
        id = verifyEmptyStr(inputVector[0]);
        identifier = inputVector[1];
        generation_id = verifyEmptyStr(inputVector[2]);
        type_id = verifyEmptyStr(inputVector[3]);
        power = verifyEmptyStr(inputVector[4]);
        pp = verifyEmptyStr(inputVector[5]);
        accuracy = verifyEmptyStr(inputVector[6]);
        priority = verifyEmptyStr(inputVector[7]);
        target_id = verifyEmptyStr(inputVector[8]);
        damage_class_id = verifyEmptyStr(inputVector[9]);
        effect_id = verifyEmptyStr(inputVector[10]);
        effect_chance = verifyEmptyStr(inputVector[11]);
        contest_type_id = verifyEmptyStr(inputVector[12]);
        contest_effect_id = verifyEmptyStr(inputVector[13]);
        super_contest_effect_id = verifyEmptyStr(inputVector[14]);
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
         printIntMax(id).c_str(), identifier.c_str(), printIntMax(generation_id).c_str(),
         printIntMax(type_id).c_str(), printIntMax(power).c_str(),printIntMax(pp).c_str(),printIntMax(accuracy).c_str(),
         printIntMax(priority).c_str(),printIntMax(target_id).c_str(),printIntMax(damage_class_id).c_str(),printIntMax(effect_id).c_str(),
         printIntMax(effect_chance).c_str(), printIntMax(contest_type_id).c_str(),
          printIntMax(contest_effect_id).c_str(),printIntMax(super_contest_effect_id).c_str());
    }
};

//Individual stats for spawned pokemon
class Stat{
    public:
    string statName;
    int value;

    Stat(string inputName, int inputValue){
        statName = inputName;
        value = inputValue;
    }
};

class Pokemon{
    public:
    // int dataLength = 8;
    int id;
    string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
    int currentLevel; //new attribute, not from csv files
    char isShiny;   //determines if pokemon is shiny
    vector<Moves> learnedMoves; //learned moves according to level-up moveset

    //stats for pokemon
    //0:HP, 1:attack, 2:defense, 3:speed, 4:special attack 5:special defense 6:MaxHP
    vector<Stat> stat; 

    Pokemon(vector<string> inputVector){
        id = verifyEmptyStr(inputVector[0]);
        identifier = inputVector[1];
        species_id = verifyEmptyStr(inputVector[2]);
        height = verifyEmptyStr(inputVector[3]);
        weight = verifyEmptyStr(inputVector[4]);
        base_experience = verifyEmptyStr(inputVector[5]);
        order = verifyEmptyStr(inputVector[6]);
        is_default = verifyEmptyStr(inputVector[7]);
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s,%s,%s,%s,%s,%s\n", 
         printIntMax(id).c_str(), identifier.c_str(), printIntMax(species_id).c_str(),
         printIntMax(height).c_str(), printIntMax(weight).c_str(),printIntMax(base_experience).c_str(),printIntMax(order).c_str(),
         printIntMax(is_default).c_str());
    }

    void printMoves(){
        for(int i = 0; i<2; i++){
            wprintw(win,"Move %d: %s\n", i+1, learnedMoves[i].identifier.c_str());
        }
        wprintw(win, "\n");
        wrefresh(win);
    }

    void printStats(){
        for(int i=0; i<6; i++){
            wprintw(win, "%s : %d\n", stat[i].statName.c_str(), stat[i].value);
        }
        wprintw(win, "Shiny : %c\n", isShiny );
        wprintw(win, "\n");
        wrefresh(win);
    }
};

class PokemonMoves{
    public:
    // int dataLength = 6;
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

    PokemonMoves(vector<string> inputVector){
        pokemon_id = verifyEmptyStr(inputVector[0]);
        version_group_id = verifyEmptyStr(inputVector[1]);
        move_id = verifyEmptyStr(inputVector[2]);
        pokemon_move_method_id = verifyEmptyStr(inputVector[3]);
        level = verifyEmptyStr(inputVector[4]);
        order = verifyEmptyStr(inputVector[5]);
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s,%s,%s,%s\n", 
         printIntMax(pokemon_id).c_str(), printIntMax(version_group_id).c_str(),
         printIntMax(move_id).c_str(), printIntMax(pokemon_move_method_id).c_str(),printIntMax(level).c_str(),
         printIntMax(order).c_str());
    }
};

class PokemonSpecies{
    public:
    // int dataLength = 20;
    int id;
    string identifier;
    int generation_id;
    int evolvesfrom_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

    PokemonSpecies(vector<string> inputVector){
        id = verifyEmptyStr(inputVector[0]);
        identifier = inputVector[1];
        generation_id = verifyEmptyStr(inputVector[2]);
        evolvesfrom_species_id = verifyEmptyStr(inputVector[3]);
        evolution_chain_id = verifyEmptyStr(inputVector[4]);
        color_id = verifyEmptyStr(inputVector[5]);
        shape_id = verifyEmptyStr(inputVector[6]);
        habitat_id = verifyEmptyStr(inputVector[7]);
        gender_rate = verifyEmptyStr(inputVector[8]);
        capture_rate = verifyEmptyStr(inputVector[9]);
        base_happiness = verifyEmptyStr(inputVector[10]);
        is_baby= verifyEmptyStr(inputVector[11]);
        hatch_counter= verifyEmptyStr(inputVector[12]);
        has_gender_differences = verifyEmptyStr(inputVector[13]);
        growth_rate_id = verifyEmptyStr(inputVector[14]),
        forms_switchable = verifyEmptyStr(inputVector[15]),
        is_legendary= verifyEmptyStr(inputVector[16]),
        is_mythical= verifyEmptyStr(inputVector[17]),
        order = verifyEmptyStr(inputVector[18]),
        conquest_order = verifyEmptyStr(inputVector[19]);
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s.%s,%s,%s\n", 
         printIntMax(id).c_str(), identifier.c_str(), printIntMax(generation_id).c_str(),
         printIntMax(evolvesfrom_species_id).c_str(), printIntMax(evolution_chain_id).c_str(),printIntMax(color_id).c_str(),printIntMax(shape_id).c_str(),
         printIntMax(habitat_id).c_str(),printIntMax(gender_rate).c_str(),printIntMax(capture_rate).c_str(),printIntMax(base_happiness).c_str(),
         printIntMax(is_baby).c_str(), printIntMax(hatch_counter).c_str(),
          printIntMax(has_gender_differences).c_str(),printIntMax(growth_rate_id).c_str(),printIntMax(forms_switchable).c_str(),printIntMax(is_legendary).c_str(),
          printIntMax(is_mythical).c_str(),printIntMax(order).c_str(),printIntMax(conquest_order).c_str());
    }
};

class Experience{
    public:
    // int dataLength = 3;
    int growth_rate_id;
    int level;
    int experience;

    Experience(vector<string> inputVector){
        growth_rate_id = verifyEmptyStr(inputVector[0]);
        level = verifyEmptyStr(inputVector[1]);
        experience = verifyEmptyStr(inputVector[2]);
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s\n", 
         printIntMax(growth_rate_id).c_str(), printIntMax(level).c_str(),
         printIntMax(experience).c_str());
    }
};

class TypeNames{
    public:
    // int dataLength = 3;
    int type_id;
    int local_language_id;
    string name;

     TypeNames(vector<string> inputVector){
        type_id = verifyEmptyStr(inputVector[0]);
        local_language_id = verifyEmptyStr(inputVector[1]);
        name = inputVector[2];
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s\n", 
         printIntMax(type_id).c_str(), printIntMax(local_language_id).c_str(),
         name.c_str());
    }
};

class PokemonStats{
    public:
    // int dataLength = 4;
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;

    PokemonStats(vector<string> inputVector){
        pokemon_id = verifyEmptyStr(inputVector[0]);
        stat_id = verifyEmptyStr(inputVector[1]);
        base_stat = verifyEmptyStr(inputVector[2]);
        effort = verifyEmptyStr(inputVector[3]);
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s,%s\n", 
         printIntMax(pokemon_id).c_str(), printIntMax(stat_id).c_str(),
         printIntMax(base_stat).c_str(), printIntMax(effort).c_str());
    }
};

class Stats{
    public:
    // int dataLength = 5;
    int id;
    int damage_class_id;
    string identifier;
    int is_battle_only;
    int game_index;

    Stats(vector<string> inputVector){
        id = verifyEmptyStr(inputVector[0]);
        damage_class_id = verifyEmptyStr(inputVector[1]);
        identifier = inputVector[2];
        is_battle_only = verifyEmptyStr(inputVector[3]);
        game_index = verifyEmptyStr(inputVector[4]);
    }

    void printProperties(){
        wprintw(win,"%s,%s,%s,%s,%s\n", 
        printIntMax(id).c_str(), printIntMax(damage_class_id).c_str(), identifier.c_str(),
         printIntMax(is_battle_only).c_str(), printIntMax(game_index).c_str());
    }
};

class PokemonType{
    public:
    // int dataLength = 3;
    int pokemon_id;
    int type_id;
    int slot;

    PokemonType(vector<string> inputVector){
        pokemon_id = verifyEmptyStr(inputVector[0]);
        type_id = verifyEmptyStr(inputVector[1]);
        slot = verifyEmptyStr(inputVector[2]);
    }

    void printProperties(){
         wprintw(win,"%s,%s,%s\n", 
         printIntMax(pokemon_id).c_str(), printIntMax(type_id).c_str(),
         printIntMax(slot).c_str());
    }
};

//Converts a string to vector array
void convertStringToArray(vector<string> &lineString, string linePerRow){ 
    stringstream string_stream(linePerRow);

    while(string_stream.good()){
        string temp;
        getline(string_stream,temp,',');
        lineString.push_back(temp);
    }
}

//Checks if string is empty, assigns int_max to int if so
int verifyEmptyStr(string Input){
    if(Input == ""){
        return INT_MAX;
    }
    return stoi(Input);
}

//Prints "" if int is empty
string printIntMax(int input){
    if (input == INT_MAX){
        return  "";
    }   
    else{
        return to_string(input);
    }
}

/*
Helper method to check if pokedex file exists in 
*/
int checkPokedexInShare(string pathToRead){ 
    DIR *d;
    d = opendir(pathToRead.c_str());

    if (d) {
       return 1;
    }
    
    return 0;
}

//Vectors for each csv data
vector<Pokemon> pokemonList;
vector<Moves> movesList;
vector<PokemonMoves> pokemonMovesList;
vector<PokemonSpecies> pokemonSpeciesList;
vector<Experience> experienceList;
vector<TypeNames> typeNamesList;
vector<PokemonStats> pokemonStatsList;
vector<Stats> statsList;
vector<PokemonType> pokemonTypeList;



/*------------------------------Pokemon Handlers---------------------------*/


/*Initializes pokemonData*/
void initializePokemonData(){
    string pathToRead = "/share/cs327/pokedex/";
    string csvFiles[] = {"pokemon.csv", "moves.csv", "pokemon_moves.csv", "pokemon_species.csv",
     "experience.csv", "type_names.csv", "pokemon_stats.csv", "stats.csv", "pokemon_types.csv" };

    int exists = checkPokedexInShare(pathToRead);

    //If share file does not contain pokedex
    if(!exists){
        pathToRead = (string)getenv((char*)"HOME")+ "/.poke327/pokedex/";
    }

    pathToRead = pathToRead + "pokedex/data/csv/";

     //Fill up data for each pokemon data
    fillDetails(pathToRead, csvFiles[0], pokemonList);
    fillDetails(pathToRead, csvFiles[1], movesList);
    fillDetails(pathToRead, csvFiles[2], pokemonMovesList);
    fillDetails(pathToRead, csvFiles[3], pokemonSpeciesList);
    fillDetails(pathToRead, csvFiles[4], experienceList);
    fillDetails(pathToRead, csvFiles[5], typeNamesList);
    fillDetails(pathToRead, csvFiles[6], pokemonStatsList);
    fillDetails(pathToRead, csvFiles[7], statsList);
    fillDetails(pathToRead, csvFiles[8], pokemonTypeList);
}

char toSpawnPokemon(char **grid, int playerXCoor, int playerYCoor);
void pokemonMovesHandler(Pokemon &spawnedPokemon);

void pokemonStatsHandler(Pokemon &spawnedPokemon);
Pokemon initiateSpawnPokemonSequence(int currMapX, int currMapY);


/*
Checks if PC is standing on grass, and 10% chance is met
Returns T if true. F if false
*/
char toSpawnPokemon(char **grid, int playerXCoor, int playerYCoor){

    if(grid[playerXCoor][playerYCoor] == ':' && rand()%10 == 9){
        return 'T';
    }
    return 'F';
}

/*
Spawns pokemon and enter battle sequence with pokemon
1092 pokemons to choose from
Level of pokemon determined by manhattan distance and random
*/
Pokemon initiateSpawnPokemonSequence(int currMapX, int currMapY){

    //Uses manhattan distance to determine min and max level of pokemon
    int manhattanDist = (int)sqrt(pow(abs(currMapY-200),2) + pow(abs(currMapX-200),2));
    int minLevelPokemon = 1;
    int maxLevelPokemon = (int)MAX(1, (int)(manhattanDist/2));

    int randomPokemonIndex = rand()%1092 + 1;
    Pokemon spawnedPokemon = pokemonList[randomPokemonIndex];

    //Handles special case where min and max is lv1
    if(maxLevelPokemon - minLevelPokemon == 0){
        spawnedPokemon.currentLevel = 1;
    }
    else if(manhattanDist > 200){
        minLevelPokemon = (int) MAX(1, (int)((manhattanDist-200)/2));
        maxLevelPokemon = 100;
        spawnedPokemon.currentLevel = rand()%(maxLevelPokemon - minLevelPokemon) + minLevelPokemon + 1; 
    }else{
        spawnedPokemon.currentLevel = rand()%(maxLevelPokemon) + 1; 
    }   

    //Determines if pokemon is shiny
    spawnedPokemon.isShiny = random()%8192 == 0 ? 'T' : 'F';

    pokemonMovesHandler(spawnedPokemon);
    pokemonStatsHandler(spawnedPokemon);

    return spawnedPokemon;
}

/*
Finds level-up moveset of pokemon, and randomly select 2 moves for 
pokemon
*/
void pokemonMovesHandler(Pokemon &spawnedPokemon){

    int species_id = spawnedPokemon.species_id;
    int currentPokemonLevel = spawnedPokemon.currentLevel;
    vector<PokemonMoves> tempPokemonMoves;
   
    //find moves that match with species_id and method id = 1 and level is <= pokemon_moves.level
    for(long unsigned int i = 0 ; i<pokemonMovesList.size(); i++){
        if(pokemonMovesList[i].pokemon_id == species_id && pokemonMovesList[i].pokemon_move_method_id == 1 && pokemonMovesList[i].level <= currentPokemonLevel){
            tempPokemonMoves.push_back(pokemonMovesList[i]);
        }
    }

    int chosenRandomMove =0;
    int moves_ID = 0;

    //Randomize and choose 2 from those sets of moves
    for(int i=1; i<=2; i++){

        chosenRandomMove = rand()%tempPokemonMoves.size();
        moves_ID = tempPokemonMoves[chosenRandomMove].move_id;

        //Searches throught movesList for matching ID, as movesList ID is not consistent
        for(long unsigned int j = 0; j<movesList.size(); j++){
            if(movesList[j].id == moves_ID){
                spawnedPokemon.learnedMoves.push_back(movesList[j]);
                break;
            }
        }

         tempPokemonMoves.erase(tempPokemonMoves.begin() + chosenRandomMove);
    }

}

/*
Assigns pokemon stats, determines IV value of pokemon
*/
void pokemonStatsHandler(Pokemon &spawnedPokemon){
    int spawnedPokemonId = spawnedPokemon.id;
    int level = spawnedPokemon.currentLevel;
    int baseStat = 0;
    int IV = 0;
    int value = 0;

    //Retrieves pokemon stats for this particular pokemon
    vector<PokemonStats> statsForPokemon;
    for(long unsigned int i =0; i<pokemonStatsList.size(); i++)
    {
        if(pokemonStatsList[i].pokemon_id == spawnedPokemonId){
            statsForPokemon.push_back(pokemonStatsList[i]);
        }
    }

    //Randomizes IV, computes value of each attribute
    for(int i=0; i<6; i++){
        IV = rand()%15;
        baseStat = statsForPokemon[i].base_stat;

            if(i==0){
                value = (int)floor(((baseStat + IV) * 2 * level)/100) + level + 10;
            }

            else{
                value = (int)floor(((baseStat + IV) * 2 * level)/100) + 5;
            }

            Stat s(statsList[i].identifier,value);
            spawnedPokemon.stat.push_back(s);
    }

    //Max hp for pokemon 
    Stat maxHP ("Max HP", spawnedPokemon.stat[0].value);
    spawnedPokemon.stat.push_back(maxHP);
}

