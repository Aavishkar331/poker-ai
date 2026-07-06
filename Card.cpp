#include"Card.h"
using namespace std;

static const string RANKS[] =
{
    "","","2","3","4","5","6","7","8","9","10","J","Q","K","A"
};

static const string SUITS[] = 
{
    "♣","♦","♥","♠"
};

string Card::toString() const
{
    return RANKS[this->rank] + SUITS[this->suit];
}