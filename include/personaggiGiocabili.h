//
// Created by zacomo
//

#ifndef PERSONAGGI_GIOCABILI_H
#define PERSONAGGI_GIOCABILI_H

#include <unordered_set>
#include <string>
#include "Item.h"
#include "Graph.h"

class PlayableCharacter{
private:
    int LP, MP, DEF, ATK;
    int LPMAX, MPMAX;
    int LV, Coins;
    Point POS;
    std::string Name;
    std::unordered_multiset<Item> Inventory;
    Item equipment [6];

public:

    PlayableCharacter();
    // Costruttore di default

    PlayableCharacter(int LP, int MP, int DEF, int ATK, std::string Name);
    //  Dati una quantità di LP, MP, DEF, ATK e un nome, crea un nuovo PG.
    
    int getLP();
    //  Restituisce i punti vita

    int getLPMAX();
    //  Restituisce i punti vita max
    
    int getMP();
    //  Restituisce i punti mana
    
    int getMPMAX();
    //  Restituisce i punti mana max
    
    int getDEF();
    //  Restituisce i punti difesa
    
    int getATK();
    //  Restituisce i punti attacco
    
    Point getPosition();
    //  Restituisce la posizione del PG
    
    int getLV();
    //  Restituisce il livello del PG
    
    int getCoins();
    //  Restituisce le monete possedute dal PG

    void setLP(int LP);
    //  Modifica i punti vita del PG
    
    void setMP(int MP);
    //  Modifica i punti mana
    
    void setDEF(int DEF);
    //  Modifica i punti difesa
    
    void setATK(int ATK);
    //  Modifica i punti attacco
    
    void setPosition(int x, int y);
    //  Date le coordinate x y, modifica la posizione del PG
    
    void setCoins(int Coins);
    //  Modifica la quantità di monete del PG

    std::string getName();
    //  Restituisce il nome del PG

    bool useConsumable(Item sbobba);
    //  Dato un Item consumabile, se questo è presente nell'inventario
    //  viene utilizzato, modificando le statistiche del PG.
    //  Restituisce true se è stato usato, false altrimenti.

    bool equip(Item ferraglia);
    //  Dato un Item equipaggiabile, se questo è presente nell'inventario
    //  viene equipaggiato, modificando le statistiche del PG.
    //  Restituisce true se equipaggiato, false altrimenti.

    bool unequip(Item ferraglia);
    //  Dato un Item equipaggiabile, se questo è equipaggiato,
    //  viene disequipaggiato.
    //  Restituisce true se è stato disequipaggiato, false altrimenti

    bool pickItem(Item thing);
    //  Dato un Item questo viene aggiunto all'inventario del PG
    //  Restituisce true se è stato aggiunto, false altrimenti

    bool dropItem(Item thing);
    //  Dato un Item, se è presente nell'invetario del PG
    //  allora questo viene scartato.
    //  Restituisce true se è stato scartato, false altrimenti.

    void LVLup();
    //  Increment il livello del PG di 1, modificando le statistiche
    // (LP, LPMAX, MP, MPMAX, etc..) di conseguenza
};

#endif //PERSONAGGI_GIOCABILI_H
