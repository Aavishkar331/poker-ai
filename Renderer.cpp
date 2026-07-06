#include "Renderer.h"
#include <raylib.h>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
using namespace std;

// SW/SH resolve to runtime window size — fills any viewport cleanly
#define SW GetScreenWidth()
#define SH GetScreenHeight()

// ─── palette ──────────────────────────────────────────────────────────────────
static const Color C_BG      = {8,   14,  26,  255};  // deep navy bg
static const Color C_WD      = {42,  26,  9,   255};  // dark walnut
static const Color C_WDL     = {72,  48,  16,  255};  // lighter walnut edge
static const Color C_RAIL    = {162, 124, 36,  255};  // brass rail
static const Color C_RAILG   = {210, 170, 60,  255};  // brass highlight
static const Color C_FE      = {10,  48,  18,  255};  // felt edge (darkest)
static const Color C_FM      = {14,  68,  26,  255};  // felt mid
static const Color C_FI      = {19,  88,  38,  255};  // felt inner
static const Color C_FC      = {25,  106, 50,  255};  // felt centre (spotlight)
static const Color C_CARD    = {253, 250, 244, 255};  // warm white card
static const Color C_CARDB   = {202, 196, 185, 255};  // card border
static const Color C_RED     = {182, 20,  34,  255};  // suit red
static const Color C_BLK     = {14,  14,  24,  255};  // suit black
static const Color C_GOLD    = {215, 172, 52,  255};  // gold accent
static const Color C_GOLD2   = {245, 210, 100, 255};  // bright gold
static const Color C_TEXT    = {222, 218, 210, 255};  // warm white text
static const Color C_DIM     = {120, 114, 100, 255};  // dim text
static const Color C_PANEL   = {0,   0,   0,   155};  // frosted panel
static const Color C_FOLD    = {142, 16,  16,  255};  // fold red
static const Color C_CALL    = {15,  72,  148, 255};  // call blue
static const Color C_RAISE   = {14,  115, 44,  255};  // raise green
static const Color C_CTRL    = {40,  46,  62,  255};  // +/- button
static const Color C_HOVER   = {225, 182, 58,  255};  // hover gold
static const Color C_OVR     = {4,   8,   18,  195};  // overlay (semi-transparent)
static const Color C_PANELB  = {12,  20,  40,  248};  // result panel body
static const Color C_WIN     = {65,  194, 74,  255};  // win green
static const Color C_LOSE    = {214, 48,  48,  255};  // lose red
static const Color C_TIE     = {170, 148, 68,  255};  // tie gold-grey

// ─── constructor / destructor ─────────────────────────────────────────────────
Renderer::Renderer(string playerName, int startingChips)
    : game(playerName, startingChips), raiseAmount(20), botTimer(0.f)
{}

Renderer::~Renderer()
{
    UnloadFont(fontSm);
    UnloadFont(font);
    UnloadFont(fontMd);
    UnloadFont(fontLg);
}

// ─── helpers ──────────────────────────────────────────────────────────────────
Color Renderer::suitColor(int suit)
{
    return (suit == 1 || suit == 2) ? C_RED : C_BLK;
}

const char* Renderer::suitSymbol(int suit)
{
    switch (suit) {
        case 0: return "\xe2\x99\xa3";   // ♣
        case 1: return "\xe2\x99\xa6";   // ♦
        case 2: return "\xe2\x99\xa5";   // ♥
        default: return "\xe2\x99\xa0";  // ♠
    }
}

const char* Renderer::rankStr(int rank)
{
    switch (rank) {
        case 14: return "A"; case 13: return "K"; case 12: return "Q";
        case 11: return "J"; case 10: return "10"; default: break;
    }
    static char buf[3];
    snprintf(buf, sizeof(buf), "%d", rank);
    return buf;
}

const char* Renderer::streetName()
{
    int n = (int)game.getTable().getCards().size();
    if (n == 0) return "PRE-FLOP";
    if (n == 3) return "FLOP";
    if (n == 4) return "TURN";
    return "RIVER";
}

