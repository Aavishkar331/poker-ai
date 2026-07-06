#include"Player.h"

using namespace std;


Player::Player(string playerName, int startingChips)
{
    name = playerName;
    chips = startingChips;
    Folded = false;
}

Hand Player::getHand()const
{
    return h;
}

string Player::getName()const
{
    return name;
}

int Player::getChips()const
{
    return chips;
}

bool Player::isFolded()
{
    return Folded;
}

void Player::receiveCard(Card c)
{
    h.addCard(c);
}

void Player::bet(int amount)
{
    if(amount<=chips)
    {
        chips-=amount;
    }
}

void Player::fold()
{
    h.clearHand();
    Folded = true;
}

void Player::resetForNewRound()
{
    h.clearHand();
    Folded = false;
}

void Player::winChips(int amount)
{
    chips += amount;
}