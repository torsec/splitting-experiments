all: game
gamespace: gamespace.c 
	gcc -w -o gamespace gamespace.c list.c comm-library/message-library.c -lncurses 
game: game.c gamespace.c list.c comm-library/message-library.c
	gcc -w -o game game.c gamespace.c list.c comm-library/message-library.c -lncurses 
game1: game1.c gamespace.c list.c comm-library/message-library.c
	gcc -w -o game1 game1.c gamespace.c list.c comm-library/message-library.c -lncurses 
game2: game2.c gamespace.c list.c comm-library/message-library.c
	gcc -w -o game2 game2.c gamespace.c list.c comm-library/message-library.c -lncurses 
game3: game3.c gamespace.c list.c comm-library/message-library.c
	gcc -w -o game3 game3.c gamespace.c list.c comm-library/message-library.c -lncurses 
game4: game4.c gamespace.c list.c comm-library/message-library.c
	gcc -w -o game4 game4.c gamespace.c list.c comm-library/message-library.c -lncurses 
game5: game5.c gamespace.c list.c comm-library/message-library.c
	gcc -w -o game5 game5.c gamespace.c list.c comm-library/message-library.c -lncurses 
game6: game6.c gamespace.c list.c comm-library/message-library.c
	gcc -w -o game6 game6.c gamespace.c list.c comm-library/message-library.c -lncurses 

clean: 
	rm -rf game game1 game2 game3 game4 game5 game6 game.dSYM game1.dSYM game2.dSYM game3.dSYM game4.dSYM game5.dSYM game6.dSYM

