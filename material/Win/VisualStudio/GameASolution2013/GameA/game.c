//                                         ._____________________.
//                                         |  .    .   .   . .__ |
//                                         | / _  /_\  |\ /| |_  |
//                                         | \_/ /   \ | v | |__ |
//                                         |                     |
//                                         |     Geometrical     |
//                                         |     Ascii           |
//                                         |     Multi-game      |
//                                         |     Environment     |
//                                         ._____________________.
//                                          (C) 2005 Toño Araújo
//                                          avaraujo@mundo-r.com
//
#include <curses.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "gamespace.h"

//#define NDEBUG

/////////////// PROGRAMA PRINCIPAL ///////////////////////////////////////////////

static void setupPlayers(PlayerSpace *space, Player *players);

List *crashedPlayers;
PlayerSpace * playerSpace;
Player players[NUM_PLAYERS];

BodyMap snakeBodyMap[] ={
	{
	"#DabAlEArroZaLAZorrAElAbaD",
	NULL
	},{
		NULL
	}
};

BodyMap trainBodyMap[] ={
	{
	"V.####.####.####.####.####.####.####.####.####.####.####.####.",
	NULL
	},{
		NULL
	}
};
BodyMap giantBodyMap[] ={
	{
	"XXXXXXXXXXXXXX",
	"XX          XX",
	"XX   XXXX   XX",
	"XX   X  X   XX",
	"XX   XXXX   XX",
	"XX          XX",
	NULL
	},{
		NULL
	}
};
BodyMap squareBodyMap[] ={
	{
	"[][][][]",
	"[]    []",
	"[]    []",
	"[][][][]",
	NULL
	},{
		NULL
	}
};

BodyMap  circleBodyMap[]={
	{
	".].--.,!",
	"/ DONT ]",
	"- WALK -",
	"-].__.[!",
	NULL
	},{
	"?/.-%.[?",
	"[ MAIN ?",
	". EXIT .",
	"-/.-_--#",
	NULL
	},{
	"/...-./=",
	"? FIRE }",
	"? CORE }",
	"{@._@./-",
	NULL
	},{
	";-.//|@=",
	"| DUMP /",
	"/ GROG .",
	"}..?;,--",
	NULL
	},{
		NULL
	}
};

extern int input_n;
extern int input_i;
extern char* input_key;

int main(int argc, char* argv[]){
    if (argc == 2) {
    	input_n = strlen(argv[1]);
    	input_key = malloc(256);
    	input_key = strncpy(input_key, argv[1], 255);
    }
	srand(time(0));
	initScreen();
	atexit(salir);
	playerSpace = setupPlayerSpace();
	crashedPlayers = createList();
	setupPlayers(playerSpace, players);
	while(1){
		gameTic();
	}
	resetScreen();
	freePlayerSpace(playerSpace);
    exit(0);
}
static void setupPlayers(PlayerSpace *space, Player *players){
	Player *player;
	player = players;
	
	setupPlayer(
		space, 
		player, 
		SOLID_BODY,	
		3, 
		40, 
		10, 
		keyboardPilot,
		onCrashEnlargeMonster,
		onPushed,
		circleBodyMap
	);
	int n;
	for(n=0;n<5;n++){
		player++;
		setupPlayer(
			space, 
			player, 
			FLEXIBLE_BODY,	
			n, 
			2*n, 
			10+n, 
			randomPilot,
			NULL,
			onPushed,
			snakeBodyMap	
		);
	}

	for(n=0;n<5;n++){
		player++;
		setupPlayer(
			space, 
			player, 
			FLEXIBLE_BODY,	
			n*2, 
			4*n, 
			n, 
			randomPilot,
			onCrashBiteMonster,
			onPushed,
			trainBodyMap	
		);
	}

	player++;
	setupPlayer(
		space, 
		player, 
		SOLID_BODY,	
		2, 
		4, 
		n, 
		circlePilot,
		onCrashBiteMonster,
		onPushed,
		giantBodyMap	
	);

	for(n=0;n<8;n++){
		player++;
		setupPlayer(
			space, 
			player, 
			SOLID_BODY,	
			n, 
			10+n*8, 
			n, 
			randomPilot,
			onCrashEnlargeMonster,
			onPushed,
			squareBodyMap	
		);
	}
}
