#ifndef MONTECARLO_H
#define MONTECARLO_H
#include "HandEvaluator.h"
#include "Table.h"
#include "Hand.h"
#include "Deck.h"

class MonteCarlo 
{
    private:
        int simulations;
        HandEvaluator evaluator;

        // builds a fresh deck with known cards removed
        std::vector<Card> getRemainingDeck(const Hand& h, const Table& t);

    public:
        MonteCarlo(int sims = 100000);

        // returns win probability 0.0 to 1.0
        float simulate(const Hand& playerHand, const Table& table);
};
#endif