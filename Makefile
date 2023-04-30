CC=g++
CFLAGS=-static -O3 -funroll-loops -c
LDFLAGS=-static -O2 -lm

SRC_DIR=src/
SRC_SUB_DIRS=core util tasks combat
OBJ_DIR=obj/
EXE_DIR=build/

SOURCES=$(wildcard $(SRC_DIR)**/*.cpp) $(wildcard $(SRC_DIR)*.cpp)
OBJECTS=$(patsubst ${SRC_DIR}%.cpp,${OBJ_DIR}%.o,$(SOURCES))
EXECUTABLE=$(EXE_DIR)MyBot

INCLUDE_PATHS=$(addprefix -I${SRC_DIR}, $(SRC_SUB_DIRS))
CFLAGS:=$(INCLUDE_PATHS) $(CFLAGS)

#Uncomment the following to enable debugging
#CFLAGS+=-g -DDEBUG

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

clean:
	@echo ${INCLUDE_PATHS}
	-rm -rf ${OBJ_DIR}* ${EXE_DIR}*
	-rm -f debug.txt

.PHONY: all clean
