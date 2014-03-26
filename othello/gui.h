#ifndef GUI_H
#define GUI_H

#include <QtWidgets>
#include "game.h"
#include <cstring>
#include <QDebug>
#include <string>

class gui : public QWidget
{
    Q_OBJECT
private:
    game* pgame; //ekzemplar konketnoy partii
    QStackedLayout* leftpart; //info s viborom i menushkami
    QStackedLayout* rightpart;
    //leftpart:
    QWidget* bot_or_playerW;
    QWidget* choosing_colourW;
    QWidget* score_lturnW;
    QWidget* after_gameW;

    QLabel* lturn; // score_lturn: last turn string
    QLabel* pturn; // kto hodit
    QLabel* score; // schet

    QLabel* winner; // after_game
    QLabel* offer_to_play_again;
    //righpart:
    QWidget* deckW;
    QWidget* bg_pic;

    bool bot_activated;
    cell p1_colour;

    QPixmap e_pic;
    QPixmap b_pic;
    QPixmap w_pic;
    QSize pic_sz;
public:
    explicit gui(QWidget *parent = 0);

    void start_game();
signals:
    void do_turn(int, int);
public slots:
    void pvp(); //vibrano vs Hum
    void pvb(); //vs Bot
    void chwh(); //p1 - white
    void chbl(); //p1 - black

    void next_turn();
    void update(const char*, const char*);
    void turn_try();
    void endofgame();
    void restart();
};

#endif // GUI_H
