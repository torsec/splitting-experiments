all: game
allDebug: gameDebug
allRelease: gameRelease

game: game.c gamespace.c list.c
	gcc -w -o bin/game game.c gamespace.c list.c -lncurses
gameDebug: game.c gamespace.c list.c
	mkdir -p bin/Debug
	gcc -w -o bin/Debug/game game.c gamespace.c list.c -lncurses
gameRelease: game.c gamespace.c list.c
	mkdir -p bin/Release
	gcc -w -o bin/Release/game game.c gamespace.c list.c -lncurses

clean:
	rm -rf bin/game
cleanDebug:
	rm -rf bin/Debug/game
cleanRelease:
	rm -rf bin/Release/game
