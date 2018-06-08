#include "table.h"
#include "ball.h"
#include <iostream>

void StageOneTable::render(QPainter &painter, const QVector2D& offset) {
    // our table colour
    painter.setBrush(m_brush);
    // draw table
    painter.drawRect(offset.x(), offset.y(), this->getWidth(), this->getHeight());
}

void StageOneTable::addPocket() {
    std::cout << "not allowed to add pocket";
}

void StageTwoTable::render(QPainter &painter, const QVector2D& offset) {
    // our table colour
    painter.setBrush(m_brush);
    // draw table
    painter.drawRect(offset.x(), offset.y(), this->getWidth(), this->getHeight());

    // render the pockets relative to this table
    for (Pocket* p : m_pockets) {
        p->render(painter, offset);
    }
}

void StageTwoTable::addPocket() {
    std::cout << "not allowed to add pocket";
}

StageTwoTable::~StageTwoTable() {
    for (Pocket* p : m_pockets) delete p;
}

bool StageTwoTable::sinks(Ball *b) {
    QVector2D absPos = b->getPosition();
    double radius = b->getRadius();
    // check whether any pockets consumes this ball
    for (Pocket* p : m_pockets) {
        // you sunk my scrabbleship
        if (p->contains(absPos, radius)) {
            p->incrementSunk();
            return true;
        }
    }
    return false;
}

void StageThreeTable::render(QPainter &painter, const QVector2D& offset) {
    // our table colour
    painter.setBrush(m_brush);
    // draw table
    painter.drawRect(offset.x(), offset.y(), this->getWidth(), this->getHeight());

    // render the pockets relative to this table
    for (Pocket* p : m_pockets) {
        p->render(painter, offset);
    }
}

StageThreeTable::~StageThreeTable() {
    for (Pocket* p : m_pockets) delete p;
}

void StageThreeTable::addPocket() {
    double y = rand()%m_height;
    double x = rand()%m_width;
    QVector2D newpos = QVector2D(x,y);
    Pocket* p = new Pocket(15.0, newpos);
    m_pockets.push_back(p);
}

bool StageThreeTable::sinks(Ball *b) {
    QVector2D absPos = b->getPosition();
    double radius = b->getRadius();
    // check whether any pockets consumes this ball
    for (Pocket* p : m_pockets) {
        // you sunk my scrabbleship
        if (p->contains(absPos, radius)) {
            p->incrementSunk();
            return true;
        }
    }
    for(int i = 0; i < m_pockets.size(); ++i){
        if(m_pockets.at(i)->getNumSunk() == 5){
            delete m_pockets.at(i);
            m_pockets.erase(m_pockets.begin()+i);
        }
    }
    return false;
}
