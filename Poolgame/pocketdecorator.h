#ifndef POCKETDECORATOR_H
#define POCKETDECORATOR_H
#include "pocket.h"
#include "utils.h"
#include "mouseeventable.h"

class pocketDecorator: public Pocket{
protected:
    Pocket* m_subPocket;
public:
    pocketDecorator(Pocket* pocket) : m_subPocket(pocket) {}
    virtual ~pocketDecorator() { delete m_subPocket; }
    void render(QPainter& painter, const QVector2D& offset) {m_subPocket->render(painter, offset);}
    bool contains(const QVector2D& center, const double& radius) {m_subPocket->contains(center, radius);}
    void incrementSunk() {m_subPocket->incrementSunk();}
};

#endif // POCKETDECORATOR_H