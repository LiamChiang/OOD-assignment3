#include "game.h"
#include "utils.h"

#include <math.h>
#include <QDebug>
#include <QJsonArray>
#include <stdexcept>
#include <cmath>
#include <exception>
#include <iostream>

Game::~Game() {
    // cleanup ya boi
    for (auto b : *m_balls) delete b;
    delete m_balls;
    delete m_table;
}

void Game::render(QPainter &painter) const {
    // table is rendered first, as its the lowest
    m_table->render(painter, m_screenshake);
    // then render all the balls
    for (Ball* b : *m_balls) b->render(painter, m_screenshake);
}

void imagePopup(){
    QGraphicsScene* scene = new QGraphicsScene();
    scene->addItem(new QGraphicsPixmapItem(QPixmap(":/images/Jumpscare.png")));
    QGraphicsView* view = new QGraphicsView(scene);
    view->show();

}

void playSoundSink(){
    QMediaPlayer* sinksound = new QMediaPlayer();
    sinksound->setMedia(QUrl("qrc:/sounds/death-Roman_K-1190383229.mp3"));
    sinksound->play();
}

void playSoundBreak(){
    QMediaPlayer* bomb = new QMediaPlayer();
    bomb->setMedia(QUrl("qrc:/sounds/Bomb_Exploding-Sound_Explorer-68256487.wav"));
    bomb->setVolume(100);
    bomb->play();
}

void Game::animate(double dt) {
    // keep track of the removed balls (they're set to nullptr during loop)
    // clean up afterwards
    std::vector<Ball*> toBeRemoved;
    // add these balls to the list after we finish
    std::vector<Ball*> toBeAdded;

    // (test) collide the ball with each other ball exactly once
    // to achieve this, balls only check collisions with balls "after them"
    for (auto it = m_balls->begin(); it != m_balls->end(); ++it) {
        Ball* ballA = *it;
        if (ballA == nullptr) continue;

        // correct ball velocity if colliding with table
        QVector2D tableBallDeltaV = resolveCollision(m_table, ballA);
        // test and resolve breakages with balls bouncing off table
        if (ballA->applyBreak(tableBallDeltaV, toBeAdded)) {
            // mark this ball to be deleted
            toBeRemoved.push_back(ballA);
            incrementShake();
            // nullify this ball
            *it = nullptr;

            continue;
        }

        // check whether ball should be swallowed
        if (m_table->sinks(ballA)) {
            //play sound
            if(isStage3()){
                playSoundSink();
                //trigger the sinked flag if cue ball sinks
                if(ballA->getColour().name() == "#000000"){
                    imagePopup();
                    sinked = true;
                }
            }
            // defer swallowing until later (messes iterators otherwise)
            toBeRemoved.push_back(ballA);

            // nullify this ball
            *it = nullptr;
            continue;
        }

        // check collision with all later balls
        for (auto nestedIt = it + 1; nestedIt != m_balls->end(); ++nestedIt) {
            Ball* ballB = *nestedIt;
            if (ballB == nullptr) continue;
            if (isColliding(ballA, ballB)) {
                // retrieve the changes in velocities for each ball and resolve collision
                QVector2D ballADeltaV,ballBDeltaV;
                std::tie(ballADeltaV, ballBDeltaV) = resolveCollision(ballA, ballB);

                // add screenshake, remove ball, and add children to table vector if breaking
                if (ballA->applyBreak(ballADeltaV, toBeAdded)) {
                    if(isStage3()){
                        playSoundBreak();
                    }
                    toBeRemoved.push_back(ballA);
                    incrementShake();
                    // nullify this ball
                    *it = nullptr;
                    break;
                }
                // add screenshake, remove ball, and add children to table vector if breaking
                if (ballB->applyBreak(ballBDeltaV, toBeAdded)) {
                    if(isStage3()){
                        playSoundBreak();
                    }
                    toBeRemoved.push_back(ballB);
                    incrementShake();
                    // nullify this ball
                    *nestedIt = nullptr;
                    continue;
                }
            }
        }
        // we marked this ball as deleted, so skip
        if (*it == nullptr) continue;

        // move ball due to speed
        ballA->translate(ballA->getVelocity() * dt);
        // apply friction
        ballA->changeVelocity(-ballA->getVelocity() * m_table->getFriction() * dt);
    }

    // clean up them trash-balls
    for (Ball* b : toBeRemoved) {
        delete b;
        // delete all balls marked with nullptr
        m_balls->erase(std::find(m_balls->begin(), m_balls->end(), nullptr));
    }
    for (Ball* b: toBeAdded) m_balls->push_back(b);

    if(stage3Pocket){
        int randomIndex = rand()%m_table->getPockets().size();
        if(randomIndex % 2 == 0){
            m_table->getPockets().at(randomIndex)->changeHorizontalPosition();
            if(m_table->getWidth() - m_table->getPockets().at(randomIndex)->getRadius() < m_table->getPockets().at(randomIndex)->getPosition().x()){
                m_table->getPockets().at(randomIndex)->changePocketHorizontalDirection();
            }
            else if( -1 * m_table->getPockets().at(randomIndex)->getRadius() > m_table->getPockets().at(randomIndex)->getPosition().x()){
                m_table->getPockets().at(randomIndex)->changePocketHorizontalDirection();
            }
        }
        else{
            m_table->getPockets().at(randomIndex)->changeVerticalPosition();
            if(m_table->getHeight() - m_table->getPockets().at(randomIndex)->getRadius() < m_table->getPockets().at(randomIndex)->getPosition().y()){
                m_table->getPockets().at(randomIndex)->changePocketVerticalDirection();
            }
            else if( -1 * m_table->getPockets().at(randomIndex)->getRadius() > m_table->getPockets().at(randomIndex)->getPosition().y()){
                m_table->getPockets().at(randomIndex)->changePocketVerticalDirection();
            }
        }
    }

    updateShake(dt);
}

