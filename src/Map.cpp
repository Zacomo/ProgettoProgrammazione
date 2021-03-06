#include "../include/Map.h"
#include "../include/utility.h"
#include <unordered_set>
#include <ncurses.h>

using namespace std;

Map::Map(int width, int height) : grid(height, width), width(width), height(height)
{
    for(int i = 0; i < width; i++)        // Map initialization
        for(int j = 0; j < height; j++)
        {
            (*this)(i,j).setType(WALL);
            (*this)(i,j).setVisible(false);
        }
}
// Creates a new map with the given # of rows and columns


Tile& Map::operator () (const int x, const int y) const
{
    return this->grid(y,x);
}
// Returns a reference variable to Map(x,y)

Tile& Map::operator () (const Point p) const
{
    return this->grid(p.y,p.x);
}
// Returns a reference variable to Map(x,y)

int Map::getWidth() const
{
    return this->width;
}

int Map::getHeight() const
{
    return this->height;
}

void Map::place(const Room& R)
{
    Point p = R.getCorner();
    int RWidth = R.getWidth();
    int RHeight = R.getHeight();

    for(int i = p.x; i < p.x + RWidth; i++)
        for(int j = p.y; j < p.y + RHeight; j++)
        {
            if((i != p.x && i != p.x + RWidth - 1) && (j != p.y && j != p.y + RHeight - 1))
                (*this)(i,j).setType(PAVEMENT);
            else
                (*this)(i,j).setType(ROOM_BORDER);
            (*this)(i,j).setId(R.getId());
            (*this)(i,j).setVisible(R.isVisible());
        }
}

Room Map::pickRoom()
{
    int index = rand(0,rooms.size()-1);
    unordered_map<string,Room>::iterator it = rooms.begin();

    for(; index > 0; index--)
        ++it;

    return (*it).second;
}

void Map::setVisible(string id,std::unordered_map<std::string,Monster>& monsters,std::unordered_map<std::string,Item>& items)
{
    this->rooms[id].setVisible(true);
    Point p = this->rooms[id].getCorner();    
    int width = this->rooms[id].getWidth();
    int height = this->rooms[id].getHeight();
    for(int i = p.x; i < p.x + width; i++)
        for(int j = p.y; j < p.y + height; j++)
        {
            (*this)(i,j).setVisible(true);
            if((*this)(i,j).getUpperLayer() != "")   // Wake up monsters and show items
            {
                if((*this)(i,j).getUpperLayer().front() == 'm')
                    monsters[(*this)(i,j).getUpperLayer()].wakeUp(true);
                else
                    items[(*this)(i,j).getUpperLayer()].setVisible(true);
            }
        }
}

void Map::showAround(int x, int y)
{
    (*this)(x + 1,y).setVisible(true);
    (*this)(x - 1,y).setVisible(true);
    (*this)(x,y + 1).setVisible(true);
    (*this)(x,y - 1).setVisible(true);
}

int Map::generate(int requiredRooms)
{
    unordered_map<string,Room>::iterator it;
    Room R,Q;
    Graph dots;

    generateRooms(requiredRooms);
    populateGraph(dots);
    createLinks(dots);

    it = this->rooms.begin();
    R = (*it).second;
    ++it;
    while(it != this->rooms.end())
    {
        Q = (*it).second;
        ++it;
        link(R,Q,dots);
        R = Q;
    }

    return this->rooms.size();
}

Room Map::generateRoom(Area A,string id)
{
    Point p;
    int x = A.getCorner().x, y = A.getCorner().y, width = A.getWidth(), height = A.getHeight(), wMax, hMax, w, h, freeXSpace,freeYSpace;

    p.x = rand(x + 1, x + width - 2 - 7); // a room has a minimum size of 7x7 and a max size of 20x20
    p.y = rand(y + 1, y + height - 2 - 7); 
    
    freeXSpace = x + width - 2 - p.x;
    freeYSpace = y + height - 2 - p.y;

    wMax = (20 < freeXSpace ? 20 : freeXSpace);
    hMax = (20 < freeYSpace ? 20 : freeYSpace);

    w = rand(7, wMax);
    h = rand(7, hMax);

    return Room(p,w,h,id);
}

void Map::populateGraph(Graph& G)
{
    for(int i = 1; i < height - 1; i++)
        for(int j = 1; j < width - 1; j++)
        {
            if((*this)(j,i).getType() == WALL)
                G.insertPoint({j,i});
        }
}

void Map::createLinks(Graph& G)
{
    for(int i = 0; i < this->height; i++)
        for(int j = 0; j < this->width; j++)
        {
            Point current = {j, i};
            Point right = {j + 1, i};
            Point down = {j, i + 1};
            if((*this)(current).getType() == WALL)
            {
                if(j < this->width - 1 && (*this)(right).getType() == WALL)
                    G.insertEdge(current,right);

                if(i < this->height - 1 && (*this)(down).getType() == WALL)
                    G.insertEdge(current,down);
            }
        }
}

void Map::link(const Room& R,const Room& Q,Graph& G)
{
    Point p = R.pickAPointAround();
    Point q = Q.pickAPointAround();
    Point p2;   // A point adjacent to p in the map and which is in the graph
    Point q2;
    list<Point> steps;
    unordered_map<Point,Point> T(6143);

    connectToMap(G,p,p2);
    connectToMap(G,q,q2);

    shortestPath(G,p,T);
    retrievePath(steps,T,p,q);

    for(Point p : steps)
        (*this)(p).setType(PAVEMENT);

    G.deletePoint(p);
    G.deletePoint(q);
}

