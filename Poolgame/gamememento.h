#ifndef GAMEMEMENTO_H
#define GAMEMEMENTO_H
#include "ball.h"

class GameMemento
{

public:
    GameMemento(std::vector<Ball*>* ballstate)
        :m_ballstate(ballstate){}
//    virtual ~GameMemento();

    std::vector<Ball*>* getBallState(){return m_ballstate;}

private:
    friend class GameOriginator;
    std::vector<Ball*>* m_ballstate;
};

#endif // GAMEMEMENTO_H
