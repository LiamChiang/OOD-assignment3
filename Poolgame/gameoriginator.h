#ifndef GAMECALLER_H
#define GAMECALLER_H

#include <QDebug>

#include "ball.h"
#include "gamememento.h"

//originator
class GameOriginator
{
public:
    GameOriginator():m_state(nullptr){}

    void set(std::vector<Ball*>* state){
        m_state = new std::vector<Ball*>;
        *m_state = *state;
    }

    std::vector<Ball*>* getBallState(){
        return m_state;
    }

    GameMemento* saveToMemento(){
        return new GameMemento(m_state);
    }

    void restoreFromMemento(GameMemento* memento){
        m_state = memento->getBallState();
    }

private:
    std::vector<Ball*>* m_state;
};

#endif // GAMECALLER_H
