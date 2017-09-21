#include "../include/Level.h"

Level::Level(int width,int height,int rooms, int _monsters, int _items):map(width,height),monsters(47),items(47)
{
    unordered_set<Points> spots;
    map.generate(rooms);
    map.freeSpots(_items,spots);
    for(Point p : spots)
    {
        // Place an object
    }
    spots.clear();
    map.freeSpots(_monsters,spots);
    for(Point p : spots)
    {
        // Spawn a monster
    }
}
