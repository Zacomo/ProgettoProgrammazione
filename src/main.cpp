#include "../include/Menu.h"
#include "../include/Window.h"
#include "../include/Level.h"
#include <cstdlib>
#include <cstring>
#include <ncurses.h>
#include <menu.h>
#include <errno.h>
#include <iostream>

using namespace std;

void startCurses();
// Sets up and starts curses mode

int mainMenu();
// Sets up and handles the main menu of the game (New Game, Credits, etc) 
// Returns an int corresponding to the choice made

void freeMainMenu(int n_choices,ITEM** my_items,char** choices,MENU* my_menu,WINDOW* menuwin);
// Frees up the memory used by the main menu parts

int playerChoiceMenu();
// Sets up the menu that lets the player choose the character
// Returns an int corresponding to the choice made

void endCurses();
// Ends curses mode

int main()
{	
    int choice;
    bool quit = false;
    int character = -1;

    while(character == -1 && !quit)
    {
        startCurses();
        choice = mainMenu();
        switch(choice)
        {
            case 0:
                startCurses();
                character = playerChoiceMenu();
                break;
            case 1:
                cout << "You selected Credits" << endl;
                break;
            case 2:
                quit = true;
                break;
        }
    }
    if(quit == true) 
        endCurses();
    else
    {
        endCurses();
        // Generate the map with the given character
    }
}

void startCurses()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();
}

int mainMenu()
{
    Menu mainMenu((COLS - 10)/2,(LINES - 10)/2,3,"New Game","Credits","Quit");
    refresh();
    return mainMenu.handleChoice();
}

void endCurses()
{
	endwin();
}
 
int playerChoiceMenu()
{
    Menu players((COLS)/2 - 25,(LINES - 10)/2,4,"Gaudenzio","Peppino","Badore","Indietro");
    Window description(players.getX() + 20,players.getY() - 5,19,70);
    bool done = false, close = false;
    int choice = -1;
    int confirm; 

    while(!done)
    {
        string filename = "resources/descriptions/";
        if(choice == -1)    // The first time choice is uninitialized (= -1); the next times this won't be necessary
            choice = players.handleChoice();
        switch(choice)
        {
            case 0:
                filename += "gaudenzio.desc";
                break;
            case 1:
                filename += "peppino.desc";
                break;
            case 2:
                filename += "badore.desc";
                break;
            case 3:
                close = true;
                break;
        }
        if(!close)
        {
            description.readFromFile(filename);
            confirm = players.handleChoice();
            if(confirm == choice)
                done = true;
            else
                choice = confirm;
        }
        else
        {
            return -1;   // The player hit back, no charater was selected
        }
    }

    return choice;
}
