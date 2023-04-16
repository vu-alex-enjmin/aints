CC=g++
CFLAGS=-static -O3 -funroll-loops -c
LDFLAGS=-static -O2 -lm

SRC_DIR=src/
OBJ_DIR=obj/
EXE_DIR=build/

SOURCES=$(wildcard $(SRC_DIR)*.cpp)
OBJECTS=$(patsubst ${SRC_DIR}%.cpp,${OBJ_DIR}%.o,$(SOURCES))
EXECUTABLE=$(EXE_DIR)MyBot

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
	-rm -f ${EXECUTABLE} ${OBJECTS} *.d
	-rm -f debug.txt

.PHONY: all clean
