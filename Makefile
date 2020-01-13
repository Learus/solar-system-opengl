BIN=./bin
SRC=./src
INCLUDE=./include/

MAIN=main
OBJECTS=$(BIN)/$(MAIN).o $(BIN)/glad.o
LIBS=-lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lassimp -lXi -ldl -lXinerama -lXcursor

all: compile
	@make run
	

compile: dirs $(OBJECTS)
	@g++ $(OBJECTS) -I$(INCLUDE) -o $(BIN)/$(MAIN) -std=c++11 -Wall $(LIBS)


$(BIN)/$(MAIN).o: $(SRC)/$(MAIN).cpp
	@gcc -c $(SRC)/$(MAIN).cpp -o $(BIN)/$(MAIN).o

$(BIN)/glad.o: $(SRC)/glad.c
	@gcc -c $(SRC)/glad.c -o $(BIN)/glad.o

dirs:
	@mkdir -p $(BIN)

run:
	@$(BIN)/$(MAIN)

clean:
	@rm -rf $(BIN)