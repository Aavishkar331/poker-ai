#ifndef RENDERER_H
#define RENDERER_H

#include "GameLoop.h"
#include <raylib.h>
#include <string>

class Renderer
{
private:
    GameLoop game;

    // SW/SH are runtime values (browser viewport on web, 1280x720 on native)
    // accessed via GetScreenWidth()/GetScreenHeight() — see top of Renderer.cpp
    static const int CW = 88;
    static const int CH = 122;

    Font fontSm;   // 18px — labels, log, badge
    Font font;     // 24px — body, buttons
    Font fontMd;   // 34px — card corners, headers
    Font fontLg;   // 52px — pot, result headline

    // Raise UI state
    int raiseAmount;

    // Bot action delay — gives the bot a 0.8s "think" pause
    float botTimer;
    static constexpr float BOT_DELAY = 0.8f;

    // ── drawing ───────────────────────────────────────────────────────────
    void drawTable();
    void drawCard(Card c, float x, float y, bool faceDown = false);
    void drawCardRow(std::vector<Card> cards, float cx, float cy, bool faceDown = false);
    void drawBotArea();
    void drawCenterArea();
    void drawHumanArea();
    void drawActionButtons();
    void drawResultOverlay();
    void drawMessageLog();
    void drawPlayerPanel(float cx, float cy, const std::string& name, int chips, bool dealer);
    void drawChip(float cx, float cy, float r, Color c);

    bool button(const char* label, Rectangle r, Color bg);

    Color       suitColor(int suit);
    const char* suitSymbol(int suit);
    const char* rankStr(int rank);
    const char* streetName();

public:
    Renderer(std::string playerName, int startingChips);
    ~Renderer();
    void run();
    void tick(); // one frame — called by emscripten_set_main_loop on web
};

#endif
