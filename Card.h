#ifndef CARD_H
#define CARD_H
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include<map>
#include<set>
#include<iostream>

class Card
{
    private:
        int rank;
        int suit;

    public:
        Card(int r, int s)
        {
            rank = r;
            suit = s;
        }
        int getRank()const
        {
            return rank;
        }
        int getSuit()const
        {
            return suit;
        }
        std::string toString()const;
};
#endif