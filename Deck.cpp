#include "Deck.h"

using namespace std;
Deck::Deck()
{
    top = 0;
    for (int s = 0; s < 4; s++)
    {
        for (int r = 2; r <= 14; r++)
        {
            cards.push_back(Card(r,s));
        }
    }
    shuffleDeck();
}

void Deck::shuffleDeck()
{
    mt19937 rng(random_device{}());
    shuffle(cards.begin(),cards.end(),rng);
    top = 0;
}

Card Deck::deal()
{
    return cards[top++];
}

void Deck::reset()
{
    this->shuffleDeck();
}

int Deck::remaining()const
{
    return (cards.size()-top);
}