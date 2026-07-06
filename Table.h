// Table class

// private: vector<Card> of size up to 5 (the community cards — flop/turn/river)
// public: addCard(Card), getCards(), toString(), clear(), size()


#ifndef TABLE_H
#define TABLE_H
#include"Card.h"
#include"Deck.h"

class Table
{
    private:
        std::vector<Card>onTable;
    public:
        void addCard(Card);
        std::vector<Card>getCards()const;
        std::string toString()const;
        void clear();
        int size()const;
        void dealFlop(Deck &d);
        void dealTurn(Deck &d);
        void dealRiver(Deck &d);     
};
#endif