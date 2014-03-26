#include "game.h"

column& operator--(column& c)
{
    switch (c)
    {
    case A:
        return c = LB_C;
    case B:
        return c = A;
    case C:
        return c = B;
    case D:
        return c = C;
    case E:
        return c = D;
    case F:
        return c = E;
    case G:
        return c = F;
    case H:
        return c = G;
    default:
        return c;
    }
}

column& operator++(column& c)
{
    switch (c)
    {
    case A:
        return c = B;
    case B:
        return c = C;
    case C:
        return c = D;
    case D:
        return c = E;
    case E:
        return c = F;
    case F:
        return c = G;
    case G:
        return c = H;
    case H:
        return c = RB_C;
    default:
        return c;
    }
}

row& operator--(row& r)
{
    switch (r)
    {
    case one:
        return r = LB_R;
    case two:
        return r = one;
    case three:
        return r = two;
    case four:
        return r = three;
    case five:
        return r = four;
    case six:
        return r = five;
    case seven:
        return r = six;
    case eigth:
        return r = seven;
    default:
        return r;
    }
}

row& operator++(row& r)
{
    switch (r)
    {
    case one:
        return r = two;
    case two:
        return r = three;
    case three:
        return r = four;
    case four:
        return r = five;
    case five:
        return r = six;
    case six:
        return r = seven;
    case seven:
        return r = eigth;
    case eigth:
        return r = RB_R;
    default:
        return r;
    }
}

void intToEnum(column& c, row& r, int i, int j)
{
    c = A;
    while (i--)
        ++c;
    r = one;
    while (j--)
        ++r;
}

game::game(bool bot, cell p1Colour) :
    QObject(), gamedeck(8, vector<cell>(8, empty)), players(2),
    count(4), p1c(2), p2c(2), p2_bot_activated(bot),
    Sgamedeck(8, vector<cell>(8, empty)), Splayers(2),
    backed_up(false)
{
    players[0] = p1Colour;
    players[1] = (players[0] == white) ? black : white;

    curr = (players[0] == white) ? 1 : 0;
    next = (curr == 1) ? 0 : 1;

    gamedeck[D][five] = black;
    gamedeck[E][four] = black;
    gamedeck[D][four] = white;
    gamedeck[E][five] = white;
}

int game::turn(column c, row r)
{
    if (gamedeck[c][r] != empty)
        return -1; //ne pustaya kletka

    gamedeck[c][r] = players[curr];

    check_tl(c, r);
    check_t(c, r);
    check_tr(c, r);
    check_r(c, r);
    check_br(c, r);
    check_b(c, r);
    check_bl(c, r);
    check_l(c, r);

    ++count;
    count_cells();

    if (backed_up == false) //backed_up==true - bot testiruet hod
    {
         if (!strcmp(curr_player(), "player2") && bot_plays() == true)
            this_thread::sleep_for(chrono::milliseconds(500));

        swap(curr, next); //smena hoda
        set_lt(c, r);
        print();
        if (get_count() >= 64)
        {
            emit eog();
            return 0;
        }
        emit eot();
    }

    return 0;
}

void game::back_up()
{
    Sgamedeck = gamedeck;
    Splayers = players;
    Scurr = curr;
    Snext = next;
    Scount = count;
    Sp1c = p1c;
    Sp2c = p2c;
    SlastTurn = lastTurn;
    Sp2_bot_activated = p2_bot_activated;

    backed_up = true;
}

void game::rollback()
{
    gamedeck = Sgamedeck;
    players = Splayers;
    curr = Scurr;
    next = Snext;
    count = Scount;
    p1c = Sp1c;
    p2c = Sp2c;
    lastTurn = SlastTurn;
    p2_bot_activated = Sp2_bot_activated;

    backed_up = false;
}

