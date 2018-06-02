#ifndef GAMESTATECARETAKER_H
#define GAMESTATECARETAKER_H

#include <QDebug>
#include <vector>

#include "gamememento.h"

class GameStateCareTaker{

    std::vector<GameMemento*> mementoList;
public:
    void add(GameMemento* state){
        mementoList.push_back(state);
    }
    std::vector<GameMemento*> getMementoList(){return mementoList;}

    GameMemento* getState(int index){
        return mementoList[index];
    }
};

#endif // GAMESTATECARETAKER_H
