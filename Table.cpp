#include"Table.h"

using namespace std;

void Table::addCard(Card c)
{
    if(onTable.size()<5)
    {
        Table::onTable.push_back(c);
    }
}

vector<Card> Table::getCards()const
{
    return Table::onTable;
}

string Table::toString()const
{
    string s = "";
    if(Table::onTable.size()<3)
    {
        s = "Empty table";
        return s;
    }
    for (int i = 0; i < Table::onTable.size()-1; i++)
    {
        s += Table::onTable[i].toString() + " ";
    }
    s += Table::onTable[Table::onTable.size()-1].toString();
    return s;
}

void Table::clear()
{
    Table::onTable.clear();
}

int Table::size()const
{
    return Table::onTable.size();
}

void Table::dealFlop(Deck &d)
{
    Table::addCard(d.deal());
    Table::addCard(d.deal());
    Table::addCard(d.deal());
}

void Table::dealTurn(Deck &d)
{
    Table::addCard(d.deal());
}

void Table::dealRiver(Deck &d)
{
    Table::addCard(d.deal());
}