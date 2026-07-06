#ifndef DECK_H
#define DECK_H
#include "Card.h"

class Deck
{
    private:
        std::vector<Card>cards;
        int top;

    public:
        Deck();
        void shuffleDeck();
        Card deal();
        void reset();
        int remaining() const;
};
#endif