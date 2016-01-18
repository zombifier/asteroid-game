all: battle.C
	g++ battle.C -o battle -lncurses -lpthread
