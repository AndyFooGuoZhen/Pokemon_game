#include "gameCoreLogic.h"

int main(int argc, char *argv[])
{
    //Initializes ncurses window
    initscr();
    start_color();
    win = newwin(22, 81, 0, 0); 
    cbreak();
    keypad(win, TRUE);

    World w1;
    Pc player;
    int trainerCountOriginal = 0;

    //Window setup, pokemon data,  world setup, initial map setup, pc and npc initialization, and starts game
    gameSetup(&w1, &player, argc, argv, trainerCountOriginal);

    gameStart(&w1, &player, argc, argv, trainerCountOriginal);
    
    endwin();

    return 0;
}




   