void Map::connectToMap(Graph& G, Point& p, Point& q)
{
    G.insertPoint(p);
    if(isWithinRoom(p.x-1,p.y))  // The room is to the left of p
        q = {p.x+1,p.y};
    else if(isWithinRoom(p.x+1,p.y)) // The room is to the right of p
        q = {p.x-1,p.y};
    else if(isWithinRoom(p.x,p.y-1)) // And so on
        q = {p.x,p.y+1};
    else if(isWithinRoom(p.x,p.y+1))
        q = {p.x,p.y-1};
    G.insertEdge(p,q);
}

void Map::freeSpots(int n,unordered_set<Point>& spots,int r)
{
    int i = 0;
    int permutation[n];
    unordered_map<string,Room>::iterator it;
    if(n < this->rooms.size())
        generateKPermutation(permutation,0,this->rooms.size()-1,n);

    it = this->rooms.begin();
    while(i < n)
    {
        if(n < this->rooms.size())
        {
            it = this->rooms.begin();
            for(int j = 0; j < permutation[i]; j++)
                ++it;
        }

        for(int k = 0; k < r; k++)
        {
            Point position;

            do
            {
                position = freeSpot((*it).second);
            }
            while(spots.find(position) != spots.end());
            spots.insert(position);
            i++;
        }
        if(n >= this->rooms.size())
            ++it;
    }
}

std::string Map::placeCharacter(PlayableCharacter& player)
{
    Room R = pickRoom();
    Point p;
    int x = R.getCorner().x;
    int y = R.getCorner().y;
    int height = R.getHeight();
    int width = R.getWidth();
    do
    {
        p = Point(rand(x+2,x+width-3),rand(y+2,y+height-3)); // the player can't spawn near an exit
    }
    while((*this)(p.x,p.y).getUpperLayer() != "");

    player.setPosition(p.x,p.y);

    return R.getId();
}

void Map::generateRooms(int n)
{
    Area A({1,1},width-1,height-1);
    unordered_set<Area> areas,toRemove,toInsert;
    bool vertical_split = true;
    int i = 1, roomId = 0, tries = 0;
    string id;
    

    areas.insert(A);
    while(i < n && tries < 500)
    {
        toRemove.clear();
        toInsert.clear();
        for(Area A : areas)
        {
            if(i < n) 
            {
                bool split = false;
                Area A1,A2;
                if ((vertical_split && (A.getWidth()/3 > 10)) || (!vertical_split && (A.getHeight()/3 > 10)))
                {
                    A.split(A1,A2,vertical_split);
                    split = true;
                }
                else if ((vertical_split && (A.getWidth()/2 > 10)) || (!vertical_split && (A.getHeight()/2 > 10)))
                {
                    A.splitInHalf(A1,A2,vertical_split);
                    split = true;
                }
                if(split)
                {
                    toInsert.insert(A1);
                    toInsert.insert(A2);
                    i++;
                    toRemove.insert(A);
                }
                tries++;
            }
        }
        for(Area A : toRemove)
            areas.erase(A);
        for(Area A : toInsert)
            areas.insert(A);
        vertical_split = !vertical_split;
    }
    // Generate rooms for each area
    for(Area A : areas)
    {
        id = "room" + to_string(roomId);
        roomId++;
        Room R = generateRoom(A,id);
        rooms[id] = R;
        place(R);
    }
}
// Given a number n, it generates n rooms

Point Map::freeSpot(Room R)
{
    int x = R.getCorner().x;
    int y = R.getCorner().y;
    int height = R.getHeight();
    int width = R.getWidth();
    Point p;

    do
    {
        p = Point(rand(x+1,x+width-2),rand(y+1,y+height-2));
    }
    while((*this)(p.x,p.y).getUpperLayer() != "");

    return p;
}

Point Map::placeStairs(tile_t type, int x, int y)
{
    Point p = {x,y};
    if(type == UP_STAIRS)
    {
        Room R = pickRoom();
        int x2 = R.getCorner().x;
        int y2 = R.getCorner().y;
        int height = R.getHeight();
        int width = R.getWidth();
        do
        {
            p = {rand(x2+2,x2+width-3),rand(y2+2,y2+height-3)}; // the stairs can't be near an ex2it
        }
        while((*this)(p.x,p.y).getUpperLayer() != "");
        (*this)(p.x,p.y).setType(type);
    }
    else if (type == DOWN_STAIRS && x != -1 && y != -1)
        (*this)(x,y).setType(type);
    return p;
}

bool Map::movePlayer(PlayableCharacter& player, int c)
{
    bool moved = false;
    int x = player.getPosition().x, y = player.getPosition().y;
    switch(c)
    {
        case 'k':
        case KEY_UP:
            if((*this)(x,y-1).isWalkable())
            {
                y = y - 1;
                moved = true;
            }
            break;
        case 'j':
        case KEY_DOWN:
            if((*this)(x,y + 1).isWalkable()) 
            {
                y = y + 1;
                moved = true;
            }
            break;
        case 'h':
        case KEY_LEFT:
            if((*this)(x - 1,y).isWalkable()) 
            {
                x = x - 1;
                moved = true;
            }
            break;
        case 'l':
        case KEY_RIGHT:
            if((*this)(x + 1,y).isWalkable()) 
            {
                x = x + 1;
                moved = true;
            }
            break;
    }
    
    player.setPosition(x,y);
    
    return moved;
}

bool Map::isWithinRoom(int x, int y)
{
    bool in;

    if((*this)(x,y).getId() == "")
        in = false;
    else
    {
        if(rooms[(*this)(x,y).getId()].isBorder(x,y))
            in = false;
        else
            in = true;
    }

    return in;
}
