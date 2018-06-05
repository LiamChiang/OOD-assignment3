//#ifndef POCKETDECORATOR_H
//#define POCKETDECORATOR_H
//#include "pocket.h"
//#include "utils.h"
//#include "mouseeventable.h"

//class pocketDecorator: public Pocket{
//protected:
//    Pocket* m_subPocket;
//public:
//    pocketDecorator(Pocket* pocket) : m_subPocket(pocket) {}
//    virtual ~pocketDecorator() { delete m_subPocket; }
//    void render(QPainter& painter, const QVector2D& offset) {m_subPocket->render(painter, offset);}
//    bool contains(const QVector2D& center, const double& radius) {m_subPocket->contains(center, radius);}
//    void incrementSunk() { m_subPocket->incrementSunk(); }
//    Pocket* copyPocket(){return m_subPocket->copyPocket();}
//    size_t getNumSunk(){return m_subPocket->getNumSunk();}
//    QVector2D getPosition(){return m_subPocket->getPosition();}
//    double getRadius(){return m_subPocket->getRadius();}
//    QBrush getColourBrush(){return m_subPocket->getColourBrush();}
//    void changePosition(const QVector2D& delta){ m_subPocket->changePosition(delta); }
//};

//class PocketMovingDecorator : public pocketDecorator {
//protected:
//    static constexpr double fadeRate = 0.01;
//public:
//    PocketMovingDecorator(Pocket* p) : pocketDecorator(p) {}

//    void render(QPainter &painter, const QVector2D &offset);
//    Pocket* copyPocket();
//};

//#endif // POCKETDECORATOR_H
