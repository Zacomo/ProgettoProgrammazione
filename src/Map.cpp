#include "../include/Map.h"
#include "../include/utility.h"
#include <unordered_set>
#ifdef DEBUG
#include <cstdlib>
#include <iostream>
#include <fstream>
#endif

using namespace std;

Map::Map(int width, int height) : grid(height, width), width(width), height(height),
    rooms(47),itemsLayer(height,width),monstersLayer(height,width)
{}
// Creates a new map with the given # of rows and columns

Tile& Map::operator () (int x, int y)
{
    return grid(y,x);
}
// Returns a reference variable to Map(x,y)

Tile& Map::operator () (Point p)
{
    return grid(p.y,p.x);
}
// Returns a reference variable to Map(x,y)

int Map::getWidth()
{
    return this->width;
}

int Map::getHeight()
{
    return this->height;
}

void Map::place(Room& R)
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

bool Map::overlaps(Room& R)
{
    Point p = R.getCorner();
    int width = R.getWidth();
    int height = R.getHeight();
    tile_t type = (*this)(p.x,p.y).getType();
    bool corner = type == PAVEMENT || type == ROOM_BORDER;  // the corner overlaps another room
    if(p.x > 0)
        corner = corner || (*this)(p.x - 1,p.y).getType() == ROOM_BORDER; // the corner is adjacent to a room which is to its left
    if(p.y > 0)
        corner = corner || (*this)(p.x,p.y - 1).getType() == ROOM_BORDER; // the corner is adjacent to a room which is above it
    if(corner)
        return true;
    else
    {
        for(int i = p.x; i < p.x + width; i++)
        {
            bool up,down;
            up = (*this)(i,p.y).getType() == PAVEMENT || (*this)(i,p.y).getType() == ROOM_BORDER; // The block overlaps another room
            if(p.y > 0)
                up = up || (*this)(i,p.y - 1).getType() == ROOM_BORDER;   // The block is adjacent to a room which is above it
            down = (*this)(i,p.y + height - 1).getType() == PAVEMENT || (*this)(i,p.y + height - 1).getType() == ROOM_BORDER;
            if(p.y + height < this->height)
                down = down || (*this)(i,p.y + height).getType() == ROOM_BORDER; // The block is adjacent to a room which is under it
            if(up || down)
                return true;
        }

        for(int i = p.y; i < p.y + height; i++)
        {
            bool left,right;
            left = (*this)(p.x,i).getType() == PAVEMENT || (*this)(p.x,i).getType() == ROOM_BORDER;
            if(p.x > 0)
                left = left || (*this)(p.x - 1,i).getType() == ROOM_BORDER; // The block is adjacent to a room which is to its left
            right = (*this)(p.x + width - 1,i).getType() == PAVEMENT || (*this)(p.x + width - 1,i).getType() == ROOM_BORDER;
            if(p.x + width < this->width)
                right = right || (*this)(p.x + width,i).getType() == ROOM_BORDER; // The block is adjacent to a room which is to its right
            if(left || right)
                return true;
        }

        /*
        for(int i = p.x; i < p.x + width; i++)
            for(int j = p.y; j < p.y + height; j++)
                if((*this)(i,j).getType() == PAVEMENT || (*this)(i,j).getType() == ROOM_BORDER)
                    return true;
                    */
        return false;
    }

}

void Map::addRoom(Room& R,string id)
{
    rooms[id] = R;
}

Room Map::pickRoom()
{
    int index = rand(0,rooms.size()-1);
    unordered_map<string,Room>::iterator it = rooms.begin();

    while(index > 0)
    {
        ++it;
        --index;
    }

    return (*it).second;
}

void Map::setVisible(string id)
{
    rooms[id].setVisible(true);
    Point p = rooms[id].getCorner();    
    int width = rooms[id].getWidth();
    int height = rooms[id].getHeight();
    for(int i = p.x; i < p.x + width; i++)
        for(int j = p.y; j < p.y + height; j++)
            (*this)(i,j).setVisible(true);
    // if (!monstersLayer.isEmpty(j,i))
    //      monstersLayer(j,i).wakeUp();
}

void Map::showAround(int x, int y)
{
    (*this)(x + 1,y).setVisible(true);
    (*this)(x - 1,y).setVisible(true);
    (*this)(x,y + 1).setVisible(true);
    (*this)(x,y - 1).setVisible(true);
}

void Map::generate(int requiredRooms)
{
    unordered_map<string,Room>::iterator it;
    int roomID = 0;
    Room R,Q;
    Graph dots;

    for(int i = 0; i < width; i++)        // Map initialization
        for(int j = 0; j < height; j++)
        {
            (*this)(i,j).setType(WALL);
            (*this)(i,j).setVisible(false);
        }

    generateRooms(requiredRooms);
    populateGraph(dots);
    createLinks(dots);

    it = rooms.begin();
    R = (*it).second;
    ++it;
    while(it != rooms.end())
    {
        Q = (*it).second;
        ++it;
        link(R,Q,dots);
        R = Q;
    }
}

