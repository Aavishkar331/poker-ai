// Bot class — extends Player

// private: float aggressionTracker (for opponent modeling later)
// public: decideAction(Table, int pot, float winPct) → returns "fold" / "call" / "raise"


#ifndef BOT_H
#define BOT_H
#include "Player.h"
#include "Table.h"

class Bot:public Player
{
    private:
        float agressionTracker = 0.5f;
    public:
        Bot(std::string name, int chips);
        std::string decideAction(const Table &t,int pot,float winPct,int callAmount);
        void updateAggression(bool opponentRaised);  
        // call this every time opponent acts
        // if raised → aggressionTracker goes up
        // if folded/called → goes down
        float getAggressionTracker() const;

};
#endif