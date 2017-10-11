//
// Created by zacomo on 19/09/17.
//
//
// Created by zacomo
//


#include <unordered_set>

#include "../include/personaggiGiocabili.h"


    PlayableCharacter::PlayableCharacter(){

        this->LP = -1;
        this->MP = -1;
        this->DEF = -1;
        this->ATK = -1;
        this->Name = "";
        this->LUCK = -1;

        for (int i = 0; i < 5; i++)
            equipment[i] = Item();
    }

    PlayableCharacter::PlayableCharacter(int LP, int MP, int DEF, int ATK, int LUCK, std::string Name){
        this->LP = LP;
        this->LPMAX = LP;
        this->MP = MP;
        this->MPMAX = MP;
        this->DEF = DEF;
        this->DEFMAX = DEF;
        this->ATK = ATK;
        this->LUCK = LUCK;
        this->LV = 0;
        this->Coins = 0;
        this->Name = Name;
        for (int i = 0; i<5; i++){
            equipment [i] = Item();      //Inizializza il vettore di Item "equipment"
        }
    }

    int PlayableCharacter::getLP(){
        return this->LP;
    }

    int PlayableCharacter::getLPMAX() {
        return this->LPMAX;
    }

    int PlayableCharacter::getMP(){
        return this->MP;
    }

    int PlayableCharacter::getMPMAX() {
        return this->MPMAX;
    }

    int PlayableCharacter::getDEF(){
        return this->DEF;
    }

    int PlayableCharacter::getDEFMAX() {
        return this->DEFMAX;
    }

    int PlayableCharacter::getATK(){
        return this->ATK;
    }

    Point PlayableCharacter::getPosition() {
        return this->POS;
    }

    int PlayableCharacter::getLuck(){
        return this->LUCK;
    }

    int PlayableCharacter::getLuckMAX() {
        return this->LUCKMAX;
    }

    int PlayableCharacter::getLV(){
        return this->LV;
    }

    int PlayableCharacter::getCoins(){
        return this->Coins;
    }

    void PlayableCharacter::setLP(int LP){
        this->LP = LP;
    }

    void PlayableCharacter::setMP(int MP){
        this->MP = MP;
    }

    void PlayableCharacter::setDEF(int DEF){
        this->DEF = DEF;
    }

    void PlayableCharacter::setATK(int ATK){
        this->ATK = ATK;
    }

    void PlayableCharacter::setPosition(int x, int y){
        this->POS.x = x;
        this->POS.y = y;
    }

    void PlayableCharacter::addCoins(int x) { //controllare che il saldo non vada sotto zero
        this->Coins += x;
    }

    void PlayableCharacter::setCoins(int Coins){
        this->Coins = Coins;
    }

    std::string PlayableCharacter::getName(){
        return this->Name;
    }

    bool PlayableCharacter::useConsumable(Item sbobba){
        if (sbobba.getType() == 5){ //  I consumabili sono item con campo Type pari a 5
            if (LPMAX >= LP + sbobba.getLP())
                LP = LP + sbobba.getLP();
            else
                LP = LPMAX;

            if (MPMAX >= MP + sbobba.getMP())
                MP = MP + sbobba.getMP();
            else
                MP = MPMAX;

            if (DEFMAX >= DEF + sbobba.getDEF())
                DEF = DEFMAX;
            else
                DEF = DEF + sbobba.getDEF();

            if (LUCKMAX >= LUCK + sbobba.getLuck())
                LUCK = LUCKMAX;
            else
                LUCK = LUCK + sbobba.getLuck();

            ATK = ATK + sbobba.getATK();
            Inventory.erase(Inventory.find(sbobba));    //  Consumabile utilizzabile una sola volta, quindi rimosso poi

            return true;
        }
        return false;
    }

    bool PlayableCharacter::equip(Item ferraglia){  //  Item equipaggiabili hanno Type da 0 a 5 compresi
        if ((ferraglia.getType() > -1 && ferraglia.getType() < 5) && (equipment[ferraglia.getType()] == Item())){

            equipment[ferraglia.getType()] = ferraglia; //  L'Item va ad occupare la sua posizione nel vettore equip.

            DEF = DEF + ferraglia.getDEF();

            ATK = ATK + ferraglia.getATK();

            return true;
        }
        return false;
    }

    bool PlayableCharacter::unequip(Item ferraglia){
        if (equipment[ferraglia.getType()] != Item()){  //Controllo per vedere se un oggetto di quel tipo è nell'equip.
            DEF = DEF - ferraglia.getDEF();
            ATK = ATK - ferraglia.getATK();
            equipment[ferraglia.getType()] = Item(); //Rimuove l'oggetto dall'equip.
            return true;
        }

        return false;
    }


    bool PlayableCharacter::pickItem(Item thing){
        bool done = false;
        if (Inventory.size() < 20) {    // 20 è la dimensione dell'inventario
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
        int atk = 0, def = 0, lp = 0, mp = 0, luck = 0;
        if (this->Name == "Gaudenzio"){ //  Ogni PG Incrementa le statistiche in modo diverso
            atk = 2;
            def = 3;
            lp = 2;
            luck = 1;
    }
        if (this->Name == "Peppino"){
            atk = 1;
            def = 1;
            lp = 1;
            mp = 3;
            luck = 1;
        }

        if (this->Name == "Badore"){
            atk = 2;
            mp = 1;
            lp = 1;
            luck = 3;
        }

        if ((this->DEFMAX += def) > 90)     //La Difesa massima può arrivare fino a 90 (a 100 il pg è invincibile)
            this->DEFMAX = 90;
        else
            this->DEFMAX += def;

        if ((this->LUCKMAX += luck) > 30)    //La fortuna masssima può arrivare fino a 30
            this->LUCKMAX = 30;
        else
            this->LUCKMAX += luck;

        this->ATK += atk;
        this->DEF = DEFMAX;
        this->MPMAX += mp ;
        this->LPMAX += lp ;
        this->LP = LPMAX;   //  Quando il PG sale di livello LP, MP, DEF e LUCK vengono portati al loro valore massimo.
        this->MP = MPMAX;
        this->LUCK = LUCKMAX;
};

Item PlayableCharacter::getEquipmentAt(int index) {
    return equipment[index];
}
