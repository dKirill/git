#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <typeinfo>
#include <chrono>
#include <thread>
#include <sstream>
#include <vector>
#include <cstring>
#include <QDebug>
//"ne ochen" effektivno sdelan bot :D nu da ladno

using namespace std;

enum cell { empty, white, black };
enum column { LB_C = -1, A, B, C, D, E, F, G, H, RB_C };
enum row { LB_R = -1, one, two, three, four, five, six, seven, eigth, RB_R };

column& operator--(column&);
column& operator++(column&);
row& operator--(row&);
row& operator++(row&);
void intToEnum(column&, row&, int, int);

class game;

void bot_gets_turn(game&, column&, row&); //teper po ssilke
void bot_makes_turn(game&);
void play(game&);

class game : public QObject
{
    Q_OBJECT
private:
    vector<vector<cell>> gamedeck;
    vector<cell> players;
    int curr;
    int next;
    int count;
    int p1c;
    int p2c;
    string lastTurn;
    bool p2_bot_activated;

    vector<vector<cell>> Sgamedeck;
    vector<cell> Splayers;
    int Scurr;
    int Snext;
    int Scount;
    int Sp1c;
    int Sp2c;
    string SlastTurn;
    bool Sp2_bot_activated;

    bool backed_up;
public:
    explicit game(bool = false, cell = white); //false = 2 igroka, true-PvBot
    ~game() { }

    int turn(column, row);

    void back_up();
    void rollback();

    void check_tl(column, row);
    void check_t(column, row);
    void check_tr(column, row);
    void check_r(column, row);
    void check_br(column, row);
    void check_b(column, row);
    void check_bl(column, row);
    void check_l(column, row);

    void count_cells();

    bool bot_plays() { return p2_bot_activated; }
    std::string p1() { return players[0] == white ? "white" : "black"; }
    std::string p2() { return players[1] == white ? "white" : "black"; }
    int get_p1c() { return p1c; }
    int get_p2c() { return p2c; }
    int get_diff() { return (p1c > p2c) ? p1c - p2c : p2c - p1c; }
    const char* curr_player() { return curr == 0 ? "player1" : "player2"; }
    int get_count() { return count; }
    std::string get_winner();
    //vector<vector<cell> > get_copy_of_gd() { return gamedeck; } pohodu ne nujna
    cell wh_c(int i, int j) { return gamedeck[i][j]; }

    void set_lt(column, row);

    void print();
signals:
    void eot(); //konec hoda
    void eog(); //konec partii
    void new_turn_table(const char*, const char*);
public slots:
    void make_turn(int, int);
};

#endif // GAME_H
