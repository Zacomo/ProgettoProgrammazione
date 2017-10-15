#ifndef LEVEL_H
#define LEVEL_H
#include "Map.h"
#include <unordered_map>
#include <string>
#include <list>
#include "Graph.h"
#include "personaggiGiocabili.h"
#include "Monster.h"
#include "Item.h"
#include "Window.h"

class Level
{
    private:

        Map map;
        std::unordered_map<std::string,Monster> monsters;
        std::unordered_map<std::string,Item> items;
        Point upStairs;
        Point downStairs;
        int level;

    public:

        Level(int level,int width,int height,int rooms, int _monsters, int _items, PlayableCharacter& pg);
        // Given a width, a height and a number of rooms, monsters and items, it creates a new level

        void printMap(PlayableCharacter& player, Window& mapWindow);
        // Prints the map, the monsters, the items and the player

        void placeCharacter(PlayableCharacter& player, int playerPosition);
        // Given a main character and a preference for the position, places the characeter accordingly
        // if playerPosition == 0, the position is random
        // if playerPosition == 1, the position is the same as the one of the down stairs
        // if playerPosition == -1, the position is the same as the one of the up stairs

        int handleMovement(Window& mapWindow, Window& info, PlayableCharacter& player);
        // Handles the movement of the main character and the exploration of the map
        // and the printing of the stats

        int getLevel();
        //  Restituisce il numero del livello

        Point getUpStairs();
        // Returns the coordinates of the up stairs

        Point getDownStairs();
        // Returns the coordinates of the down stairs

        void monstersAround(PlayableCharacter& player, std::list<Monster>& list);
        // Given a player and an empty list, it fills the list with the the monsters around player

        void shopMenu(PlayableCharacter& pg, std::vector<Item>& itemsSet);
        //  Genera la finestra dello shop con i vari Item da acquistare
};

void writeInfo(Window& win,PlayableCharacter& pg, int level);
//  Scrive le informazioni nella finestra a destra, fornendo i dati su oggetti posseduti e sulle statistiche del PG

void writeEquipment(Window& win, PlayableCharacter& pg);
//  Scrive gli oggetti equipaggiati nella finestra a destra


void whatsHappening(Window& win,PlayableCharacter& pg);
//  Scrive gli avvenimenti nella finestra in basso

void move(PlayableCharacter& pg, Monster& mons, Map map);
//  Il mostro si sposta di 1 verso il pg (solo se questo accorcia la distanza tra loro)

#endif
