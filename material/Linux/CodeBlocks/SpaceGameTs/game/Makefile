all: game
allDebug: gameDebug
allRelease: gameRelease

game: game.c gamespace.c list.c
	gcc -w -o bin/game game.c gamespace.c list.c comm-library/message-library.o -lncurses
gameDebug: game.c gamespace.c list.c
	mkdir -p bin/Debug
	gcc -w -o bin/Debug/game game.c gamespace.c list.c comm-library/message-library.o -lncurses
gameRelease: game.c gamespace.c list.c
	mkdir -p bin/Release
	gcc -w -o bin/Release/game game.c gamespace.c list.c comm-library/message-library.o -lncurses

clean:
	rm -rf bin/game
cleanDebug:
	rm -rf bin/Debug/game
cleanRelease:
	rm -rf bin/Release/game
