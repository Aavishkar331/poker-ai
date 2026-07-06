#include "MonteCarlo.h"
using namespace std;

MonteCarlo::MonteCarlo(int sims) 
{
    simulations = sims;
}

// step 1 — build a vector of all 52 cards minus the ones we already know about
vector<Card> MonteCarlo::getRemainingDeck(const Hand& h, const Table& t)
{

    // collect all known cards into a set of (rank, suit) pairs
    set<pair<int,int>> known;
    for (const Card& c : h.getCards())
    {
        known.insert({c.getRank(), c.getSuit()});
    }
    for (const Card& c : t.getCards())
    {
        known.insert({c.getRank(), c.getSuit()});
    }

    // build remaining deck — all 52 minus known cards
    vector<Card> remaining;
    for (int s = 0; s < 4; s++) 
    {
        for (int r = 2; r <= 14; r++) 
        {
            if (known.find({r, s}) == known.end())
            {
                remaining.push_back(Card(r, s));
            }
        }
    }
    return remaining;
}

float MonteCarlo::simulate(const Hand& playerHand, const Table& table) 
{

    int wins = 0;
    mt19937 rng(random_device{}());

    vector<Card> baseDeck = getRemainingDeck(playerHand, table);
    for (int i = 0; i < simulations; i++) 
    {

        // get fresh shuffled remaining deck each simulation
        vector<Card>deck = baseDeck;
        shuffle(deck.begin(), deck.end(), rng);

        int idx = 0; // pointer into remaining deck

        // step 2 — deal 2 random cards to opponent
        Hand oppHand;
        oppHand.addCard(deck[idx++]);
        oppHand.addCard(deck[idx++]);

        // step 3 — fill missing community cards
        Table simTable = table; // copy current table state
        while (simTable.size() < 5)
        {
            simTable.addCard(deck[idx++]);
        }

        // step 4 — evaluate both hands
        long long myScore = evaluator.evaluate(simTable, playerHand);
        long long oppScore = evaluator.evaluate(simTable, oppHand);

        // step 5 — count wins
        if (myScore > oppScore)
        {
            wins++;
        }
    }

    return (float)wins / simulations;
}