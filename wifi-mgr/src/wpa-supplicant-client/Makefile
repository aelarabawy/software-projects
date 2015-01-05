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
PUB_INC_DIR=$(INCLUDE_DIR)/wpa-supplicant-client

## Add any more include folders here
CFLAGS= -I$(CUR_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/logger -I$(INCLUDE_DIR)/wpa-supplicant-dbus -Wall 

SRC_DIR=$(CUR_DIR)
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS= -lwpa_supplicant_dbus -llogger
DEP=

BIN_NAME=libwpa_supplicant_client.a

SRC= wpa_supplicant_client.c     \
     wpa_supplicant_client_iface_mgr.c    \
     wpa_supplicant_client_iface.c \
     wpa_supplicant_client_bss_mgr.c \
     wpa_supplicant_client_bss.c \
     wpa_supplicant_client_network_mgr.c \
     wpa_supplicant_client_network.c  \
     wpa_supplicant_mgr_api.c
 	 
PUB_INC= wpa_supplicant_client.h \
         wpa_supplicant_client_iface.h \
         wpa_supplicant_client_bss.h \
 	 	 wpa_supplicant_client_network.h \
		 wpa_supplicant_client_common.h	      



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