Room Map::generateRoom(Area A,string id)
{
    Point p;
    int x = A.getCorner().x;
    int y = A.getCorner().y;
    int width = A.getWidth();
    int height = A.getHeight();
    int wMax, hMax;
    int w,h;

    p.x = rand(x + 1, x + width - 2 - 7); // a room has a minimum size of 7x7 and a max size of 20x20
    p.y = rand(y + 1, y + height - 2 - 7); 
    
    int freeXSpace = x + width - 2 - p.x;
    int freeYSpace = y + height - 2 - p.y;

    if(20 < freeXSpace)
        wMax = 20;
    else 
        wMax = freeXSpace;

    if(20 < freeYSpace)
        hMax = 20;
    else 
        hMax = freeYSpace;

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
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            Point current = {j, i};
            Point right = {j + 1, i};
            Point down = {j, i + 1};
            if((*this)(current).getType() == WALL)
            {
                if(j < width - 1 && (*this)(right).getType() == WALL)
                    G.insertEdge(current,right);

                if(i < height - 1 && (*this)(down).getType() == WALL)
                    G.insertEdge(current,down);
            }
        }
}

void Map::link(Room& R,Room& Q,Graph& G)
{
    Point p = R.pickAPointAround();
    Point q = Q.pickAPointAround();
    Point toRemove1;
    Point toRemove2;
    list<Point> steps;
    unordered_map<Point,Point> T(6143);

    connectToMap(G,p,toRemove1);
    connectToMap(G,q,toRemove2);

    if(p != q)
    {
        shortestPath(G,p,T);
        retrievePath(steps,T,p,q);
    }
    else
        steps.insert(steps.begin(),p);

    for(Point p : steps)
    {
        if(p != Point())
            (*this)(p).setType(HALLWAY);
    }

    disconnectFromMap(G,p,toRemove1);
    disconnectFromMap(G,q,toRemove2);
}

void Map::connectToMap(Graph& G, Point& p, Point& q)
{
    G.insertPoint(p);
    if((*this)(p).getType() == ROOM_BORDER)
    {
        if((*this)(p.x - 1, p.y).getType() == WALL || (*this)(p.x - 1, p.y).getType() == HALLWAY)
            q = {p.x - 1,p.y};
        else if((*this)(p.x + 1, p.y).getType() == WALL || (*this)(p.x + 1, p.y).getType() == HALLWAY)
            q = {p.x + 1,p.y};
        else if((*this)(p.x, p.y - 1).getType() == WALL || (*this)(p.x, p.y - 1).getType() == HALLWAY)
            q = {p.x,p.y - 1};
        else if((*this)(p.x, p.y + 1).getType() == WALL || (*this)(p.x, p.y + 1).getType() == HALLWAY)
            q = {p.x,p.y + 1};
        G.insertPoint(q);
        G.insertEdge(p,q);
    }
}

void Map::disconnectFromMap(Graph& G, Point& p, Point& q)
{
    G.deleteEdge(p,q);
    G.deletePoint(p);
    G.deletePoint(q);
}

void Map::freeSpots(int n,unordered_set<Point>& spots,int r)
{
    unordered_map<string,Room>::iterator it;
    int permutation[n];
    generateKPermutation(permutation,0,rooms.size()-1,n);

    for(int i = 0; i < n; i++) 
    {
        it = rooms.begin();
        for(int j = 0; j < permutation[i]; j++)
            ++it;

        for(int k = 0; k < r; k++)
        {
            int x = (*it).second.getCorner().x;
            int y = (*it).second.getCorner().y;
            int height = (*it).second.getHeight();
            int width = (*it).second.getWidth();
            Point position;

            do
            {
                position = Point(rand(x+1,x+width-2),rand(y+1,y+height-2)); // Rememeber to implement the printing of the
                // map to hide an object if a monster is on it
            }
            while(!itemsLayer.isEmpty(position.y,position.x) && !monstersLayer.isEmpty(position.y,position.x) &&
                    (spots.find(position) == spots.end()));
            spots.insert(position);
        }
    }
}

void Map::placeCharacter(PlayableCharacter& player)
{
    Room R = pickRoom();
    setVisible(R.getId());
    Point p;
    int x = R.getCorner().x;
    int y = R.getCorner().y;
    int height = R.getHeight();
    int width = R.getWidth();
    do
    {
                p = Point(rand(x+1,x+width-2),rand(y+1,y+height-2)); // Rememeber to implement the printing of the
    }
    while(!itemsLayer.isEmpty(p.y,p.x) && !monstersLayer.isEmpty(p.y,p.x));

    player.setPosition(p.x,p.y);
}

void Map::placeItem(Item i)
{
    Point p = i.getPosition();
    itemsLayer(p.y,p.x) = i;
}
// Given an items and its position

void Map::placeMonster(Monster& m)
{
    Point p = m.getPosition();
    monstersLayer(p.y,p.x) = m;
}

void Map::generateRooms(int n)
{
    Area A({0,0},width,height);
    unordered_set<Area> areas,toRemove,toInsert;
    bool vertical = true;
    int i = 1;
    int roomId = 0;
    string id;
    int tries = 0;

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
                if ((vertical && (A.getWidth()/3 > 10)) || (!vertical && (A.getHeight()/3 > 10)))
                {
                    A.split(A1,A2,vertical);
                    split = true;
                }
                else if ((vertical && (A.getWidth()/2 > 10)) || (!vertical && (A.getHeight()/2 > 10)))
                {
                    A.splitInHalf(A1,A2,vertical);
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
        vertical = !vertical;
    }
    // Generate rooms for each area
    for(Area A : areas)
    {
        id = "room" + to_string(roomId);
        roomId++;
        Room R = generateRoom(A,id);
        addRoom(R,id);
        place(R);
    }
}
// Given a number n, it generates n rooms