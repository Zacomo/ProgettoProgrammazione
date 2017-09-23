//
// Created by zacomo on 19/09/17.
//

#ifndef GAME_ITEM_H
#define GAME_ITEM_H

#include <string>
#include <vector>
#include "Graph.h"

class Item {
    protected:
        std::string name;
        int type;           //0-3 Armatura 4-5 Arma/Scudo 6 Consumabile
        int LP,MP,DEF,ATK;
        char symbol;
        Point position;
    public:
        Item();
        //  Costruttore di default

        Item(std::string name, int type, int LP, int MP, int DEF, int ATK);
        //  Dati un nome, un intero e una quantità di LP, MP, DEF e ATK, crea un nuovo Item

        std::string getName();
        // Restituisce il nome dell'Item

        int getType();
        // Restituisce un intero che indica il tipo di Item

        int getLP();
        //  Restituisce i punti vita dell'Item

        int getMP();
        //  Restituisce i punti mana dell'Item

        int getDEF();
        //  Restituisce i punti difesa dell'Item

        int getATK();
        //  Restituisce i punti di attacco dell'Item

        char getSymbol();
        //  Restituisce il simbolo che indica l'Item ('?')

        Point getPosition();
        //  Restituisce la posizione dell'Item

        Point setPosition(int x, int y);
        //  Modifica la posizione dell'Item

        friend bool operator ==(Item i1, Item i2);
        //  Definisce l'operatore '==' per la classe Item

        friend bool operator !=(Item i1, Item i2);
        //  Definisce l'operatore '!=' per la classe Item
};

namespace std
{
    template <> struct hash<Item>
    {
        size_t operator()(Item i) const;
        // Hash function for the class Item
    };
}


void retrieveItems(std::ifstream& file, std::vector<Item>& set);
// Given a file with the items and a vector of Items, it retireves them and stores them in the vector
#endif //GAME_ITEM_H
