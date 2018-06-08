#include "dialog.h"
#include "ui_dialog.h"
#include <QPainter>
#include <QTimer>
#include <iostream>
#include <QMouseEvent>
#include "utils.h"

Dialog::Dialog(Game *game, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_game(game)
{

    ui->setupUi(this);

    // for animating (i.e. movement, collision) every animFrameMS
    aTimer = new QTimer(this);
    connect(aTimer, SIGNAL(timeout()), this, SLOT(nextAnim()));
    aTimer->start(animFrameMS);

    // for drawing every drawFrameMS milliseconds
    dTimer = new QTimer(this);
    connect(dTimer, SIGNAL(timeout()), this, SLOT(tryRender()));
    dTimer->start(drawFrameMS);

    //set up background music if its stage3
    if(m_game->isStage3()){
        background = new QMediaPlaylist();
        //set up the sound effects
        ballshot = new QMediaPlayer();
        powerup = new QMediaPlayer();
        doh = new QMediaPlayer();
        bomb = new QMediaPlayer();

        background->addMedia(QUrl("qrc:/sounds/002 - Kazumi Totaka - Mii Plaza.mp3"));
        ballshot->setMedia(QUrl("qrc:/sounds/40_smith_wesson_single-mike-koenig.wav"));
        powerup->setMedia(QUrl("qrc:/sounds/Dragon Ball Z - Sound Effects - Super Saiyan. (192  kbps).mp3"));
        bomb->setMedia(QUrl("qrc:/sounds/Bomb_Exploding-Sound_Explorer-68256487.wav"));
        doh->setMedia(QUrl("qrc:/sounds/Doh 6.mp3"));
        background->setPlaybackMode(QMediaPlaylist::Loop);

        powerup->setVolume(60);
        ballshot->setVolume(50);
        music= new QMediaPlayer();
        music->setPlaylist(background);
        music->play();

    }

    // set the window size to be at least the table size
    this->resize(game->getMinimumWidth(), game->getMinimumHeight());
}

Dialog::~Dialog()
{
    delete aTimer;
    delete dTimer;
    delete m_game;
    delete ui;
}

void Dialog::tryRender() {
    this->update();
}

void Dialog::nextAnim() {
    m_game->animate(1.0/(double)animFrameMS);
}

void Dialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    m_game->render(painter);
}

void Dialog::mousePressEvent(QMouseEvent* event) {
    if(m_game->isStage3()){
        if(powerup->state() == QMediaPlayer::PlayingState){
            powerup->setPosition(0);
        }
        else if(powerup->state() == QMediaPlayer::StoppedState){
            powerup->play();
        }
    }
    evalAllEventsOfTypeSpecified(MouseEventable::EVENTS::MouseClickFn, event);
}

void Dialog::mouseReleaseEvent(QMouseEvent* event) {
    if(m_game->isStage3()){
        if(ballshot->state() == QMediaPlayer::PlayingState){
            ballshot->setPosition(0);
        }
        else if(ballshot->state() == QMediaPlayer::StoppedState){
            ballshot->play();
        }
    }
    m_game->saveGameDate(m_game->getBalls());
    evalAllEventsOfTypeSpecified(MouseEventable::EVENTS::MouseRelFn, event);
}
void Dialog::mouseMoveEvent(QMouseEvent* event) {
    evalAllEventsOfTypeSpecified(MouseEventable::EVENTS::MouseMoveFn, event);
}

void Dialog::keyPressEvent(QKeyEvent* event){
    if(m_game->isStage3()){
        if(Qt::Key_R == event->key()){
            m_game->undo(m_game);
        }
        if(Qt::Key_Q == event->key()){
            if(m_game->isStage3()){
                if(doh->state() == QMediaPlayer::PlayingState){
                    doh->setPosition(0);
                }
                else if(doh->state() == QMediaPlayer::StoppedState){
                    doh->play();
                }
            }
            m_game->addBall();
        }
        if(Qt::Key_W == event->key()){
            if(m_game->isStage3()){
                if(bomb->state() == QMediaPlayer::PlayingState){
                    bomb->setPosition(0);
                }
                else if(bomb->state() == QMediaPlayer::StoppedState){
                    bomb->play();
                }
            }
            m_game->removeBall();
        }
        if(Qt::Key_E == event->key()){
            if(m_game->isPocketTriggered()){
                m_game->stopPocket();
            }
            else{
                m_game->triggerPocket();
            }
        }
        if(Qt::Key_S == event->key()){
            m_game->ballMove();
        }
        if(Qt::Key_A == event->key()){
            m_game->addPocket();
        }
    }
    if(event->key() == Qt::Key_Escape){
        close();
    }
}

void Dialog::evalAllEventsOfTypeSpecified(MouseEventable::EVENTS t, QMouseEvent *event) {
    // handle all the clicky events, and remove them if they've xPIRED
    MouseEventable::EventQueue& Qu = m_game->getEventFns();
    for (ssize_t i = Qu.size()-1; i >= 0; i--) {
        if (auto spt = (Qu.at(i)).lock()) {
            if (spt->second == t) {
                spt->first(event);
            }
        } else {
            // remove this element from our vector
            Qu.erase(Qu.begin() + i);
        }
    }
}
