// Player class

// private: Hand, int chips, string name
// public: getHand(), getChips(), bet(int), fold(), isFolded()
#ifndef PLAYER_H
#define PLAYER_H
#include "Hand.h"

class Player
{
    private:
        Hand h;
        int chips;
        bool Folded = false;
        std::string name;
    public:
        Player(std::string playerName, int startingChips);
        Hand getHand()const;
        int getChips()const;
        std::string getName()const;
        bool isFolded();

        void bet(int amount);
        void fold();
        void winChips(int amount);
        void receiveCard(Card c);
        void resetForNewRound();
};
#endif