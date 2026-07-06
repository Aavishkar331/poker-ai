#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "Player.h"
#include "Bot.h"
#include "Deck.h"
#include "Table.h"
#include "MonteCarlo.h"
#include "HandEvaluator.h"
#include <string>
#include <vector>

enum class Phase {
    HUMAN_ACTION,  // waiting for human to act
    BOT_ACTION,    // renderer should call runBotAction()
    SHOWDOWN,      // cards revealed, show result
    ROUND_OVER,    // someone folded, show result
    GAME_OVER      // someone is out of chips
};

class GameLoop
{
private:
    Player human;
    Bot bot;
    Deck deck;
    Table table;
    MonteCarlo mc;

    int pot;
    int round_count;
    int smallBlind, bigBlind;
    bool humanIsDealer;
    bool botHandVisible;

    // Betting state for current street
    Phase phase;
    int humanPaid, botPaid, toMatch;
    bool humanDone, botDone;
    bool humanFirst;
    int raises;
    static const int MAX_RAISES = 4;
    int street; // 0=preflop 1=flop 2=turn 3=river

    std::vector<std::string> messages;
    std::string lastResult;

    void checkStreetDone();
    void advanceStreet();
    void doShowdown();

public:
    GameLoop(std::string playerName, int startingChips);

    void startNewRound();

    // State queries
    Phase       getPhase()         const { return phase; }
    Hand        getHumanHand()     const { return human.getHand(); }
    Hand        getBotHand()       const { return bot.getHand(); }
    Table       getTable()         const { return table; }
    int         getPot()           const { return pot; }
    int         getHumanChips()    const { return human.getChips(); }
    int         getBotChips()      const { return bot.getChips(); }
    int         getRoundCount()    const { return round_count; }
    bool        isBotHandVisible() const { return botHandVisible; }
    bool        isHumanDealer()    const { return humanIsDealer; }
    std::string getLastResult()    const { return lastResult; }
    const std::vector<std::string>& getMessages() const { return messages; }

    bool canCheck()      const;
    int  getCallAmount() const;

    // Human actions — only valid when phase == HUMAN_ACTION
    void humanFold();
    void humanCheck();
    void humanCall();
    void humanRaise(int amount);

    // Bot action — call once per frame when phase == BOT_ACTION
    void runBotAction();

    void clearMessages() { messages.clear(); }
};

#endif