void Game::updateShake(double dt) {
    // <3 code lovingly taken from here: <3
    // https://gamedev.stackexchange.com/a/47565
    // slightly modified..

    // update the screen shake per time step
    m_shakeRadius *= (1-dt)*0.9;
    m_shakeAngle += (150 + rand()%60);
    m_screenshake = QVector2D(sin(m_shakeAngle)*m_shakeRadius, cos(m_shakeAngle)*m_shakeRadius);
}

QVector2D Game::resolveCollision(const Table* table, Ball* ball) {
    QVector2D bPos = ball->getPosition();

    QVector2D startingVel = ball->getVelocity();

    // resulting multiplicity of direction. If a component is set to -1, it
    // will flip the velocity's corresponding component
    QVector2D vChange(1,1);

    // ball is beyond left side of table's bounds
    if (bPos.x() - ball->getRadius() <= 0) {
        // flip velocity if wrong dir
        if (ball->getVelocity().x() <= 0) vChange.setX(-1);
    // ball is beyond right side of table's bounds
    } else if (bPos.x() + ball->getRadius() >= 0 + table->getWidth()) {
        // flip velocity if wrong dir
        if (ball->getVelocity().x() >= 0) vChange.setX(-1);
    }
    // ball is above top of the table's bounds
    if (bPos.y() - ball->getRadius() <= 0) {
        // flip iff we're travelling in the wrong dir
        if (ball->getVelocity().y() <= 0) vChange.setY(-1);
    // ball is beyond bottom of table's bounds
    } else if (bPos.y() + ball->getRadius() >= 0 + table->getHeight()) {
        // if we're moving down (we want to let the ball bounce up if its heading back)
        if (ball->getVelocity().y() >= 0) vChange.setY(-1);
    }

    ball->multiplyVelocity(vChange);

    // return the change in velocity
    return ball->getVelocity() - startingVel;
}

