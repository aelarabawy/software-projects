#######################################
## Makefile
## Author: Ahmed ElArabawy
## Created on : 16 Nov. 2014
#######################################
# ToolChain
TOOL_CHAIN_DIR=/usr/bin
CC=$(TOOL_CHAIN_DIR)/gcc -std=gnu99
AR=$(TOOL_CHAIN_DIR)/ar
CUR_DIR=.

# Include Directory
INCLUDE_DIR=../include

## Add any more include folders here
CFLAGS= -I$(CUR_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/logger -I$(INCLUDE_DIR)/wpa-supplicant-client -Wall 

SRC_DIR=$(CUR_DIR)
LIB_DIR=../lib
BIN_DIR=../bin

OBJ_DIR=../build
LIBS= -lwpa_supplicant_client -lwpa_supplicant_dbus -llogger -lgio-2.0 -lglib-2.0 -lgobject-2.0

DEP=

BIN_NAME=wifi-mgr

SRC= main.c
	      

# No Customization below this line
#######################################
OBJ=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))


all: $(CUR_DIR)/$(BIN_NAME)

$(CUR_DIR)/$(BIN_NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -L$(LIB_DIR)

$(OBJ_DIR)/%.o: %.c $(DEP)
	mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~ $(CUR_DIR)/$(BIN_NAME)


.PHONY: install
install:
	mkdir -p $(BIN_DIR)
	cp $(CUR_DIR)/$(BIN_NAME) $(BIN_DIR)

