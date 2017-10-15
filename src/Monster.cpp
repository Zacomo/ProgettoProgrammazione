//
// Created by zacomo on 20/09/17.
//

#include "Monster.h"

using namespace std;

Monster::Monster() {
    awake = false;
    this->LP = -1;
    this->DEF = -1;
    this->ATK = -1;
    Name = "";
    awake = false;
}

Monster::Monster(string Name, int LV) {
    if (LV < 1)
        LV = 1; //il livello non può essere negativo

    this->LV = LV;
    this->Name = Name;
    int atk = 0, def = 0, lp = 1;
    awake = false;

    if (Name == "Goblin"){  //  Mostri diversi statistiche e simboli diversi
        atk = 2 * (LV +1 );
        def = 1;
        lp = LV;
        this->symbol = 'G';
    }

    if (Name == "Troll"){
        if ((3 + LV) > 50) //  I mostri hanno punti difesa di 50 massimo
            def = 50;
        else
            def = 3 +LV;

        atk = 3 + LV;
        lp = 3 + LV;
        this->symbol = 'T';
    }

    if (Name == "Golem"){
        if ((2 * (LV + 1)) > 50)
            def = 50;
        else
            def = 2 * (LV + 1);

        atk = 2 + LV;
        lp = 2 + LV;
        this->symbol = 'O';
    }

    if (Name == "Gineppino"){
        if ((5 * LV) > 50)
            def = 50;
        else
            def = 5 * LV;

        atk = 3 * LV;
        lp = 5 * LV;
        this->symbol = 'P';
    }

    this->ATK = atk;
    this->DEF = def;
    this->LP = lp;
}

int Monster::getLP(){
    return this->LP;
}

int Monster::getDEF(){
    return this->DEF;
}

int Monster::getDEFMAX(){
    return this->DEFMAX;
}

int Monster::getATK(){
    return this->ATK;
}

Point Monster::getPosition() {
    return this->POS;
}

int Monster::getLV(){
    return this->LV;
}

string Monster::getId(){
    return this->id;
}

void Monster::setLP(int LP){
    this->LP = LP;
}

void Monster::setDEF(int DEF){
    this->DEF = DEF;
}

void Monster::setATK(int ATK){
    this->ATK = ATK;
}

void Monster::setPosition(int x, int y){
    this->POS.x = x;
    this->POS.y = y;
}

char Monster::getSymbol(){
    return this->symbol;
}

void Monster::setId(string id){
    this->id = id;
}

string Monster::getName(){
    return this->Name;
}

bool Monster::isAwake(){
    return this->awake;
}

void Monster::wakeUp(bool b){
    this->awake = b;
}

void Monster::move(PlayableCharacter& pg, Map map){

    if (awake && (POS.x != pg.getPosition().x-1 || POS.y != pg.getPosition().y-1)){

        int distance[4];
        int i = 0;
        int dist = -1;
        Point tmppos;

        dist = w(pg.getPosition(), this->POS);

        for (int i = 0; i < 4; i++) {

            tmppos.x = -1;
            tmppos.y = -1;

            switch (i){
                case 0:
                    tmppos.x = this->POS.x + 1;
                    tmppos.y = this->POS.y;
                    if ((w(tmppos, pg.getPosition()) < dist) && map(tmppos).isWalkable())
                        POS = tmppos;
                    break;
                case 1:
                    tmppos.x = this->POS.x - 1;
                    tmppos.y = this->POS.y;
                    if ((w(tmppos, pg.getPosition()) < dist) && map(tmppos).isWalkable())
                        POS = tmppos;
                    break;
                case 2:
                    tmppos.x = this->POS.x;
                    tmppos.y = this->POS.y + 1;
                    if ((w(tmppos, pg.getPosition()) < dist) && map(tmppos).isWalkable())
                        POS = tmppos;
                    break;
                case 3:
                    tmppos.x = this->POS.x;
                    tmppos.y = this->POS.y - 1;
                    if ((w(tmppos, pg.getPosition()) < dist) && map(tmppos).isWalkable())
                        POS = tmppos;
                    break;
            }
        }
    }

}

bool operator==(Monster m1,Monster m2){
    m1.id == m2.id;
}

bool operator!=(Monster m1,Monster m2){
    m1.id != m2.id;
}

namespace std
{
    size_t hash<Monster>::operator()(Monster m) const
    {
        return hash<string>()(m.getName());
    }
    // Hash function for the class Monster
}
