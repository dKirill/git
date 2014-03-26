#include <QtWidgets>
#include "gui.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <QDebug>

gui::gui(QWidget *parent) :
    QWidget(parent), pgame(nullptr)
{
    QHBoxLayout* main_h_lo = new QHBoxLayout;
    main_h_lo->setMargin(0);
    main_h_lo->setSpacing(0);
    setLayout(main_h_lo);

    //init levoy chasti

    bot_or_playerW = new QWidget;
    QVBoxLayout* bot_or_playerL = new QVBoxLayout; //vibor sopernika
    bot_or_playerL->addWidget(new QLabel("Choose game mode"));
    QPushButton* qpb1 = new QPushButton("vs Human");
    QPushButton* qpb2 = new QPushButton("vs Bot");
    connect(qpb1, SIGNAL(clicked()), SLOT(pvp()));
    connect(qpb2, SIGNAL(clicked()), SLOT(pvb()));
    bot_or_playerL->addWidget(qpb1);
    bot_or_playerL->addWidget(qpb2);
    bot_or_playerL->addStretch(1);
    bot_or_playerW->setLayout(bot_or_playerL);

    choosing_colourW = new QWidget;
    QVBoxLayout* choosing_colourL = new QVBoxLayout; //vibor cveta
    choosing_colourL->addWidget(new QLabel("Choose your colour"));
    QPushButton* qpb3 = new QPushButton("white(2nd turn)");
    QPushButton* qpb4 = new QPushButton("black(1st turn)");
    connect(qpb3, SIGNAL(clicked()), SLOT(chwh()));
    connect(qpb4, SIGNAL(clicked()), SLOT(chbl()));
    choosing_colourL->addWidget(qpb3);
    choosing_colourL->addWidget(qpb4);
    choosing_colourL->addStretch(1);
    choosing_colourW->setLayout(choosing_colourL);

    score_lturnW = new QWidget;
    QVBoxLayout* score_lturnL = new QVBoxLayout; //info vo vrema igri
    lturn = new QLabel; //menaetsa na kajdom hodu tak chto toka init
    pturn = new QLabel;
    score = new QLabel;
    score_lturnL->addWidget(lturn);
    score_lturnL->addWidget(pturn);
    score_lturnL->addWidget(score);
    score_lturnL->addStretch(1);
    score_lturnW->setLayout(score_lturnL);

    after_gameW = new QWidget;
    QVBoxLayout* after_gameL = new QVBoxLayout; //predlojenie sigrat' ewe + result
    winner = new QLabel;
    offer_to_play_again = new QLabel("Wanna play again?");
    QPushButton* qpb5 = new QPushButton("yes");
    connect(qpb5, SIGNAL(clicked()), SLOT(restart()));
    QPushButton* qpb6 = new QPushButton("no");
    connect(qpb6, SIGNAL(clicked()), qApp, SLOT(quit()));
    after_gameL->addWidget(winner);
    after_gameL->addWidget(offer_to_play_again);
    after_gameL->addWidget(qpb5);
    after_gameL->addWidget(qpb6);
    after_gameL->addStretch(1);
    after_gameW->setLayout(after_gameL);

    leftpart = new QStackedLayout;
    leftpart->addWidget(bot_or_playerW);
    leftpart->addWidget(choosing_colourW);
    leftpart->addWidget(score_lturnW);
    leftpart->addWidget(after_gameW);

    //teper' igrovaya doska

    deckW = new QWidget;
    QGridLayout* deckL = new QGridLayout;
    deckL->setMargin(0);
    deckL->setSpacing(0);
    deckW->setLayout(deckL);

    e_pic = QPixmap("empty.png");
    b_pic = QPixmap("black.png");
    w_pic = QPixmap("white.png");
    pic_sz = e_pic.size();

    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
        {
            QPushButton* tqpb = new QPushButton;
            tqpb->setIcon(e_pic);
            tqpb->setIconSize(pic_sz);

            std::stringstream ss;
            std::string s;
            ss << i << j;
            ss >> s;

            tqpb->setObjectName(s.c_str());
            deckL->addWidget(tqpb, i, j);
            tqpb->setParent(deckW);
            connect(tqpb, SIGNAL(clicked()), SLOT(turn_try()));
        }
    //kartinka
    bg_pic = new QWidget;
    QPalette pal;
    pal.setBrush(bg_pic->backgroundRole(), QBrush(QPixmap("bg.png")));
    bg_pic->setPalette(pal);
    bg_pic->resize(deckW->size());
    bg_pic->setAutoFillBackground(true);

    rightpart = new QStackedLayout;
    rightpart->addWidget(deckW);
    rightpart->addWidget(bg_pic);

    main_h_lo->addLayout(leftpart);
    main_h_lo->addLayout(rightpart);
    leftpart->setCurrentWidget(bot_or_playerW);
    rightpart->setCurrentWidget(bg_pic);
    setMaximumSize(size());
}

void gui::pvp()
{
    bot_activated = false;
    leftpart->setCurrentWidget(choosing_colourW);
}

void gui::pvb()
{
    bot_activated = true;
    leftpart->setCurrentWidget(choosing_colourW);
}

void gui::chwh()
{
    p1_colour = white;
    start_game();
}

void gui::chbl()
{
    p1_colour = black;
    start_game();
}

void gui::start_game()
{
    pgame = new game(bot_activated, p1_colour);
    connect(pgame, SIGNAL(eog()), SLOT(endofgame()));
    connect(pgame, SIGNAL(new_turn_table(const char*, const char*)),
                   SLOT(update(const char*, const char*)));
    connect(this, SIGNAL(do_turn(int, int)), pgame, SLOT(make_turn(int, int)));
    connect(pgame, SIGNAL(eot()), SLOT(next_turn()));

    leftpart->setCurrentWidget(score_lturnW);
    rightpart->setCurrentWidget(deckW);
    pgame->print();
    next_turn();
}

void gui::next_turn()
{
    if (!strcmp(pgame->curr_player(), "player2") && pgame->bot_plays() == true)
        bot_makes_turn(*pgame);
}

void gui::update(const char* lt, const char* sc)
{
    lturn->setText(QString(lt));
    pturn->setText(QString("curr turn:\n") + QString(pgame->curr_player()));
    score->setText(QString("score:\n") + QString(sc));
    //obnovlyaem dosku
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
        {
            QString name;
            QString t;
            name.setNum(i);
            t.setNum(j);
            name += t;

            QPushButton* b = deckW->findChild<QPushButton*>(name);
            cell c = pgame->wh_c(i, j);

            if (c == empty)
                b->setIcon(e_pic);
            else if (c == white)
                b->setIcon(w_pic);
            else if (c == black)
                b->setIcon(b_pic);

            b->setIconSize(pic_sz);
        }qDebug() << "upd";
}

void gui::turn_try()
{
    if (std::strcmp(pgame->curr_player(), "player2") == 0 && pgame->bot_plays())
        return; //esli seychas hodit BOT niche ne delaem
    QString bname = sender()->objectName();
    int num = bname.toInt();

    emit do_turn(num / 10, num % 10);
}

void gui::endofgame()
{
    std::string s = pgame->get_winner();
    s += " won!";
    winner->setText(s.c_str());
    leftpart->setCurrentWidget(after_gameW);
}

void gui::restart() //newgame
{
    delete pgame;
    rightpart->setCurrentWidget(bg_pic);
    leftpart->setCurrentWidget(bot_or_playerW);
}
//eof
