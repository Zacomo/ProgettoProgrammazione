#include "../include/Level.h"
#include "../include/utility.h"
#include <ncurses.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <unistd.h>
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

    if(level > 1)
        shopMenu(pg, itemsSet);

    map.generate(rooms);
    map.freeSpots(_items,spots);
    for(Point p : spots)
    {
        int index = rand(0,min(level*3,static_cast<int>(itemsSet.size())-1));
        Item i = itemsSet[index];
        i.setPosition(p.x,p.y);
        string ID = "item" + to_string(id) + "." + to_string(level);
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

void Level::printMap(PlayableCharacter& player, Window& mapWindow)
{
    mapWindow.clear();
    WINDOW *win = mapWindow.getWin();
    move(0,0);
    for(int i = 0; i < map.getHeight(); i++)
    {
        for (int j = 0; j < map.getWidth(); j++)
        {
            if(map(j,i).isVisible())
                waddch(win,map(j,i).getSymbol());
            else
                waddch(win,' ');
        }
        //waddch(win,'\n');
    }
    for(auto i : items)
    {
        Point position = i.second.getPosition();
        if(i.second.isVisible())
            mvwaddch(win,position.y,position.x,i.second.getSymbol());
    }
    for(auto m : monsters)
    {
        Point position = m.second.getPosition();
        if(m.second.isAwake())
            mvwaddch(win,position.y,position.x,m.second.getSymbol());
    }
    Point p = player.getPosition();
    mvwaddch(win,p.y,p.x,'@');

    wrefresh(win);
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

int Level::handleMovement(Window& mapWindow, Window& info, Window& bottom,PlayableCharacter& player)
{
    int x,y,c;
    Point pos = player.getPosition();
    bool moved = false;
    x = pos.x;
    y = pos.y;
    map.showAround(x,y);
    list<Monster> monstersNearby;
    while(true)
    {
        monstersNearby.clear();
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
            case '$':
                player.setCoins(9999);
                break;
#endif
            case 'i':
                player.showInventory();
                break;
            case 'q':
                return 0;
                break;
        }
        player.setPosition(x,y);
        if(map(x,y).getId() != "")
            map.setVisible(map(x,y).getId(),monsters,items);
        else
            map.showAround(x,y);
        if(moved == true)
        {
            moved = false;
            for(auto& m : monsters)
                if(m.second.isAwake())
                    moveMonster(player.getPosition(),m.second);
            if(map(x,y).getType() == UP_STAIRS)
                return 1;
            else if(map(x,y).getType() == DOWN_STAIRS)
                return -1;
        }
        printMap(player,mapWindow);
        if(map(x,y).getUpperLayer() != "" && map(x,y).getUpperLayer()[0] == 'i')
            if(player.pickItem(items[map(x,y).getUpperLayer()]))
            {
                bottom.clear();
                bottom.printLine("Raccolto " + items[map(x,y).getUpperLayer()].getName());
                getch();
                bottom.clear();
                items.erase(map(x,y).getUpperLayer());
                map(x,y).setUpperLayer("");
            }
            else
            {
                bottom.clear();
                bottom.printLine("Hai camminato su " + items[map(x,y).getUpperLayer()].getName());
                getch();
                bottom.clear();
            } 
        info.clear();
        writeInfo(info,player,level);
        monstersAround(player.getPosition(),monstersNearby);
        if(!monstersNearby.empty())
        {
            for(Monster m : monstersNearby)
            if(Battle(bottom,info,player,level,m))
            {
                map(m.getPosition()).setUpperLayer("");
                monsters.erase(m.getId());
                printMap(player,mapWindow);
                info.clear();
                writeInfo(info,player,level);
            } 
            else
                return 0;
        }
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

void Level::shopMenu(PlayableCharacter& pg, vector<Item>& itemsSet)
{
    int priceMult = 4;
    int items = 3;
    int indexes[items];
    int yoffset = items + 1;
    int xoffset = 24;   // length of the name of the item with the longest name
    int xinfo = 20;
    int yinfo = 0;
    int choice = 0;
    bool done = false;
    bool noMoney = false;
    int confirm = -1;
    string ID = "item.shop." + to_string(level) + ".";
    Item chosenItem;

    generateKPermutation(indexes,0,itemsSet.size()-1,items);

    Menu shop(map.getWidth()/2 - xoffset,map.getHeight()/2 - yoffset,4, itemsSet[indexes[0]].getName().c_str(), itemsSet[indexes[1]].getName().c_str(), itemsSet[indexes[2]].getName().c_str(), "Sono Povero"); // c_str() returns the c string correpsonding to the string
    Window itemInfo(shop.getX() + xinfo, shop.getY() + yinfo, 11, 30);

    choice = shop.handleChoice();

    while (!done) {

        if (choice != 3) {
            chosenItem = itemsSet[indexes[choice]];
            itemInfo.clear();

            chosenItem = itemsSet[indexes[choice]];
            itemInfo.printLine(chosenItem.getName());
            itemInfo.printLine("Prezzo: " + to_string((1 + indexes[choice])*priceMult) + " Cucuzze");
            itemInfo.printLine("LP: + " + to_string(chosenItem.getLP()));
            itemInfo.printLine("MP: + " + to_string(chosenItem.getMP()));
            itemInfo.printLine("ATK: + " + to_string(chosenItem.getATK()));
            itemInfo.printLine("DEF: + " + to_string(chosenItem.getDEF()));
            itemInfo.printLine("LUCK: + " + to_string(chosenItem.getLuck()));
            itemInfo.separator();
            if(noMoney)     // If you tried to buy something you can't afford
            {
                itemInfo.printLine("Cucuzze insufficienti!");
                noMoney = false;
            }
            confirm = shop.handleChoice();

            if (confirm == choice){ // se scelto due volte, viene scalato il prezzo dell'Item
                if (pg.addCoins(-(1 + indexes[choice])*priceMult))  // true se bastano i fondi, false altrimenti
                {
                    chosenItem.setId(ID + to_string(choice));
                    pg.pickItem(chosenItem);
                    done = true;
                }
                else
                    noMoney = true;
                    itemInfo.printLine("Cucuzze insufficienti!");
                    //non viene stampato a causa del clear; implementare setFirstAvaiableLine?
            }
            else
                choice = confirm;   // Così fa vedere le stat della nuova scelta
        }
        else
            done = true;        // è stato selezionato quit/esci
    }

}

Point Level::getUpStairs()
{
    return upStairs;
}

Point Level::getDownStairs()
{
    return downStairs;
}

void Level::monstersAround(Point playerPos, std::list<Monster>& list)
{
    std::list<std::string> ids;

    for(int i = playerPos.y - 1; i < playerPos.y + 2; i++)
        for(int j = playerPos.x - 1; j < playerPos.x + 2; j++)
            if(!map(j,i).isWalkable() && map(j,i).getUpperLayer() != "" && map(j,i).getUpperLayer()[0] == 'm') 
                // Map(j,i) is not walkable as there is a monster on it
                ids.push_back(map(j,i).getUpperLayer());

    for(string id : ids)
    {
        list.push_back(monsters[id]);
    }
}

void Level::moveMonster(Point playerPosition, Monster& mons){
    int dist;
    Point fmpos;    //  Futura posizione del mostro
    Point mpos; //  Posizione del mostro
    mpos = mons.getPosition();
    fmpos = mpos;
    map(mpos).setUpperLayer("");

    dist = w(playerPosition, mpos);
    if (map(playerPosition).getId() == map(mpos).getId()) { // if the monster and the player are in the same room

        for (int i = 0; i < 4; i++) {
            switch (i) {
                case 0:
                    fmpos.x = mpos.x + 1;
                    fmpos.y = mpos.y;
                    break;
                case 1:
                    fmpos.x = mpos.x - 1;
                    fmpos.y = mpos.y;
                    break;
                case 2:
                    fmpos.x = mpos.x;
                    fmpos.y = mpos.y + 1;
                    break;
                case 3:
                    fmpos.x = mpos.x;
                    fmpos.y = mpos.y - 1;
                    break;
            }

            if (validPosition(fmpos, playerPosition) && (w(fmpos, playerPosition) < dist))
                mpos = fmpos;

        }
    }
    else{
        int rmove;
        rmove = rand(0,3);

        switch (rmove) {
            case 0:
                fmpos.x = mpos.x + 1;
                fmpos.y = mpos.y;
                break;
            case 1:
                fmpos.x = mpos.x - 1;
                fmpos.y = mpos.y;
                break;
            case 2:
                fmpos.x = mpos.x;
                fmpos.y = mpos.y + 1;
                break;
            case 3:
                fmpos.x = mpos.x;
                fmpos.y = mpos.y - 1;
                break;
        }

        if (validPosition(fmpos, playerPosition))
            mpos = fmpos;

    }

    mons.setPosition(mpos.x, mpos.y);   // Assegna la nuova posizione al mostro
    map(mpos).setUpperLayer(mons.getId());
}

bool Level::validPosition(Point pos,Point playerPos)
{
    return pos != playerPos && map(pos).getType() == PAVEMENT && map(pos).getId() != "";    // The monster can't leave
    //the room it's in and can't walk on the player
}

int Battle(Window& battle_win, Window& right_win, PlayableCharacter& player, int level,Monster& m){

    using namespace std;
    char c;
    bool noAttack = false;

    while ((m.getLP() > 0) && (player.getLP() > 0)) {

        //battle_win.printLine("Per iniziare la battaglia premere a, per consultare l'inventario premere s");
        battle_win.printLine("Per attaccare premere a, per consultare l'inventario premere i");
        c = getch();
        battle_win.clear();

        switch (c) {
            case 'a':

                battle_win.printLine("Battaglia con " + m.getName() + ":");
                battle_win.printLine("");
                battle_win.printLine("PUNTI VITA -> " + to_string(m.getLP()));
                battle_win.printLine("ATTACCO -> " + to_string(m.getATK()));
                battle_win.printLine("DIFESA -> " + to_string(m.getDEF()));

                //sleep(1);   // alternatively getch() press a key to continue
                getch();

                if (Critical_Atk(player.getLuck()) == 1){
                    m.setLP(m.getLP() - Atk_Def(m.getDEF(), (2 * player.getATK())));
                    battle_win.printLine("");
                    battle_win.printLine("COLPO CRITICO");
                }
                else
                    m.setLP(m.getLP() - Atk_Def(m.getDEF(), player.getATK()));

                if (m.getLP() < 0)
                    m.setLP(0);

                battle_win.clear();

                battle_win.printLine("Battaglia con " + m.getName() + ":");
                battle_win.printLine("");
                battle_win.printLine("PUNTI VITA -> " + to_string(m.getLP()));
                battle_win.printLine("ATTACCO -> " + to_string(m.getATK()));
                battle_win.printLine("DIFESA -> " + to_string(m.getDEF()));

                //sleep(2);
                getch();

                break;

            case 'i':
                player.showInventory();
                break;
            default:
                battle_win.clear();
                battle_win.printLine("Premere un tasto valido!");
                getch();
                battle_win.clear();
                noAttack = true;
                break;
        }

        if (m.getLP() > 0 && !noAttack) {
            battle_win.clear();
            battle_win.printLine("L'avversario ti attacca!");

            getch();

            player.setLP(player.getLP() - Atk_Def(player.getDEF(), m.getATK()));

            if (player.getLP() <= 0)
                player.setLP(0);

            right_win.clear();
            writeInfo(right_win, player,level);

            battle_win.printLine("");
            battle_win.printLine("L'attacco del nemico ti toglie -> " + to_string(Atk_Def(player.getDEF(), m.getATK())) + " LP");
        }
        else
            noAttack = false;

        //sleep(2);
    }

    if (m.getLP() <= 0) {
        battle_win.clear();
        battle_win.printLine("!VITTORIA!");
        getch();
        battle_win.clear();
        //mvprintw(4, 44,);
    }
    else if (player.getLP() <= 0){
        battle_win.clear();
        battle_win.printLine("SEI STATO SCONFITTO!");
        getch();
        battle_win.clear();
        return 0;
        //mvprintw(4, 39, "");
    }

    return 1;
}

int Atk_Def (int def, int atk) {
    return  (int)(atk -  (double)(atk * def) / 100);           // fare il casting
}

int Critical_Atk (int luck){
    
    int i=0;

    i = rand() % 100;
    
    if (i <= luck)    // se i <= della fortuna del pg allora viene effettuato il critico (atk*2)
        return 1;
    else
        return 0;
}