std::pair<QVector2D, QVector2D> Game::resolveCollision(Ball* ballA, Ball* ballB) {
    // SOURCE : ASSIGNMENT SPEC

    // if not colliding (distance is larger than radii)
    QVector2D collisionVector = ballB->getPosition() - ballA->getPosition();
    if (collisionVector.length() > ballA->getRadius() + ballB->getRadius()) {
       throw std::logic_error("attempting to resolve collision of balls that do not touch");
    }
    collisionVector.normalize();

    QVector2D ballAStartingVelocity = ballA->getVelocity();
    QVector2D ballBStartingVelocity = ballB->getVelocity();

    float mr = ballB->getMass() / ballA->getMass();
    double pa = QVector2D::dotProduct(collisionVector, ballA->getVelocity());
    double pb = QVector2D::dotProduct(collisionVector, ballB->getVelocity());

    if (pa <= 0 && pb >= 0) return std::make_pair(QVector2D(0,0), QVector2D(0,0));

    double a = -(mr + 1);
    double b = 2*(mr * pb + pa);
    double c = -((mr - 1)*pb*pb + 2*pa*pb);
    double disc = sqrt(b*b - 4*a*c);
    double root = (-b + disc)/(2*a);
    if (root - pb < 0.01) {
        root = (-b - disc)/(2*a);
    }

    ballA->changeVelocity(mr * (pb - root) * collisionVector);
    ballB->changeVelocity((root-pb) * collisionVector);

    // return the change in velocities for the two balls
    return std::make_pair(ballA->getVelocity() - ballAStartingVelocity, ballB->getVelocity() - ballBStartingVelocity);
}

void Game::saveGameDate(std::vector<Ball*>* ballstates){
    std::vector<Ball*>* balls_state = new std::vector<Ball*>();
    for (Ball* b: *ballstates){
        balls_state->push_back(b->copyBall());
    }
    originator.set(balls_state);
    caretaker.add(originator.saveToMemento());
}

void Game::addBall(){
    int randomIndex = rand()%m_balls->size();
    double x = rand()%m_table->getWidth();
    double y = rand()%m_table->getHeight();
    QVector2D newpos = QVector2D(x,y);
    if(randomIndex != 0){
        Ball * b = m_balls->at(randomIndex)->copyBall();
        b->setPosition(newpos);
        m_balls->push_back(b);
    }
}

void Game::removeBall(){
    int randomIndex = rand()%m_balls->size();
    if(randomIndex != 0){
        delete m_balls->at(randomIndex);
        m_balls->erase(m_balls->begin()+randomIndex);
    }
}

void Game::ballMove(){
    for(Ball* b : *m_balls){
        double x = rand()%m_table->getWidth();
        double y = rand()%m_table->getHeight();
        QVector2D newvel = QVector2D(x,y);
        b->setVelocity(newvel);
    }
}

void Game::addPocket(){
    m_table->addPocket();
}

void Game::undo(Game * game){
    if(!m_balls->empty()){
        if(m_balls->at(0)->getVelocity().length() < 1 && sinked != true){
            originator.restoreFromMemento(caretaker.getState(caretaker.getMementoList().size() - 1));
            m_balls->clear();
            for(Ball* b: *originator.getBallState()){
                m_balls->push_back(b->copyBall());
            }
            Ball* c = m_balls->front();
            CueBall* cb = new CueBall(c);
            m_balls->front() = static_cast<Ball*>(cb);
            m_mouseEventFunctions.clear();
            game->addMouseFunctions(cb->getEvents());
            caretaker.getMementoList().clear();
            originator.set(config_balls);
            caretaker.add(originator.saveToMemento());
        }
        else{
            originator.restoreFromMemento(caretaker.getState(0));
            m_balls->clear();
            for(Ball* b: *originator.getBallState()){
                m_balls->push_back(b->copyBall());
            }
            Ball* c = m_balls->front();
            CueBall* cb = new CueBall(c);
            m_balls->front() = static_cast<Ball*>(cb);
            m_mouseEventFunctions.clear();
            game->addMouseFunctions(cb->getEvents());
            caretaker.getMementoList().clear();
            originator.set(config_balls);
            caretaker.add(originator.saveToMemento());
            sinked = false;
        }
    }
    else{
        originator.restoreFromMemento(caretaker.getState(0));
        for(Ball* b: *originator.getBallState()){
            m_balls->push_back(b->copyBall());
        }
        Ball* c = m_balls->front();
        CueBall* cb = new CueBall(c);
        m_balls->front() = static_cast<Ball*>(cb);
        m_mouseEventFunctions.clear();
        game->addMouseFunctions(cb->getEvents());
        caretaker.getMementoList().clear();
        originator.set(config_balls);
        caretaker.add(originator.saveToMemento());
        sinked = false;

    }
}
