#include "../include/Level.h"
#include "../include/utility.h"
#include <ncurses.h>
#include <vector>
#include <cmath>
#include <fstream>
#include "../include/Menu.h"

using namespace std;

Level::Level(int level, int width, int height, int rooms, int _monsters, int _items, PlayableCharacter& pg):level(level),map(width,height),monsters(47),items(47)
{
    unordered_set<Point> spots;
    vector<Item> itemsSet;
    vector<Monster> monstersSet;
    int id = 0;
    ifstream itemsFile("resources/items.txt");
    retrieveItems(itemsFile,itemsSet);
    monstersSet = {Monster("Goblin",level),Monster("Troll",level),Monster("Golem",level),Monster("Gineppino",level)};

    shopMenu(pg, itemsSet);

    map.generate(rooms);
    map.freeSpots(_items,spots);
    for(Point p : spots)
    {
        int index = rand(0,min(level*3,static_cast<int>(itemsSet.size())-1));
        Item i = itemsSet[index];
        i.setPosition(p.x,p.y);
        string ID = "item" + to_string(id);
        i.setId(ID);
        id++;
        map(p.x,p.y).setUpperLayer(ID);
        items[ID] = i;
        // Place an object
    }
    spots.clear();
    id = 0;
    map.freeSpots(_monsters,spots);
    for(Point p : spots)
    {
        int index = rand(0,monstersSet.size()-1);
        Monster m = monstersSet[index];
        m.setPosition(p.x,p.y);
        string ID = "monster" + to_string(id);
        m.setId(ID);
        id++;
        map(p.x,p.y).setUpperLayer(ID);
        monsters[ID] = m;
        // Spawn a monster
    }

    Room R = map.pickRoom();
    Point p = map.freeSpot(R);
    map(p.x,p.y).setType(UP_STAIRS);
    upStairs = {p.x,p.y};
}

void Level::printMap(PlayableCharacter& player)
{
    clear();
    move(0,0);
    for(int i = 0; i < map.getHeight(); i++)
    {
        for (int j = 0; j < map.getWidth(); j++)
        {
            if(map(j,i).isVisible())
                addch(map(j,i).getSymbol());
            else
                addch(' ');
        }
        addch('\n');
    }
    for(auto m : monsters)
    {
        Point position = m.second.getPosition();
        if(m.second.isAwake())
            mvaddch(position.y,position.x,m.second.getSymbol());
    }
    for(auto i : items)
    {
        Point position = i.second.getPosition();
        if(i.second.isVisible())
            mvaddch(position.y,position.x,i.second.getSymbol());
    }
    Point p = player.getPosition();
    mvaddch(p.y,p.x,'@');

    refresh();
}

void Level::placeCharacter(PlayableCharacter& player,int playerPosition)
{
    switch(playerPosition)
    {
        case 0:
            {
                string room = map.placeCharacter(player);
                map.setVisible(room,monsters,items);
                if (level > 1)
                {
                    int x = player.getPosition().x;
                    int y = player.getPosition().y;
                    map(x,y).setType(DOWN_STAIRS);
                    downStairs = {x,y};
                }
            }
            break;
            
        case 1:
            player.setPosition(downStairs.x,downStairs.y);
            break;

        case -1:
            player.setPosition(upStairs.x,upStairs.y);
            break;
    }
}

int Level::handleMovement(Window& info,PlayableCharacter& player)
{
    int x,y,c;
    Point pos = player.getPosition();
    bool moved = false;
    x = pos.x;
    y = pos.y;
    map.showAround(x,y);
    while(true)
    {
        c = getch();
        switch(c)
        {
            case 'k':
            case KEY_UP:
                if(map(x,y-1).isWalkable())
                {
                    y = y - 1;
                    moved = true;
                }
                break;

            case 'j':
            case KEY_DOWN:
                if(map(x,y + 1).isWalkable()) 
                {
                    y = y + 1;
                    moved = true;
                }
                break;

            case 'h':
            case KEY_LEFT:
                if(map(x - 1,y).isWalkable()) 
                {
                    x = x - 1;
                    moved = true;
                }
                break;

            case 'l':
            case KEY_RIGHT:
                if(map(x + 1,y).isWalkable()) 
                {
                    x = x + 1;
                    moved = true;
                }
                break;
#ifdef DEBUG
            case 'p':
                for(int i = 0; i < map.getHeight(); i++)
                    for(int j = 0; j < map.getWidth(); j++)
                        map(j,i).setVisible(true);
                for(auto& m : monsters)
                    m.second.wakeUp(true);
                for(auto& i : items)
                    i.second.setVisible(true);
                break;
            case 'n':
                return 1;
                break;
            case 'N':
                return -1;
                break;
#endif
            case 'q':
                return 0;
                break;
        }
        player.setPosition(x,y);
        if(map(x,y).getId() != "")
        {
            map.setVisible(map(x,y).getId(),monsters,items);
        }
        else
        {
            map.showAround(x,y);
        }
        if(moved == true)
        {
            if(map(x,y).getType() == UP_STAIRS)
                return 1;
            else if(map(x,y).getType() == DOWN_STAIRS)
                return -1;
        }
        printMap(player);
        info.clear();
        writeInfo(info,player,level);

    }
}

