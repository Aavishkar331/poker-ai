#include"Bot.h"
#include<random>
using namespace std;

static mt19937 rng(random_device{}());

Bot::Bot(string name, int chips):Player(name,chips){}

string Bot::decideAction(const Table& t, int pot, float winPct, int callAmount)
{
    // pot odds: equity needed to break even — but cap at 0.30 to account for
    // implied odds (future streets where we can win a bigger pot if we hit)
    float potOdds = (float)callAmount / (pot + callAmount);
    float callThreshold = min(potOdds, 0.30f);

    // bluff frequency scales with aggression (5%–20% of hands)
    float bluffChance = 0.05f + agressionTracker * 0.15f;
    uniform_real_distribution<float> dist(0.0f, 1.0f);
    bool bluffing = dist(rng) < bluffChance;

    float raiseThreshold = 0.55f - (agressionTracker - 0.5f) * 0.2f;

    if (bluffing)
    {
        return dist(rng) < 0.6f ? "raise" : "call";
    }

    if (winPct > raiseThreshold) return "raise";
    if (winPct > callThreshold)  return "call";
    return "fold";
}

void Bot::updateAggression(bool opponentRaised)
{
    if (opponentRaised)
        agressionTracker = min(1.0f, agressionTracker + 0.05f);
    else
        agressionTracker = max(0.0f, agressionTracker - 0.05f);
}

float Bot::getAggressionTracker()const
{
    return Bot::agressionTracker;
}
