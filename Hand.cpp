#include "Hand.h"

using namespace std;

void Hand::addCard(Card c)
{
    if(inHand.size() < 2)
    {
        inHand.push_back(c);
    }
}

vector<Card> Hand::getCards()const
{
    return inHand;
}

string Hand::toString()const
{
    if(inHand.size()<2)
    {
        return "Empty Hand";
    }
    return inHand[0].toString() + " " + inHand[1].toString();
}

void Hand::clearHand()
{
    inHand.clear();
}