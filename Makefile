SRCS = main.cpp Card.cpp Deck.cpp Hand.cpp HandEvaluator.cpp \
       Player.cpp Table.cpp Bot.cpp MonteCarlo.cpp GameLoop.cpp Renderer.cpp

RAYLIB_SRC = $(HOME)/raylib/src

# ── native ────────────────────────────────────────────────────────────────────
native:
	g++ -std=c++17 -O2 $(SRCS) \
	    -I/usr/local/include -L/usr/local/lib \
	    -lraylib -lm -lpthread -ldl \
	    -o poker

# ── web (requires emsdk active + raylib built for web) ────────────────────────
# Step 1: run  make web-raylib  once to build raylib.a with emcc
# Step 2: run  make web         to build the game

web-raylib:
	$(MAKE) -C $(RAYLIB_SRC) PLATFORM=PLATFORM_WEB \
	    EMSDK_PATH=$(EMSDK) -j$(nproc)

web: $(SRCS)
	mkdir -p web
	emcc -std=c++17 -Os -DPLATFORM_WEB \
	    -I$(RAYLIB_SRC) \
	    $(SRCS) \
	    $(RAYLIB_SRC)/libraylib.web.a \
	    -s USE_GLFW=3 \
	    -s TOTAL_MEMORY=134217728 \
	    -s FORCE_FILESYSTEM=1 \
	    --preload-file DejaVuSans.ttf \
	    --shell-file shell.html \
	    -o web/poker.html

clean:
	rm -f poker web/poker.html web/poker.js web/poker.wasm web/poker.data

.PHONY: native web web-raylib clean
