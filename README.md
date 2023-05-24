# Pokemon game

This is a CLI based pokemon game that was developed using C/C++ and the ncruses library for my class project. This game is developed initially using C and was ported to C++ in later parts of the semester.

## Features supported
- Battling and capturing of pokemons
- Battling with NPC's
- Healing and swapping pokemon compositions 
- Buying essential items such as pokeballs, potions and revives
- Traversing different maps and capturing higher level pokemons as the game progresses

## Diplay and layout
A single map is displayed on a grid using a 80 x 21 character layouts. The ncurses library is used to handle color rendering of characters and handling keyboard events from users. The entire game has 401 x 401 maps that the user will be able to explore. For performance optimization reasons, these maps will be generated on the fly once the user arrives at a new map.

<img width="566" alt="SCR-20230523-jexi" src="https://github.com/AndyFooGuoZhen/Pokemon_game/assets/77149531/1a35752c-17df-4e47-b7d1-1dd555b408b6">

## Characters and specific buildings are marked with their corresponding predefined symbols

### Characters
- R: Rivals, serves an an NPC that would chase you around to battle with you
- H: Hikers, similar to Rivals, but they can traverse through different terrains while chasing you
- P: Pacers, an NPC that walks back and forth in a fixed direction, battles with users if user is met
- W: Wanderers, similar to Pacers, but they random directions across the map
- S: Sentries, fixed NPC that doesn't move
- E : Explorers , NPC that moves like Wanderers but they have the ability to traverse different terrains

### Buildings 
- C: Pokecenters, used for healing pokemons and changing current pokemon grouping and compositions
- M: Pokemarts, used for purchasing pokeballs, revives, and potions

### Terrain symbols
- % : Boulders, edge of the world
- \# : Roads
- : : Long grass
- . : Clearings
- ^ : Trees and forests

## User interface and controls
Users interact with the game via the commands below

### Motion related commands
- 7 : upper left
- 8 : top
- 9: upper right
- 6: right
- 3: lower right
- 2: down
- 1: lower left
- 4: left

### Other commands
- \> : Entering pokemarts and pokecenters
- < : Exiting pokemarts and pokecenters
- Q : Quitting the game
- t : Shows the number of NPC on current map
- b : Shows contents of bag
- ESC : Exiting and returning to normal game interface

### Inner workings of NPCs
- A heap implemenetaion of Djikstra's algorithm was used for computing the shortest distance for NPC to reach the user. 
- Due to the limitations bounded by the terrain for Rivals and Hikers, the shortest paths for these NPC had to be tweaked according to the costs of the incrossable terrains.
- For every move the user makes, the Djikstra algorithm had to be run again to compute a new shortest path
- To avoid collisions of NPC, a priority queue was used to handle the movement order of the NPC's







