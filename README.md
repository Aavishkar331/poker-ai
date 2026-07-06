# Poker AI

Texas Hold'em heads-up poker against a Monte Carlo AI bot, built in C++ with Raylib. Playable in the browser via WebAssembly.

**[Play in browser →](https://aavishkar331.github.io/poker-ai/)**

![table](https://raw.githubusercontent.com/Aavishkar331/poker-ai/main/docs/preview.png)

---

## How the AI works

The bot runs a **Monte Carlo simulation** every time it needs to act. It:

1. Takes the current known cards (its hole cards + community cards)
2. Builds all remaining possible deck cards
3. Runs N simulated runouts — deals random opponent hands, fills missing community cards, evaluates both 7-card hands
4. Returns `wins / N` as the estimated win probability

That win probability is then fed into a decision function that weighs **pot odds**, a **bluff frequency** (scales with an aggression tracker that updates based on opponent behaviour), and raise/call thresholds.

```
winPct > raiseThreshold  →  raise
winPct > potOdds (capped at 0.30)  →  call
else  →  fold
bluffChance (5–20%)  →  random raise/call regardless of hand
```

The aggression tracker rises when the opponent raises and decays when they call/check, making the bot adapt its style over the session.

---

## Architecture

| File | Role |
|---|---|
| `Card` / `Deck` | Standard 52-card deck, mt19937 shuffle |
| `Hand` / `Table` | Hole cards and community cards |
| `HandEvaluator` | Scores all hand ranks 1–10 with full kicker tiebreaking via base-15 encoding |
| `MonteCarlo` | Win-probability engine |
| `Player` / `Bot` | Player state; Bot extends Player with `decideAction()` |
| `GameLoop` | State machine (no I/O) — `HUMAN_ACTION / BOT_ACTION / SHOWDOWN / ROUND_OVER / GAME_OVER` |
| `Renderer` | Raylib rendering — table, cards, panels, buttons, overlays |

---

## Build

### Native
```bash
make native
./poker
```
Requires Raylib installed (`/usr/local`).

### Web (WebAssembly)
```bash
# one-time: build Raylib for web
source ~/emsdk/emsdk_env.sh
make web-raylib

# then build the game
make web
# output → docs/index.html
```
Requires [emsdk](https://github.com/emscripten-core/emsdk).

---

## Controls

| Action | How |
|---|---|
| Fold | Click **Fold** |
| Check (no bet) | Click **Check** |
| Call | Click **Call N** |
| Raise | Set amount with **−/+**, click **Raise** |
