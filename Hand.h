// Hand class

// private: vector<Card> of size 2 (the two hole cards a player holds)
// public: addCard(Card), getCards(), toString(), clear()

#ifndef HAND_H
#define HAND_H
#include "Card.h"

class Hand
{
    private:
        std::vector<Card>inHand;
    public:
        void addCard(Card);
        std::vector<Card> getCards()const;
        std::string toString()const;
        void clearHand();

};
#endif