void game::check_tl(column c, row r)
{
    row savedr = r;
    column savedc = c;

    if (r == one || c == A || gamedeck[--c][--r] != players[next])
        return;

    do
    {
        --r;
        --c;
        if (r < one || c < A) //za predelami doski
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    row rr = --savedr; //xz for ne hochet oboih jrat'
    column cc = --savedc;
    for ( ; rr > r && cc > c; --rr, --cc) //zakrashiwaem v nash cvet
            gamedeck[cc][rr] = players[curr];
}

void game::check_t(column c, row r)
{
    row savedr = r;

    if (r == one || gamedeck[c][--r] != players[next])
        return;

    do
    {
        --r;
        if (r < one)
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    for (row rr = --savedr; rr > r; --rr) //zakrashiwaem v nash cvet
            gamedeck[c][rr] = players[curr];
}

void game::check_tr(column c, row r)
{
    row savedr = r;
    column savedc = c;

    if (r == one || c == H || gamedeck[++c][--r] != players[next])
        return;

    do
    {
        --r;
        ++c;
        if (r < one || c > H) //za predelami doski
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    row rr = --savedr;
    column cc = ++savedc;
    for ( ; rr > r && cc < c; --rr, ++cc) //zakrashiwaem v nash cvet
        gamedeck[cc][rr] = players[curr];
}

void game::check_r(column c, row r)
{
    column savedc = c;

    if (c == H || gamedeck[++c][r] != players[next])
        return;

    do
    {
        ++c;
        if (c > H)
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    for (column cc = ++savedc; cc < c; ++cc) //zakrashiwaem v nash cvet
        gamedeck[cc][r] = players[curr];
}

void game::check_br(column c, row r)
{
    row savedr = r;
    column savedc = c;

    if (r == eigth || c == H || gamedeck[++c][++r] != players[next])
        return;

    do
    {
        ++r;
        ++c;
        if (r > eigth || c > H) //za predelami doski
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    row rr = ++savedr;
    column cc = ++savedc;
    for ( ; rr < r && cc < c; ++rr, ++cc) //zakrashiwaem v nash cvet
        gamedeck[cc][rr] = players[curr];
}

void game::check_b(column c, row r)
{
    row savedr = r;

    if (r == eigth || gamedeck[c][++r] != players[next])
        return;

    do
    {
        ++r;
        if (r > eigth)
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    for (row rr = ++savedr; rr < r; ++rr) //zakrashiwaem v nash cvet
            gamedeck[c][rr] = players[curr];
}

void game::check_bl(column c, row r)
{
    row savedr = r;
    column savedc = c;

    if (r == eigth || c == A || gamedeck[--c][++r] != players[next])
        return;

    do
    {
        ++r;
        --c;
        if (r > eigth || c < A) //za predelami doski
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    row rr = ++savedr;
    column cc = --savedc;
    for ( ; rr < r && cc > c; ++rr, --cc) //zakrashiwaem v nash cvet
        gamedeck[cc][rr] = players[curr];
}

void game::check_l(column c, row r)
{
    column savedc = c;

    if (c == A || gamedeck[--c][r] != players[next])
        return;

    do
    {
        --c;
        if (c < A)
            return;
    }
    while (gamedeck[c][r] == players[next]); //smotrim skolko vrajeskih

    if (gamedeck[c][r] == empty)
        return;

    for (column cc = --savedc; cc > c; --cc) //zakrashiwaem v nash cvet
        gamedeck[cc][r] = players[curr];
}

void game::count_cells()
{
    p1c = 0;
    p2c = 0;
    for (vector<cell>& v : gamedeck)
        for (cell& c : v)
            (c == players[0]) ? ++p1c : ((c == players[1]) ? ++p2c : false);
}

std::string game::get_winner()
{
    int ip1 = 0, ip2 = 0;

    for (vector<cell>& v : gamedeck)
        for (cell& c : v)
        {
            if (c == white)
                players[0] == white ? ++ip1 : ++ip2;
            else
                players[0] == black ? ++ip1 : ++ip2;
        }

    std::string s = ip1 > ip2 ? ("player1(" + p1() + ", " + to_string(ip1) + ")") : ("player2(" + p2() + ", " + to_string(ip2) + ")");
    return s;
}

void game::set_lt(column c, row r)
{
    stringstream ss;
    char t;
    char* ans = new char[3];
    ss << c;
    ss >> t;
    ans[0] = (t + 17); //POCHEMU 17 a ne 65 ? xz
    ss << r;
    ss >> t;
    ans[1] = (t + 1);
    ans[2] = '\0';
    lastTurn = std::string(ans);
    delete[] ans;
}

void game::print()
{
    string lt = "last turn: " + (lastTurn.empty() ? "INITIALIZATION" : lastTurn);
    stringstream ss;
    ss << "p1(" << p1c << ") : p2 (" << p2c << ")";
    string sc;
    getline(ss, sc);

    emit new_turn_table(lt.c_str(), sc.c_str());
}

void game::make_turn(int i, int j)
{
    column c;
    row r;

    intToEnum(c, r, i, j);

    turn(c, r);
}
//ne funkcii chleni dalee
void bot_gets_turn(game& test, column& c, row& r)
{
    int bdiff = -65;

    for (column i = A; i < RB_C; ++i)
        for (row j = one; j < RB_R; ++j)
        {
            test.back_up();
            if (test.turn(i, j) == -1)
                continue;
            int t = test.get_p2c() - test.get_p1c();
            if (t > bdiff)
            {
                bdiff = t;
                c = i;
                r = j;
            }
            test.rollback();
        }
}

void bot_makes_turn(game& test)
{
    column c;
    row r;

    bot_gets_turn(test, c, r);
    test.rollback(); // ne znau pochemu no inogda nado..
    test.turn(c, r);
}

//
