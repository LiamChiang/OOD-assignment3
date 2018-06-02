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
    GameMemento* getState(int index){
//        qDebug() << mementoList[index];
        return mementoList[index];
    }
};

#endif // GAMESTATECARETAKER_H
