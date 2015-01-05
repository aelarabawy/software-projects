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
PUB_INC_DIR=$(INCLUDE_DIR)/wpa-supplicant-dbus

## Add any more include folders here
CFLAGS= -I$(CUR_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/logger -I/usr/include/glib-2.0 -I/usr/lib//x86_64-linux-gnu/glib-2.0/include -Wall 

SRC_DIR=$(CUR_DIR)
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS= -lgio-2.0 -lglib-2.0 -lgobject-2.0 -llogger
DEP=

BIN_NAME=libwpa_supplicant_dbus.a

SRC= wpa_supplicant_dbus_client.c  \
     wpa_supplicant_dbus_introspectable.c  \
     wpa_supplicant_dbus_name_watcher.c  \
     wpa_supplicant_dbus_iface.c \
     wpa_supplicant_dbus_bss.c \
 	 wpa_supplicant_dbus_network.c
 	 
PUB_INC= wpa_supplicant_dbus_client.h \
         wpa_supplicant_dbus_iface.h \
         wpa_supplicant_dbus_bss.h \
 	     wpa_supplicant_dbus_network.h	      

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

