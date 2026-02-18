ASM = fasm
SRC_DIR = src
DST_DIR = dist

SRC_FILE = $(SRC_DIR)/tetris.asm
DST_FILE = $(DST_DIR)/tetris.com
INC_FILES =  $(wildcard $(SRC_DIR)/*.inc)
INC_FILES += $(wildcard $(SRC_DIR)/*/*.inc)

all: $(DST_DIR)/tetris.com

$(DST_FILE): $(DST_DIR) $(SRC_FILE) $(INC_FILES)
	$(ASM) $(SRC_DIR)/tetris.asm $(DST_DIR)/tetris.com

$(DST_DIR):
	mkdir -p $(DST_DIR)

clean:
	rm -r $(DST_DIR)
