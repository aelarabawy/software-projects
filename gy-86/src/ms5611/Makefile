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
INCLUDE_DIR=../../include
PUB_INC_DIR=$(INCLUDE_DIR)/ms5611

## Add any more include folders here
CFLAGS= -I$(CUR_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/logger -Wall 

SRC_DIR=$(CUR_DIR)
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS=
DEP=

BIN_NAME=libms5611.a

SRC= ms5611.c
 	 
PUB_INC= ms5611_api.h \
         ms5611_common.h 	      


# No Customization below this line
#######################################
OBJ=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
_PUB_INC=$(patsubst %.h,$(SRC_DIR)/%.h,$(PUB_INC))


all: $(CUR_DIR)/$(BIN_NAME)


$(CUR_DIR)/$(BIN_NAME): $(OBJ)
	$(AR) -cvq $@ $^

$(OBJ_DIR)/%.o: %.c $(DEP)
	mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~ $(CUR_DIR)/$(BIN_NAME)


.PHONY: install
install:
	mkdir -p $(PUB_INC_DIR)
	cp $(_PUB_INC) $(PUB_INC_DIR)
	mkdir -p $(LIB_DIR)
	cp $(CUR_DIR)/$(BIN_NAME) $(LIB_DIR)

