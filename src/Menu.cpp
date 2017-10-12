#include "../include/Menu.h"
#include <cstring>
#include <cstdlib>

using namespace std;

Menu::Menu(int x, int y, int n, string option1, ...):x(x),y(y)
{
    va_list args;
    va_start(args,option1);
    nlines = n;

    string max = option1;
    ncols = max.length();

    for(int i = 0; i < n - 1; i++) // given n arguments, the variadic arguments are n - 1
    {
        string current(va_arg(args,char*));
        if(current.length() > ncols)
        {
            max = current;
            ncols = current.length();
        }
    }   // finds the longest string and assigns its length to ncols

    choices = (char**)malloc(nlines*sizeof(char*)); // Given nlines choices, an array of nlines char arrays is
    // allocated
    items = (ITEM**)malloc((nlines + 1)*sizeof(ITEM*));

    va_start(args,option1);
    choices[0] = (char*)malloc(ncols*sizeof(char));
    strcpy(choices[0],option1.c_str());
    items[0] = new_item(choices[0],nullptr); 

    for(int i = 1; i < nlines; i++)
    {
        choices[i] = (char*)malloc(ncols*sizeof(char)); // ncols is the length of the longest string in the list
        strcpy(choices[i],va_arg(args,char*));
        items[i] = new_item(choices[i],nullptr); 
    }
    items[nlines] = nullptr;
    
    menuwin = newwin(nlines + 1,ncols,y,x);
    subwin = derwin(menuwin,nlines,ncols,1,0); // The items show up one line after the title, at the same indentation
    //level as the title
    keypad(menuwin,TRUE); // Lets the user use arrow keys
    keypad(subwin,TRUE);
    
    menu = new_menu(items);
    set_menu_win(menu,menuwin);
    set_menu_sub(menu,subwin);

    choice = 0;
    set_menu_mark(menu,"");
    curs_set(0);

    post_menu(menu);
    wrefresh(menuwin);
}

Menu::~Menu()
{
    unpost_menu(menu);
    wrefresh(menuwin);
	free_menu(menu);
    delwin(subwin);
    delwin(menuwin);
    for(int i = 0; i < nlines; i++)
    {
        free_item(items[i]);
        free(choices[i]);
    }
    free(choices);
    free(items);
}

int Menu::handleChoice()
{
    int c;
    bool done = false;

	while(!done)
	{   
        c = wgetch(menuwin);
        switch(c)
	    {	
            case KEY_DOWN:
		        menu_driver(menu, REQ_DOWN_ITEM);
                if(choice < nlines - 1)
                    choice++;
				break;
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
                if(choice > 0)
                    choice--;
				break;
            case 10:    // Enter
                /*
                ITEM* cur_item = current_item(menu);    // Fix: init i at 0, increase and decrease with key_up and
                                                        // key_down, return choice in constant time
                while(i < nlines && choice == -1)
                {
                    if(strcmp(item_name(cur_item),choices[i]) == 0)
                    i++;
                }
                */
                done = true;
                break;
		}
        wrefresh(menuwin);
	}	
    return choice;
}

int Menu::getX()
{
    return x;
}

int Menu::getY()
{
    return y;
}
