#include "GameLoop.h"
using namespace std;

GameLoop::GameLoop(string playerName, int startingChips)
    : human(playerName, startingChips), bot("Bot", startingChips),
      pot(0), round_count(0), smallBlind(5), bigBlind(10),
      humanIsDealer(true), botHandVisible(false),
      phase(Phase::ROUND_OVER), street(0),
      humanPaid(0), botPaid(0), toMatch(0),
      humanDone(false), botDone(false), humanFirst(true), raises(0),
#ifdef PLATFORM_WEB
      mc(8000)   // fewer sims on web to avoid blocking the main thread
#else
      mc(100000)
#endif
{}

void GameLoop::startNewRound()
{
    if (human.getChips() <= 0 || bot.getChips() <= 0)
    {
        lastResult = human.getChips() <= 0 ? "Bot wins the game!" : "You win the game!";
        phase = Phase::GAME_OVER;
        return;
    }

    human.resetForNewRound();
    bot.resetForNewRound();
    table.clear();
    deck.reset();
    pot = 0;
    botHandVisible = false;
    messages.clear();
    raises = 0;
    street = 0;

    human.receiveCard(deck.deal());
    human.receiveCard(deck.deal());
    bot.receiveCard(deck.deal());
    bot.receiveCard(deck.deal());

    if (humanIsDealer)
    {
        human.bet(smallBlind); humanPaid = smallBlind;
        bot.bet(bigBlind);     botPaid   = bigBlind;
        messages.push_back("You post SB (" + to_string(smallBlind) + "), Bot posts BB (" + to_string(bigBlind) + ")");
    }
    else
    {
        bot.bet(smallBlind);   botPaid   = smallBlind;
        human.bet(bigBlind);   humanPaid = bigBlind;
        messages.push_back("Bot posts SB (" + to_string(smallBlind) + "), You post BB (" + to_string(bigBlind) + ")");
    }
    pot    += smallBlind + bigBlind;
    toMatch = bigBlind;
    humanDone = false;
    botDone   = false;

    // Dealer (small blind) acts first pre-flop in heads-up
    humanFirst = humanIsDealer;
    phase = humanFirst ? Phase::HUMAN_ACTION : Phase::BOT_ACTION;
}

bool GameLoop::canCheck() const
{
    return (toMatch - humanPaid) <= 0;
}

int GameLoop::getCallAmount() const
{
    return max(0, toMatch - humanPaid);
}

// ── human actions ────────────────────────────────────────────────────────────

void GameLoop::humanFold()
{
    human.fold();
    bot.winChips(pot);
    lastResult = "You folded. Bot wins " + to_string(pot) + " chips!";
    pot = 0;
    round_count++;
    humanIsDealer = !humanIsDealer;
    phase = Phase::ROUND_OVER;
}

void GameLoop::humanCheck()
{
    if (!canCheck()) return;
    humanDone = true;
    checkStreetDone();
}

void GameLoop::humanCall()
{
    int toCall = min(getCallAmount(), human.getChips());
    human.bet(toCall); pot += toCall; humanPaid += toCall;
    bot.updateAggression(false);
    humanDone = true;
    checkStreetDone();
}

void GameLoop::humanRaise(int amount)
{
    if (raises >= MAX_RAISES) return;
    amount = max(amount, bigBlind);
    int total = min(getCallAmount() + amount, human.getChips());
    human.bet(total); pot += total; humanPaid += total;
    toMatch = humanPaid;
    bot.updateAggression(false);
    humanDone = true;
    botDone   = false;
    raises++;
    // bot must respond
    phase = Phase::BOT_ACTION;
}

// ── bot action ────────────────────────────────────────────────────────────────

void GameLoop::runBotAction()
{
    int botToCall = max(0, toMatch - botPaid);
    float winPct  = mc.simulate(bot.getHand(), table);
    string action = bot.decideAction(table, pot, winPct, max(botToCall, 1));

    if (botToCall <= 0 && (action == "call" || action == "fold")) action = "check";
    if (raises >= MAX_RAISES && action == "raise") action = "call";

    if (action == "fold")
    {
        botHandVisible = true;
        human.winChips(pot);
        lastResult = "Bot folds! You win " + to_string(pot) + " chips! (Bot had: " + bot.getHand().toString() + ")";
        pot = 0;
        round_count++;
        humanIsDealer = !humanIsDealer;
        phase = Phase::ROUND_OVER;
    }
    else if (action == "check")
    {
        messages.push_back("Bot checks.");
        botDone = true;
        checkStreetDone();
    }
    else if (action == "call")
    {
        int paying = min(botToCall, bot.getChips());
        bot.bet(paying); pot += paying; botPaid += paying;
        bot.updateAggression(false);
        messages.push_back("Bot calls " + to_string(paying) + ".");
        botDone = true;
        checkStreetDone();
    }
    else if (action == "raise")
    {
        int raiseSize = max(bigBlind * 2, toMatch);
        int total     = min(botToCall + raiseSize, bot.getChips());
        bot.bet(total); pot += total; botPaid += total;
        toMatch = botPaid;
        bot.updateAggression(true);
        messages.push_back("Bot raises " + to_string(total) + ".");
        botDone   = true;
        humanDone = false;
        raises++;
        phase = Phase::HUMAN_ACTION;
    }
}

// ── internal helpers ──────────────────────────────────────────────────────────

void GameLoop::checkStreetDone()
{
    if (humanDone && botDone)
    {
        advanceStreet();
    }
    else
    {
        phase = humanDone ? Phase::BOT_ACTION : Phase::HUMAN_ACTION;
    }
}

void GameLoop::advanceStreet()
{
    street++;
    raises    = 0;
    humanPaid = 0;
    botPaid   = 0;
    toMatch   = 0;
    humanDone = false;
    botDone   = false;

    if (street == 1)
    {
        table.dealFlop(deck);
        messages.push_back("--- Flop ---");
    }
    else if (street == 2)
    {
        table.dealTurn(deck);
        messages.push_back("--- Turn ---");
    }
    else if (street == 3)
    {
        table.dealRiver(deck);
        messages.push_back("--- River ---");
    }
    else
    {
        doShowdown();
        return;
    }

    // Post-flop: non-dealer acts first
    humanFirst = !humanIsDealer;
    phase = humanFirst ? Phase::HUMAN_ACTION : Phase::BOT_ACTION;
}

void GameLoop::doShowdown()
{
    botHandVisible = true;
    HandEvaluator eval;
    long long humanScore = eval.evaluate(table, human.getHand());
    long long botScore   = eval.evaluate(table, bot.getHand());

    if (humanScore > botScore)
    {
        human.winChips(pot);
        lastResult = "You win the pot of " + to_string(pot) + "!";
    }
    else if (botScore > humanScore)
    {
        bot.winChips(pot);
        lastResult = "Bot wins the pot of " + to_string(pot) + "!";
    }
    else
    {
        human.winChips(pot / 2);
        bot.winChips(pot - pot / 2);
        lastResult = "Tie! Pot of " + to_string(pot) + " split.";
    }
    pot = 0;
    round_count++;
    humanIsDealer = !humanIsDealer;
    phase = Phase::SHOWDOWN;
}
