ASM = fasm
SRC = src
DIST = dist

all: $(DIST)/tetris.com

$(DIST)/tetris.com: $(DIST) $(SRC)/tetris.asm $(SRC)/render.inc $(SRC)/render_text.inc $(SRC)/font8x8_basic.inc
	$(ASM) $(SRC)/tetris.asm $(DIST)/tetris.com

$(DIST):
	mkdir -p $(DIST)

clean:
	rm -r $(DIST)
