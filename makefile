#======================================================================================================================#
# CHECK
#======================================================================================================================#

ifneq ($(strip $(TARGET)),linux)
ifneq ($(strip $(TARGET)),windows)
$(error set TARGET=linux or TARGET=windows)
endif
endif

ifeq ($(strip $(TARGET)),windows)
ifeq ($(strip $(MINGW)),)
$(error set MINGW=[[path to mingw root folder]], for example, MINGW="C:\MinGW")
endif
endif

#======================================================================================================================#
# CONFIG
#======================================================================================================================#

#---- BASIC -----------------------------------------------------------------------------------------------------------#

LIBRARY        := microcanvas
ifeq ($(strip $(TARGET)),linux)
LIBS           := -lmicrocanvas -lmicrocompute -lGL -lglfw -lm
else ifeq ($(strip $(TARGET)),windows)
LIBS           := -lmicrocanvas -lmicrocompute -lopengl32 -lglfw3 -lgdi32 -lm
endif
FLAGS          := -std=c11 -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wno-unused-parameter -O3
DEFS           := $(CFLAGS)

#---- PROJECT STRUCTURE -----------------------------------------------------------------------------------------------#

INCLUDE_FOLDER := include
LIB_FOLDER     := lib
BUILD_FOLDER   := build
SRC_FOLDER     := src
OUT_FOLDER     := tmp
EXAMPLE_FOLDER := example

#======================================================================================================================#

ifeq ($(strip $(TARGET)),linux)
CC                := gcc $(FLAGS) $(DEFS) -isystem $(INCLUDE_FOLDER) -I $(SRC_FOLDER)
else ifeq ($(strip $(TARGET)),windows)
CC                := gcc $(FLAGS) $(DEFS) -isystem $(INCLUDE_FOLDER) -isystem $(MINGW)/include -I $(SRC_FOLDER)
endif
AR                := ar rcs
RM                := rm -rf
CD                := cd
CP                := cp
MKDIR             := mkdir -p
SRC_FOLDERS       := $(shell find $(SRC_FOLDER)/ -type d)
BUILD_SUB_FOLDERS := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(SRC_FOLDERS))
C_FILES           := $(shell find $(SRC_FOLDER)/ -type f -name "*.c")
C_OBJECTS         := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(C_FILES)))
EXAMPLE_C_FILES   := $(shell find $(EXAMPLE_FOLDER)/ -type f -name "*.c")
EXAMPLES          := $(subst $(EXAMPLE_FOLDER)/,$(OUT_FOLDER)/,$(subst .c,,$(EXAMPLE_C_FILES)))
STATIC_LIB        := $(LIB_FOLDER)/lib$(LIBRARY).a

.PHONY: default all library examples dependency doc clean

default: library

all: library examples doc

library: clean $(STATIC_LIB)

examples: $(EXAMPLES)

dependency: $(INCLUDE_FOLDER) $(LIB_FOLDER)
	$(CD) microcompute; \
	$(MAKE_COMMAND) clean; \
	$(MAKE_COMMAND) library
	$(CP) microcompute/include/microcompute.h $(INCLUDE_FOLDER)/microcompute.h
	$(CP) microcompute/lib/libmicrocompute.a $(LIB_FOLDER)/libmicrocompute.a

doc:
	python3 microcompute/microdoc/doc_generator.py $(SRC_FOLDER)/$(LIBRARY).h doc.md

$(OUT_FOLDER):
	$(MKDIR) $(OUT_FOLDER)

$(INCLUDE_FOLDER):
	$(MKDIR) $(INCLUDE_FOLDER)

$(LIB_FOLDER):
	$(MKDIR) $(LIB_FOLDER)

$(BUILD_SUB_FOLDERS):
	$(MKDIR) $(BUILD_SUB_FOLDERS)

$(C_OBJECTS): $(BUILD_SUB_FOLDERS) $(C_FILES)
	$(CC) -c $(subst $(BUILD_FOLDER)/,$(SRC_FOLDER)/,$(subst .o,.c,$@)) -o $@

$(STATIC_LIB): dependency $(INCLUDE_FOLDER) $(LIB_FOLDER) $(C_OBJECTS)
	$(AR) $(STATIC_LIB) $(C_OBJECTS)
	$(CP) $(SRC_FOLDER)/$(LIBRARY).h $(INCLUDE_FOLDER)/$(LIBRARY).h

$(EXAMPLES): $(OUT_FOLDER) $(STATIC_LIB)
	$(CC) -g $(subst $(OUT_FOLDER)/,$(EXAMPLE_FOLDER)/,$@).c -o $@ -L$(LIB_FOLDER) $(LIBS)

clean:
	$(RM) $(BUILD_FOLDER) $(INCLUDE_FOLDER) $(LIB_FOLDER) $(OUT_FOLDER)
