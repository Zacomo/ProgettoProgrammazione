//
// Created by zacomo on 20/09/17.
//

#ifndef GAME_MONSTER_H
#define GAME_MONSTER_H
#include <string>
#include "Graph.h"
#include "personaggiGiocabili.h"
#include "Level.h"
class Monster {
    private:
        int LP, DEF, ATK;
        int DEFMAX;
        int LV;
        Point POS;
        std::string Name, id;
        char symbol;
        bool awake;

    public:

    Monster();
    //  Costruttore di default

    Monster(std::string Name, int LV);
    //  Dati un nome e un intero che indica il livello, restituisce un mostro

    int getLP();
    //  Restituisce i punti vita del mostro

    int getDEF();
    //  Restituisce i punti difesa del mostro

    int getDEFMAX();
    //  Restituisce i punti difesa massimi del mostro

    int getATK();
    //  Restituisce i punti di attacco del mostro

    Point getPosition();
    //  Restituisce la posizione del mostro

    int getLV();
    //  Restituisce il livello del mostro

    std::string getId();
    //  Restituisce l'ID del mostro

    void setLP(int LP);
    //  Modifica i punti vita del mostro

    void setDEF(int DEF);
    //  Modifica i punti difesa del mostro

    void setATK(int ATK);
    //  Modifica i punti attacco del mostro

    void setPosition(int x, int y);
    //  Date due coordinate x y, modifica la posizione del mostro

    void setId(std::string id);
    //  Dato una stringa questa viene assegnata come ID del mostro

    std::string getName();
    //  Restituisce il nome del mostro

    char getSymbol();
    //  Restituisce il simbolo che indica il mostro

    bool isAwake();
    //  Restituisce true se il mostro è sveglio, false altrimenti

    void wakeUp(bool b);
    //  Dato un booleano setta il campo awake di conseguenza (sveglia o fa dormire il mostro)

    friend bool operator==(Monster m1,Monster m2);
    //  Definisce l'operatore "==" per la classe Monster

    friend bool operator!=(Monster m1,Monster m2);
    //  Definisce l'operatore "!=" per la classe Monster
};

namespace std
{
    template <> struct hash<Monster>
    {
        size_t operator()(Monster m) const;
        // Hash function for the class Monster
    };

}


#endif //GAME_MONSTER_H