void Renderer::drawChip(float cx, float cy, float r, Color c)
{
    DrawCircleV({cx, cy}, r,       c);
    DrawCircleV({cx, cy}, r*0.66f, Fade(WHITE, 0.10f));
    DrawCircleLinesV({cx, cy}, r,       Fade(WHITE, 0.45f));
    DrawCircleLinesV({cx, cy}, r*0.66f, Fade(WHITE, 0.18f));
}

// button — returns true if clicked this frame
bool Renderer::button(const char* label, Rectangle r, Color bg)
{
    Vector2 m   = GetMousePosition();
    bool    hov = CheckCollisionPointRec(m, r);
    Color   dc  = hov ? C_HOVER : bg;

    // Drop shadow
    DrawRectangleRounded({r.x+2, r.y+4, r.width, r.height}, 0.38f, 8, Fade(BLACK, 0.45f));
    // Fill
    DrawRectangleRounded(r, 0.38f, 8, dc);
    // Top gloss strip
    DrawRectangleRounded({r.x+2, r.y+2, r.width-4, r.height*0.42f}, 0.38f, 6,
                         Fade(WHITE, hov ? 0.20f : 0.13f));
    // Border
    DrawRectangleRoundedLinesEx(r, 0.38f, 8, 1.5f, Fade(WHITE, hov ? 0.55f : 0.22f));

    Color tc = hov ? C_BLK : C_TEXT;
    Vector2 sz = MeasureTextEx(font, label, 21, 1);
    DrawTextEx(font, label, {r.x + r.width/2 - sz.x/2, r.y + r.height/2 - sz.y/2}, 21, 1, tc);

    return hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ─── table ────────────────────────────────────────────────────────────────────
void Renderer::drawTable()
{
    float cx = SW / 2.f;
    float cy = SH * 0.44f;
    float rx = SW * 0.40f;
    float ry = SH * 0.39f;

    DrawRectangle(0, 0, SW, SH, C_BG);

    // Wood border — two ellipses for edge highlight
    DrawEllipse(cx, cy, rx+28, ry+22, C_WD);
    DrawEllipse(cx, cy, rx+22, ry+17, C_WDL);

    // Brass rail — 5-pass to simulate thickness
    for (int i = -2; i <= 2; i++)
        DrawEllipseLines(cx, cy, rx+13+(float)i*0.7f, ry+9+(float)i*0.45f,
                         Fade(C_RAIL, 0.55f + fabsf((float)i)*0.06f));
    DrawEllipseLines(cx, cy, rx+13, ry+9, C_RAILG);

    // Felt — 4 concentric ellipses: dark edge → bright spotlight centre
    DrawEllipse(cx, cy, rx,       ry,       C_FE);
    DrawEllipse(cx, cy, rx*0.82f, ry*0.82f, C_FM);
    DrawEllipse(cx, cy, rx*0.60f, ry*0.60f, C_FI);
    DrawEllipse(cx, cy, rx*0.34f, ry*0.34f, C_FC);

    // Inner shadow along the rail
    DrawEllipseLines(cx, cy, rx-1, ry-1, Fade(BLACK, 0.25f));
    DrawEllipseLines(cx, cy, rx,   ry,   Fade(BLACK, 0.18f));
}

// ─── card ─────────────────────────────────────────────────────────────────────
void Renderer::drawCard(Card c, float x, float y, bool faceDown)
{
    // Layered drop shadow
    DrawRectangleRounded({x+4, y+6, (float)CW, (float)CH}, 0.10f, 8, {0,0,0,55});
    DrawRectangleRounded({x+2, y+3, (float)CW, (float)CH}, 0.10f, 8, {0,0,0,35});

    Rectangle r = {x, y, (float)CW, (float)CH};

    if (faceDown)
    {
        DrawRectangleRounded(r, 0.10f, 8, {28, 48, 118, 255});
        DrawRectangleRoundedLinesEx(r, 0.10f, 8, 2.f, {62, 92, 170, 255});
        // Subtle crosshatch
        for (int i = 10; i < CH-4; i += 13)
            DrawLineEx({x+4, y+(float)i}, {x+CW-4, y+(float)i}, 0.5f, Fade(WHITE, 0.06f));
        for (int i = 10; i < CW-4; i += 13)
            DrawLineEx({x+(float)i, y+4}, {x+(float)i, y+CH-4}, 0.5f, Fade(WHITE, 0.06f));
        // Diamond centre
        float dcx = x + CW/2.f, dcy = y + CH/2.f;
        Vector2 pts[4] = {{dcx, dcy-15},{dcx+15,dcy},{dcx,dcy+15},{dcx-15,dcy}};
        for (int i = 0; i < 4; i++)
            DrawLineEx(pts[i], pts[(i+1)%4], 1.2f, Fade(WHITE, 0.20f));
        DrawCircleV({dcx, dcy}, 4, Fade(WHITE, 0.14f));
        return;
    }

    // Face-up card
    DrawRectangleRounded(r, 0.10f, 8, C_CARD);
    DrawRectangleRoundedLinesEx(r, 0.10f, 8, 1.5f, C_CARDB);

    Color       sc = suitColor(c.getSuit());
    const char* rs = rankStr(c.getRank());
    const char* ss = suitSymbol(c.getSuit());

    // Corner: rank
    DrawTextEx(fontMd, rs, {x+6,  y+4},  22, 0, sc);
    // Corner: suit below rank
    DrawTextEx(font,   ss, {x+7,  y+28}, 17, 0, sc);

    // Centre large suit
    Vector2 csz = MeasureTextEx(fontLg, ss, 46, 0);
    DrawTextEx(fontLg, ss,
               {x + CW/2.f - csz.x/2.f, y + CH/2.f - csz.y/2.f}, 46, 0, sc);

    // Bottom-right mirrored
    Vector2 rw = MeasureTextEx(fontMd, rs, 22, 0);
    Vector2 sw2= MeasureTextEx(font,   ss, 17, 0);
    DrawTextEx(fontMd, rs, {x+CW-6-rw.x,  y+CH-28}, 22, 0, sc);
    DrawTextEx(font,   ss, {x+CW-7-sw2.x, y+CH-46}, 17, 0, sc);
}

void Renderer::drawCardRow(vector<Card> cards, float cx, float cy, bool faceDown)
{
    int n = (int)cards.size();
    if (n == 0) return;
    float gap   = 14.f;
    float total = n*CW + (n-1)*gap;
    float sx    = cx - total/2.f;
    for (int i = 0; i < n; i++)
        drawCard(cards[i], sx + i*(CW+gap), cy, faceDown);
}

// ─── player info panel ────────────────────────────────────────────────────────
void Renderer::drawPlayerPanel(float cx, float cy, const string& name, int chips, bool dealer)
{
    float pw = 230.f, ph = 46.f;
    Rectangle r = {cx-pw/2, cy, pw, ph};

    // Panel
    DrawRectangleRounded(r, 0.35f, 8, C_PANEL);
    DrawRectangleRoundedLinesEx(r, 0.35f, 8, 1.f, Fade(C_RAIL, 0.30f));

    // Name (left)
    DrawTextEx(font, name.c_str(), {r.x+14, cy+ph/2-11}, 21, 1, C_TEXT);

    // Chip icon + count (right)
    float iconX = cx+pw/2-50;
    drawChip(iconX, cy+ph/2, 9, C_GOLD);
    string cs = to_string(chips);
    Vector2 csz = MeasureTextEx(font, cs.c_str(), 21, 1);
    DrawTextEx(font, cs.c_str(), {iconX+14, cy+ph/2-csz.y/2}, 21, 1, C_GOLD2);

    // Dealer button
    if (dealer)
    {
        float dx = cx+pw/2+22, dy = cy+ph/2;
        DrawCircleV({dx, dy}, 14, C_GOLD);
        DrawCircleLinesV({dx, dy}, 14, Fade(WHITE, 0.35f));
        Vector2 dsz = MeasureTextEx(fontSm, "D", 16, 0);
        DrawTextEx(fontSm, "D", {dx-dsz.x/2, dy-dsz.y/2}, 16, 0, C_BLK);
    }
}

// ─── bot area ─────────────────────────────────────────────────────────────────
void Renderer::drawBotArea()
{
    float cx  = SW / 2.f;
    float top = SH * 0.44f - SH*0.39f; // top edge of table ellipse

    drawPlayerPanel(cx, top - 56, "Bot", game.getBotChips(), !game.isHumanDealer());

    auto cards = game.getBotHand().getCards();
    if (!cards.empty())
        drawCardRow(cards, cx, top - 56 + 46 + 8, !game.isBotHandVisible());

    // Thinking indicator
    if (game.getPhase() == Phase::BOT_ACTION)
    {
        int   dots  = (int)(GetTime() * 2.4f) % 4;
        float pulse = 0.55f + 0.35f * sinf(GetTime() * 4.2f);
        string t = "thinking";
        for (int i = 0; i < dots; i++) t += ".";
        Vector2 tsz = MeasureTextEx(fontSm, t.c_str(), 16, 1);
        float   tx  = cx - tsz.x/2, ty = top - 56 + 46 + 8 + CH + 10;
        DrawRectangleRounded({tx-10, ty-4, tsz.x+20, tsz.y+8}, 0.5f, 6, Fade(BLACK, 0.5f));
        DrawTextEx(fontSm, t.c_str(), {tx, ty}, 16, 1, Fade(C_GOLD, pulse));
    }
}

// ─── centre area (community cards + pot + street) ─────────────────────────────
void Renderer::drawCenterArea()
{
    float cx  = SW / 2.f;
    float cy  = SH * 0.44f;
    float ry  = SH * 0.39f;

    // Street badge
    const char* sn  = streetName();
    Vector2     snz = MeasureTextEx(fontSm, sn, 14, 2);
    float bw = snz.x+22, bh = 24;
    float bx = cx-bw/2, by = cy - ry*0.86f - bh/2;
    DrawRectangleRounded({bx, by, bw, bh}, 0.5f, 6, Fade(C_BLK, 0.72f));
    DrawRectangleRoundedLinesEx({bx, by, bw, bh}, 0.5f, 6, 1.f, Fade(C_RAIL, 0.55f));
    DrawTextEx(fontSm, sn, {bx+11, by+bh/2-snz.y/2}, 14, 2, C_GOLD);

    // Community cards
    auto cards = game.getTable().getCards();
    if (!cards.empty())
        drawCardRow(cards, cx, cy - CH/2.f, false);

    // Pot — large gold number, centred below cards
    float poty = cards.empty() ? cy - 26 : cy + CH/2.f + 18;
    string pot = to_string(game.getPot());
    Vector2 lsz = MeasureTextEx(fontSm, "POT", 13, 2);
    Vector2 psz = MeasureTextEx(fontLg, pot.c_str(), 46, 1);
    DrawTextEx(fontSm, "POT",        {cx - lsz.x/2, poty},      13, 2, C_DIM);
    DrawTextEx(fontLg, pot.c_str(), {cx - psz.x/2, poty + 16}, 46, 1, C_GOLD);
}

// ─── player (human) area ──────────────────────────────────────────────────────
void Renderer::drawHumanArea()
{
    float cx   = SW / 2.f;
    float tbot = SH * 0.44f + SH * 0.39f;

    auto cards = game.getHumanHand().getCards();
    if (!cards.empty())
        drawCardRow(cards, cx, tbot - CH - 20, false);

    drawPlayerPanel(cx, tbot + 10, "You", game.getHumanChips(), game.isHumanDealer());
}

// ─── message log ──────────────────────────────────────────────────────────────
void Renderer::drawMessageLog()
{
    const auto& msgs = game.getMessages();
    int   n    = (int)msgs.size();
    int   show = min(n, 5);
    float y0   = SH - 68.f - show * 20.f;
    for (int i = n - show; i < n; i++)
    {
        int   idx = i - (n - show);
        float a   = 0.28f + 0.55f * (idx / (float)max(show-1, 1));
        DrawTextEx(fontSm, msgs[i].c_str(), {16, y0 + idx*20.f}, 16, 1, Fade(C_DIM, a));
    }
    // Round number top-left
    string rnd = "Round " + to_string(game.getRoundCount() + 1);
    DrawTextEx(fontSm, rnd.c_str(), {16, 14}, 16, 1, Fade(C_DIM, 0.65f));
}

// ─── action buttons ───────────────────────────────────────────────────────────
void Renderer::drawActionButtons()
{
    float bh   = 52.f;
    float by   = SH - bh - 14.f;
    // Row: [Fold 148] [8] [Check/Call 158] [8] [- 40] [6] [amt 76] [6] [+ 40] [8] [Raise 128]
    // Total = 626
    float bx   = SW / 2.f - 313.f;

    if (button("Fold", {bx, by, 148, bh}, C_FOLD))
        game.humanFold();
    bx += 156;

    if (game.canCheck()) {
        if (button("Check", {bx, by, 158, bh}, C_CALL))
            game.humanCheck();
    } else {
        string cl = "Call  " + to_string(game.getCallAmount());
        if (button(cl.c_str(), {bx, by, 158, bh}, C_CALL))
            game.humanCall();
    }
    bx += 166;

    // [-] amount [+] Raise
    if (button("-", {bx, by, 40, bh}, C_CTRL))
        raiseAmount = max(10, raiseAmount - 10);
    bx += 46;

    string rl = to_string(raiseAmount);
    Vector2 rlsz = MeasureTextEx(font, rl.c_str(), 21, 1);
    DrawRectangleRounded({bx, by, 76, bh}, 0.28f, 6, Fade(BLACK, 0.52f));
    DrawRectangleRoundedLinesEx({bx, by, 76, bh}, 0.28f, 6, 1.f, Fade(C_RAIL, 0.38f));
    DrawTextEx(font, rl.c_str(), {bx+38-rlsz.x/2, by+bh/2-rlsz.y/2}, 21, 1, C_TEXT);
    bx += 82;

    if (button("+", {bx, by, 40, bh}, C_CTRL))
        raiseAmount += 10;
    bx += 48;

    if (button("Raise", {bx, by, 128, bh}, C_RAISE))
        game.humanRaise(raiseAmount);
}

// ─── result overlay ───────────────────────────────────────────────────────────
void Renderer::drawResultOverlay()
{
    DrawRectangle(0, 0, SW, SH, C_OVR);

    Phase  p   = game.getPhase();
    string res = game.getLastResult();

    // Pick accent colour based on outcome
    Color rc = C_GOLD;
    if (res.rfind("You win", 0) == 0 || res.rfind("You w", 0) == 0) rc = C_WIN;
    else if (res.rfind("Bot wins", 0) == 0 || res.rfind("Bot w", 0) == 0) rc = C_LOSE;
    else if (res.rfind("Tie", 0) == 0) rc = C_TIE;

    float pw = fminf(SW * 0.54f, 680.f);
    float ph = 260.f;
    float px = SW/2.f - pw/2;
    float py = SH/2.f - ph/2;

    // Panel shadow
    DrawRectangleRounded({px+4, py+7, pw, ph}, 0.12f, 8, Fade(BLACK, 0.55f));
    // Panel body
    DrawRectangleRounded({px, py, pw, ph}, 0.12f, 8, C_PANELB);
    DrawRectangleRoundedLinesEx({px, py, pw, ph}, 0.12f, 8, 2.f, Fade(C_RAIL, 0.5f));
    // Accent stripe at top
    DrawRectangleRounded({px, py, pw, 5}, 0.2f, 4, rc);

    // Result text — scale down if too wide
    float fs  = 38.f;
    Vector2 rsz = MeasureTextEx(fontLg, res.c_str(), fs, 1);
    if (rsz.x > pw - 40) fs = fs * (pw - 40) / rsz.x;
    rsz = MeasureTextEx(fontLg, res.c_str(), fs, 1);
    DrawTextEx(fontLg, res.c_str(), {SW/2.f - rsz.x/2, py + 30}, fs, 1, rc);

    // Divider
    DrawLineEx({px+28, py+100}, {px+pw-28, py+100}, 1.f, Fade(C_RAIL, 0.28f));

    // Show both hands at showdown
    if (p == Phase::SHOWDOWN)
    {
        // Scale cards to fit in panel
        float scale = fminf(1.f, (pw - 80) / (4*CW + 3*14 + 60));
        float scw = CW * scale, sch = CH * scale;
        float gap = 14 * scale;

        float handsY = py + 116;
        float leftCX  = SW/2.f - scw*1.5f - gap*0.5f - 28;
        float rightCX = SW/2.f + scw*1.5f + gap*0.5f + 28;

        // Labels
        Vector2 bl = MeasureTextEx(fontSm, "Bot", 15, 1);
        Vector2 yl = MeasureTextEx(fontSm, "You", 15, 1);
        DrawTextEx(fontSm, "Bot", {leftCX  - (scw+gap) - bl.x/2 + scw/2, handsY + sch + 6}, 15, 1, C_DIM);
        DrawTextEx(fontSm, "You", {rightCX - (scw+gap) - yl.x/2 + scw/2, handsY + sch + 6}, 15, 1, C_DIM);

        auto bCards = game.getBotHand().getCards();
        auto hCards = game.getHumanHand().getCards();

        // Draw scaled cards manually for bot
        for (int i = 0; i < (int)bCards.size(); i++)
        {
            float cx2 = leftCX  - (scw+gap)/2.f + i*(scw+gap);
            drawCard(bCards[i], cx2, handsY, false);
        }
        // Draw scaled cards for human
        for (int i = 0; i < (int)hCards.size(); i++)
        {
            float cx2 = rightCX - (scw+gap)/2.f + i*(scw+gap);
            drawCard(hCards[i], cx2, handsY, false);
        }

        // "vs" divider
        Vector2 vsz = MeasureTextEx(font, "vs", 19, 1);
        DrawTextEx(font, "vs", {SW/2.f - vsz.x/2, handsY + sch/2 - vsz.y/2}, 19, 1, C_DIM);
    }

    // Next Round / Game Over button
    float bw = 188, bh = 50;
    float bbx = SW/2.f - bw/2, bby = py + ph - bh - 18;
    const char* lbl = (p == Phase::GAME_OVER) ? "Game Over" : "Next Round";
    Color bc = (p == Phase::GAME_OVER) ? C_CTRL : C_CALL;

    if (button(lbl, {bbx, bby, bw, bh}, bc) && p != Phase::GAME_OVER)
    {
        game.clearMessages();
        game.startNewRound();
        raiseAmount = 20;
        botTimer    = 0.f;
    }
}

// ─── per-frame tick ────────────────────────────────────────────────────────────
void Renderer::tick()
{
    float dt = GetFrameTime();
    Phase p  = game.getPhase();

    if (p == Phase::BOT_ACTION)
    {
        botTimer += dt;
        if (botTimer >= BOT_DELAY)
        {
            game.runBotAction();
            botTimer = 0.f;
        }
    }

    BeginDrawing();

    drawTable();
    drawBotArea();
    drawCenterArea();
    drawHumanArea();
    drawMessageLog();

    p = game.getPhase();
    if (p == Phase::HUMAN_ACTION)
        drawActionButtons();
    if (p == Phase::SHOWDOWN || p == Phase::ROUND_OVER || p == Phase::GAME_OVER)
        drawResultOverlay();

    EndDrawing();
}

// ─── main loop ────────────────────────────────────────────────────────────────
#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#include <emscripten.h>
static Renderer* s_instance = nullptr;
static void webTick() { s_instance->tick(); }
#endif

void Renderer::run()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);

