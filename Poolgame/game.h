#pragma once
#include <QDebug>
#include <QJsonObject>
#include <functional>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <algorithm>

#include "abstractstagefactory.h"
#include "gameoriginator.h"
#include "gamestatecaretaker.h"
#include "gamememento.h"
#include "table.h"
#include "ball.h"
#include "balldecorator.h"
#include "utils.h"


class Game {
    std::vector<Ball*>* m_balls;
    std::vector<Ball*>* config_balls;
    std::vector<Pocket*>* m_pockets;
    Table* m_table;
    GameOriginator originator;
    GameStateCareTaker caretaker;
    // screenshake stuff
    QVector2D m_screenshake;
    double m_shakeRadius = 0.0;
    double m_shakeAngle = 0;
    static constexpr double SCREENSHAKEDIST = 10.0;
    bool sinked = false;
    bool stage3Trigger = false;
    bool stage3Pocket = false;

    /* increase the amount of screen shake */
    void incrementShake(double amount=SCREENSHAKEDIST) { m_shakeRadius += amount; }
private:
    // store the functions that get scanned through whenever a mouse event happens
    MouseEventable::EventQueue m_mouseEventFunctions;

    /**
     * @brief updateShake - update the screenshake radius (make it smaller)
     * @param dt - the timestep change
     */
    void updateShake(double dt);
public:
    ~Game();
    Game(std::vector<Ball*>* balls, Table* table) :
        m_balls(balls), m_table(table) {

        // deep copy the ball state for storing the ball state into memento list
        config_balls = new std::vector<Ball*>();
        for(Ball* b: *m_balls){
            Ball* clone =  b->copyBall();
            config_balls->push_back(clone);
        }
        // set up the ball state through the originator and save the satate into memento list by the caretaker
        originator.set(config_balls);
        caretaker.add(originator.saveToMemento());
    }
    /**
     * @brief Draws all owned objects to the screen (balls and table)
     * @param painter - qtpainter to blit to screen with
     */
    void render(QPainter& painter) const;
    /**
     * @brief Updates the positions of all objects within, based on how much time has changed
     * @param dt - time elapsed since last frame in seconds
     */
    void animate(double dt);

    /* how large the window's width should at least be */
    int getMinimumWidth() const { return m_table->getWidth(); }
    /* how large the window's height should at least be */
    int getMinimumHeight() const { return m_table->getHeight(); }

    /**
     * @brief resolveCollision - modify the ball's velocity if it is colliding with the table
     * @param table - the table to be bounds checked
     * @param ball - the ball to move
     * @return velocity - the change of velocity that a ball underwent
     */
    QVector2D resolveCollision(const Table* table, Ball* ball);
    /**
     * @brief resolveCollision - resolve both ball's velocity whether these balls collide
     * @param ballA - first ball
     * @param ballB - second ball
     * @param pair<deltaVelocityA, deltaVelocityB> - the change of velocities for each ball
     */
    std::pair<QVector2D, QVector2D> resolveCollision(Ball* ballA, Ball* ballB);

    /**
     * @brief isColliding - returns whether two balls are touching each other
     * @param ballA
     * @param ballB
     * @return whether the two balls are touching each other
     */
    bool isColliding(const Ball* ballA, const Ball* ballB) {
        QVector2D collisionVector = ballB->getPosition() - ballA->getPosition();
        return !(collisionVector.length() > ballA->getRadius() + ballB->getRadius());
    }

    /**
     * @brief addMouseFunctions - append all of the specified functions to be
     *  our eventqueue - these will be cycled through onclick, etc
     * @param fns
     */
    void addMouseFunctions(MouseEventable::EventQueue fns) {
        std::copy(fns.begin(), fns.end(), std::back_inserter(m_mouseEventFunctions));
    }

    /**
     * @brief getEventFns - get all of our event functions (mouseclicksfns, etc)
     * @return event queue of event functions
     */
    MouseEventable::EventQueue& getEventFns() { return m_mouseEventFunctions; }

    /**
     * @brief addBall - dynamically add random balls once receive the command Q
     * @return void
     */
    void addBall();
    /**
     * @brief removeBall - dynamically remove random balls once receive the command W
     * @return void
     */
    void removeBall();
    /**
     * @brief undo - undo the ball state to previous one or to the initialised one once receive the command R
     * @return void
     */
    void undo(Game* game);
    /**
     * @brief ballMove - dynamically make all balls move once receive the command A
     * @return void
     */
    void ballMove();
    /**
     * @brief addPocket - dynamically add random pockets once receive the command S
     * @return void
     */
    void addPocket();
    /**
     * @brief getBalls - get current ball information in the game
     * @return std::vector<Ball*>
     */
    std::vector<Ball*>* getBalls(){ return m_balls; }
    /**
     * @brief getPockets - get current pockets information in the game
     * @return std::vector<Pocket*>
     */
    std::vector<Pocket*>* getPockets(){return m_pockets;}
    /**
     * @brief saveGameDate - save the ball state into the memento list by the originator and caretake.
     * @return void
     */
    void saveGameDate(std::vector<Ball*>* ballstates);
    /**
     * @brief triggerStage3 - trigger stage3 extensions once its stage3 game
     * @return void
     */
    void triggerStage3(){stage3Trigger = true;}
    /**
     * @brief triggerPocket - trigger pocket moving extensions once receive its stage 3 game and receive the command E
     * @return void
     */
    void triggerPocket(){stage3Pocket = true;}
    /**
     * @brief stopPocket - stop moving pockets once receive its stage 3 game and receive the command E again
     * @return void
     */
    void stopPocket(){stage3Pocket = false;}
    /**
     * @brief isStage3 - check whether its stage 3 game or not
     * @return bool
     */
    bool isStage3(){return stage3Trigger;}
    /**
     * @brief isPocketTriggered - check whether the pocket moving extension is triggered or not.
     * @return bool
     */
    bool isPocketTriggered(){return stage3Pocket;}
};