int Level::getLevel() {
    return level;
}

void writeEquipment(Window& win, PlayableCharacter& pg){
    string msg1 = "";
    string msg2 = "";
    for (int i = 0; i < 5; i++) {
        switch (i) {
            case 0:
                msg1 = "Elmo: ";
                if (pg.getEquipmentAt(0) == Item()) {
                    msg2 = "Non equipaggiato";
                }
                else {
                    msg2 = pg.getEquipmentAt(0).getName();
                }
                msg1 = msg1 + msg2;
                win.printLine(msg1);
                break;

            case 1:
                msg1 = "Corazza: ";
                if (pg.getEquipmentAt(1) == Item())
                    msg2 = "Non equipaggiata";
                else
                    msg2 = (pg.getEquipmentAt(1)).getName();

                msg1 = msg1 + msg2;
                win.printLine(msg1);
                break;

            case 2:
                msg1 = "Stivali: ";
                if (pg.getEquipmentAt(2) == Item())
                    msg2 = "Non equipaggiati";
                else
                    msg2 = (pg.getEquipmentAt(2)).getName();

                msg1 = msg1 + msg2;
                win.printLine(msg1);
                break;

            case 3:
                msg1 = "Spada: ";
                if (pg.getEquipmentAt(3) == Item())
                    msg2 = "Non equipaggiata";
                else
                    msg2 = (pg.getEquipmentAt(3)).getName();

                msg1 = msg1 + msg2;
                win.printLine(msg1);
                break;

            case 4:
                msg1 = "Scudo: ";
                if (pg.getEquipmentAt(4) == Item())
                    msg2 = "Non equipaggiato";
                else
                    msg2 = (pg.getEquipmentAt(4)).getName();

                msg1 = msg1 + msg2;
                win.printLine(msg1);
                break;
        }
    }
}

void writeInfo(Window& win,PlayableCharacter& pg, int level){
    win.box();
    win.printLine(pg.getName());
    win.printLine("");
    win.printLine("LP: " + to_string(pg.getLP()) + '/' + to_string(pg.getLPMAX()));
    win.printLine("MP: " + to_string(pg.getMP()) + '/' + to_string(pg.getMPMAX()));
    win.printLine("ATK: " + to_string(pg.getATK()));
    win.printLine("DEF: " + to_string(pg.getDEF()));
    win.printLine("LV: " + to_string(pg.getLV()));
    win.printLine("Cucuzze: " + to_string(pg.getCoins()));
    win.printLine("");
    win.printLine("Livello attuale: " + to_string(level));

    win.separator();

    writeEquipment(win, pg);

}

void shopMenu(PlayableCharacter& pg, vector<Item>& itemsSet){
    int tmp = -1;

    int i0=0, i1=0, i2=0;
    i0 = rand() % itemsSet.size(); // Non dipende dal livello e potrebbero essere
    i1 = rand() % itemsSet.size(); // uguali. Inoltre non distinguo gli equip tra loro
    i2 = rand() % itemsSet.size();

    int choice = 0;

    Menu shop(COLS/2,LINES/2,4, itemsSet[i0].getName().c_str(), itemsSet[i1].getName().c_str(), itemsSet[i2].getName().c_str(), "Sono Povero");
    // c_str() returns the c string correpsonding to the string

    refresh();  // potrebbe non servire
    choice = shop.handleChoice();

    switch (choice){
        case 0:
            // istruzioni per aggiungere equip1
            pg.pickItem(itemsSet[i0]);
            pg.addCoins(-i0*5);
            // pagamento
            break;
        case 1:
            // istruzioni per aggiungere equip2
            pg.pickItem(itemsSet[i1]);
            pg.addCoins(-i1*5);
            // pagamento
            break;
        case 2:
            // istruzioni per aggiungere Consumable
            pg.pickItem(itemsSet[i2]);
            pg.addCoins(-i2*5);
            //pagamento
            break;
    }
}

void whatsHappening(Window& win, PlayableCharacter& pg){
    // Dovranno esserci le comunicazioni della battaglia, della raccolta di oggetti, dell'aumento delle stats
}

Point Level::getUpStairs()
{
    return upStairs;
}

Point Level::getDownStairs()
{
    return downStairs;
}

void Level::monstersAround(PlayableCharacter& player, std::list<Monster>& list)
{
    std::list<std::string> ids;
    Point p = player.getPosition();

    for(int i = p.y - 1; i < p.y + 2; i++)
        for(int j = p.x - 1; j < p.x + 2; j++)
            if(!map(j,i).isWalkable() && map(j,i).getUpperLayer() != "" && map(j,i).getUpperLayer()[0] == 'm') 
                // Map(j,i) is not walkable as there is a monster on it
                ids.push_back(map(j,i).getUpperLayer());

    for(string id : ids)
    {
        list.push_back(monsters[id]);
    }
}
