FLAGS := -I./comm-library -L./comm-library

all: game	
game: game.c gamespace.c list.c 
	gcc -w -o game game.c gamespace.c list.c ${FLAGS} -lmessage-library -lncurses 

clean: 
	rm -rf game 
