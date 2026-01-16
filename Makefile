ASM = fasm
SRC = src
DIST = dist

all: $(DIST)/tetris.com

$(DIST)/tetris.com: $(DIST) $(SRC)/tetris.asm
	$(ASM) $(SRC)/tetris.asm $(DIST)/tetris.com

$(DIST):
	mkdir -p $(DIST)

clean:
	rm -r $(DIST)
