//
// Created by zacomo on 19/09/17.
//
//
// Created by zacomo
//


#include <unordered_set>

#include "personaggiGiocabili.h"


    PlayableCharacter::PlayableCharacter(double LP, double MP, double DEF, double ATK, std::string Name, int x, int y){
        this->LP = LP;
        this->MP = MP;
        this->DEF = DEF;
        this->ATK = ATK;
        this->POS.X = x;
        this->POS.Y = y;
        this->LV = 0;
        this->Coins = 0;
        this->Name = Name;
        for (int i = 0; i<6; i++){
            equipment [i] = Item();      //Inizializza equipment
        }
    }

    double PlayableCharacter::getLP(){
        return this->LP;
    }

    double PlayableCharacter::getMP(){
        return this->MP;
    }

    double PlayableCharacter::getDEF(){
        return this->DEF;
    }

    double PlayableCharacter::getATK(){
        return this->ATK;
    }

    Position PlayableCharacter::getPosition() {
        return this->POS;
    }

    int PlayableCharacter::getLV(){
        return this->LV;
    }

    int PlayableCharacter::getCoins(){
        return this->Coins;
    }

    void PlayableCharacter::setLP(double LP){
        this->LP = LP;
    }

    void PlayableCharacter::setMP(double MP){
        this->MP = MP;
    }

    void PlayableCharacter::setDEF(double DEF){
        this->DEF = DEF;
    }

    void PlayableCharacter::setATK(double ATK){
        this->ATK = ATK;
    }

    void PlayableCharacter::setPosition(double x, double y){
        this->POS.X = x;
        this->POS.Y = y;
    }

    void PlayableCharacter::setCoins(int Coins){
        this->Coins = Coins;
    }

    std::string PlayableCharacter::getName(){
        return this->Name;
    }

    bool PlayableCharacter::useConsumable(Item sbobba){
        if (sbobba.gettype() == 6){
            LP = LP + sbobba.getLP();
            MP = MP + sbobba.getMP();
            DEF = DEF + sbobba.getDEF();
            ATK = ATK + sbobba.getATK();
            Inventory.erase(Inventory.find(sbobba));           //consumabile utilizzabile una sola volta
            return true;
        }
        return false;
    }

    bool PlayableCharacter::equip(Item ferraglia){                       //
        if ((ferraglia.gettype() > -1 && ferraglia.gettype() < 6) && (equipment[ferraglia.gettype()] == Item())){

            equipment[ferraglia.gettype()] = ferraglia;
            LP = LP + ferraglia.getLP();
            MP = MP + ferraglia.getMP();
            DEF = DEF + ferraglia.getDEF();
            ATK = ATK + ferraglia.getATK();
            return true;
        }
        return false;
    }

    bool PlayableCharacter::unequip(Item ferraglia){
        if (equipment[ferraglia.gettype()] != Item()){
            LP = LP - ferraglia.getLP();
            MP = MP - ferraglia.getMP();
            DEF = DEF - ferraglia.getDEF();
            ATK = ATK - ferraglia.getATK();
            equipment[ferraglia.gettype()] = Item(); //Rimuove l'oggetto dall'equip.
            return true;
        }

        return false;
    }


    bool PlayableCharacter::getItem(Item thing){
        bool done = false;
        if (Inventory.size() < 20) {     //20 è la dimensione dell'inventario
            Inventory.insert(thing);
            done = true;
        }
        return done;
    }

    bool PlayableCharacter::dropItem(Item thing){
        bool done = false;
        if (!Inventory.empty()) {

            Inventory.erase(thing);
            done = true;
        }
        return done;
    }

    void PlayableCharacter::LVLup() {
        this->LV++;
        double atk = 0, def = 0, lp = 0, mp = 0;
        if (this->Name == "Gaudenzio"){
            atk = 2;
            def = 3;
            lp = 2;
    }
        if (this->Name == "Peppino"){
            atk = 1;
            def = 1;
            lp = 1;
            mp = 3;
        }

        if (this->Name == "Badore"){
            atk = 3;
            mp = 1;
            lp = 1;
        }
        this->ATK += atk;
        this->DEF += def;
        this->MP  += mp ;
        this->LP  += lp ;

};