#ifdef PLATFORM_WEB
    int initW = EM_ASM_INT({ return window.innerWidth; });
    int initH = EM_ASM_INT({ return window.innerHeight; });
    InitWindow(initW, initH, "Poker");
#else
    InitWindow(1280, 720, "Poker");
#endif

    SetTargetFPS(60);

    // Load Unicode font with suit codepoints
    static int cp[260];
    for (int i = 0; i < 256; i++) cp[i] = 32 + i;
    cp[256] = 9824; cp[257] = 9827; cp[258] = 9829; cp[259] = 9830;

    const char* fontPath = "DejaVuSans.ttf";
    fontSm = LoadFontEx(fontPath, 18, cp, 260);
    font   = LoadFontEx(fontPath, 24, cp, 260);
    fontMd = LoadFontEx(fontPath, 34, cp, 260);
    fontLg = LoadFontEx(fontPath, 52, cp, 260);
    SetTextureFilter(fontSm.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(font.texture,   TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(fontMd.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(fontLg.texture, TEXTURE_FILTER_BILINEAR);

    game.startNewRound();

#ifdef PLATFORM_WEB
    s_instance = this;
    emscripten_set_main_loop(webTick, 0, 1);
#else
    while (!WindowShouldClose())
        tick();
    UnloadFont(fontSm);
    UnloadFont(font);
    UnloadFont(fontMd);
    UnloadFont(fontLg);
    CloseWindow();
#endif
}
