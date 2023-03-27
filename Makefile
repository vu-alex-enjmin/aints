CC=g++
CFLAGS=-static -O3 -funroll-loops -c
LDFLAGS=-static -O2 -lm

SRC_DIR=src\\
OBJ_DIR=obj\\
EXE_DIR=build\\

SOURCES=Bot.cc MyBot.cc State.cc
OBJECTS=$(addprefix ${OBJ_DIR}, $(addsuffix .o, $(basename ${SOURCES})))
EXECUTABLE=$(EXE_DIR)MyBot

#Uncomment the following to enable debugging
#CFLAGS+=-g -DDEBUG

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)MyBot.o : $(SRC_DIR)MyBot.cc
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)State.o : $(SRC_DIR)State.cc $(SRC_DIR)State.h
	$(CC) $(CFLAGS) $< -o $@

$(SRC_DIR)State.h : $(addprefix ${SRC_DIR}, Timer.h Bug.h Square.h Location.h)
	touch $@

$(SRC_DIR)Bot.h : $(addprefix ${SRC_DIR}, State.h)
	touch $@

$(OBJ_DIR)%.o : $(SRC_DIR)%.cc $(SRC_DIR)%.h
	$(CC) $(CFLAGS) $< -o $@

clean: 
	-rm -f ${EXECUTABLE} ${OBJECTS} *.d
	-rm -f debug.txt

.PHONY: all clean

