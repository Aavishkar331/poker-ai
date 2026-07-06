#ifndef HANDEVALUATOR_H
#define HANDEVALUATOR_H
#include"Hand.h"
#include"Table.h"

class HandEvaluator
{
    private:
        std::vector<Card>totalHand;
        std::map<int,int>rankFreq;
        std::map<int,int>suitFreq;
        long long heat;
        Card highestRank = Card(0, 0);
    public:
        std::vector<Card> getTotalHand(Table t,Hand h);
        void setTotalHand(Table t,Hand h);
        std::map<int,int> getRankFreq();
        void setRankFreq();
        std::map<int,int> getSuitFreq();
        void setSuitFreq();
        bool isFlush();
        bool isStraight();
        bool isStraightFlush();
        bool isRoyalFlush();
        bool isFourOfAKind();
        bool isFullHouse();
        bool isThreeOfAKind();
        bool isTwoPair();
        bool isOnePair();
        void setHighestRank();
        long long evaluate(const Table &t,const Hand &h);
        void setHeat(const Table &t,const Hand &h);
};
#endif
