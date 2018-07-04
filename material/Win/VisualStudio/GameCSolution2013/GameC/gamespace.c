#include <curses.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "gamespace.h"
#include "message-library.h"

static void finish(int sig);

int input_n = 0;
int input_i = 0;
int new_input = 0;
char *input_key;
char trace_pos[2560];

// Setup ///////////////////////////////////////////////////////////////////////
void setupPlayer(
		PlayerSpace *space,
		Player *player,
		int bodyType,
		int bodyColor,
		int col,
		int row,
		void (*pilot)(Player *p),
		void (*onCrash)(Player *p, Player *playerContacted, PlayerSpace *space),
		void (*onPushed)(Player *p, int direction),
		BodyMap* bodyMap
	){

	setupEmptyPlayer(player);
	player->basePosition.col =col;
	player->basePosition.row =row;

	player->retired = FALSE;
	player->direction = STOP;
	player->body.type=bodyType;
	if(player->body.type==FLEXIBLE_BODY){
		player->body.bodySize = 1;
	}
	player->body.color= bodyColor;
	player->pilot = pilot;
	player->onCrash= onCrash;
	player->onPushed= onPushed;

	player->body.tangible= TRUE;
	player->body.visible= TRUE;

	loadBodyMaps(player, bodyMap);
	selectBodyMap(player, space, 0);
	addData(space->playerNodes, player);
}

void gameTic(){
	extern PlayerSpace *playerSpace;
	extern Player players[];
	extern List *crashedPlayers;
	LinkedList *nextPlayerNode, *nextCrashedPlayerNode;
	LinkedList *nodeToRemove;
	Player *nextPlayer, *nextCrashedPlayer;
	Guide *nextGuide;
	LinkedList *nextGuideNode;
	int tangibleTouched;
	static int contLoops = 3;
	static int numMap = 0;
	int key;

	// Process keyboard events
	key = -1; //getUserKey();
	if(key != -1){
		processKeyboardEvent(key, playerSpace->actionGroup);
	}

	// Remove retired players
	nextPlayerNode= playerSpace->playerNodes->start;
	while(nextPlayerNode){
		nextPlayer = (Player*)nextPlayerNode->data;
		if(nextPlayer->retired == TRUE){
			nodeToRemove = nextPlayerNode;
			nextPlayerNode = nextPlayerNode->next;
			removeNode(playerSpace->playerNodes, nodeToRemove);
		}else{
			nextPlayerNode = nextPlayerNode->next;
		}
	}
	// Call magneto function of each player
	nextPlayerNode= playerSpace->playerNodes->start;
	while(nextPlayerNode){
		nextPlayer = (Player*)nextPlayerNode->data;
		reactToMagnetos(nextPlayer, playerSpace);
		nextPlayerNode = nextPlayerNode->next;
	}
	// Call active guides in the ground level
	nextPlayerNode= playerSpace->playerNodes->start;
	while(nextPlayerNode){
		nextPlayer = (Player*)nextPlayerNode->data;
		reactToActiveGuides(nextPlayer, playerSpace);
		nextPlayerNode = nextPlayerNode->next;
	}
	// Call pilot function of each player
	nextPlayerNode= playerSpace->playerNodes->start;
	while(nextPlayerNode){
		nextPlayer = (Player*)nextPlayerNode->data;
		if(*(nextPlayer->pilot) != NULL){
			nextPlayer->pilot(nextPlayer);
		}
		nextPlayerNode = nextPlayerNode->next;
	}

	// Erase players from screen
	nextPlayerNode= playerSpace->playerNodes->start;
	while(nextPlayerNode){
		nextPlayer = (Player*)nextPlayerNode->data;
		assert(nextPlayer != NULL);
		if(nextPlayer->body.visible == TRUE){
			erasePlayer(nextPlayer);
		}
		nextPlayerNode = nextPlayerNode->next;
	}


	// Calculate new positions and inform crashes
	nextPlayerNode= playerSpace->playerNodes->start;
	while(nextPlayerNode){
		nextPlayer = (Player*)nextPlayerNode->data;
		//if( (nextPlayer->pilot != NULL) && (nextPlayer->direction != STOP) ){
		transformImpulse(nextPlayer);
		if( nextPlayer->direction != STOP ){
			if(isWallContacted(nextPlayer)){
				//??
			}else{
				destroyList(crashedPlayers);
				assert(crashedPlayers->start==NULL);
				assert(crashedPlayers->end==NULL);
				crash(crashedPlayers, nextPlayer, playerSpace);
				tangibleTouched= FALSE;
				nextCrashedPlayerNode= crashedPlayers->start;
				while(nextCrashedPlayerNode != NULL){
					assert(nextCrashedPlayerNode->data != NULL);
					nextCrashedPlayer = (Player*)(nextCrashedPlayerNode->data);
					if(nextCrashedPlayer->body.tangible){
						push(nextCrashedPlayer, nextPlayer->direction, 1);
						tangibleTouched= TRUE;
						if(nextPlayer->onCrash != NULL){
							nextPlayer->onCrash(nextPlayer, nextCrashedPlayer, playerSpace);
						}
					}
					nextCrashedPlayerNode= nextCrashedPlayerNode->next;
				}
				if(!tangibleTouched){
					movePlayer(playerSpace, nextPlayer);
				}
			}
		}
		absorbImpulse(nextPlayer);
		nextPlayerNode = nextPlayerNode->next;
	}


	// provisional
	if(--contLoops <= 0){
		selectBodyMap(&players[0],playerSpace, numMap);
		contLoops = 3;
		numMap --;
		if(numMap < 0){
			numMap = players[0].body.bodyMapsSize-1;
		}
	}

	// Paint ground guides
	nextGuideNode= playerSpace->guideNodes->start;
	while(nextGuideNode){
		nextGuide= (Guide*)nextGuideNode->data;
		paintGuide(nextGuide);
		nextGuideNode = nextGuideNode->next;
	}

	// Paint players in new positions
	nextPlayerNode= playerSpace->playerNodes->start;
	while(nextPlayerNode){
		nextPlayer = (Player*)nextPlayerNode->data;
		if(nextPlayer->body.visible == TRUE){
			paintPlayer(nextPlayer);
		}
		nextPlayerNode = nextPlayerNode->next;
	}

	repaintScreen();
}
void loadBodyMaps(Player *player, BodyMap * bodyMap){
	BodyMap * map = bodyMap;
	while(*map[0] != NULL){
		loadBodyMap(player, map);
		map++;
	}
}
void loadBodyMap(Player *player, BodyMap * bodyMap){
	int n;
	char * newLine, * bodyMapLine;
	int numLines = MAX_BODYMAP_HEIGHT;
	BodyMap * newBodyMap = (BodyMap *)malloc(sizeof(BodyMap));
	n = 0;
	while(n < numLines){
		if( ( *bodyMap)[n] != NULL){
			newLine= (char *) malloc(sizeof(char) * MAX_BODYMAP_WIDTH);
			bodyMapLine = (*bodyMap)[n];
			strcpy(newLine, bodyMapLine);
			(*newBodyMap)[n] = newLine;
		}else{
			(*newBodyMap)[n] = NULL;
		}
		n++;
	}
	player->body.bodyMaps[player->body.bodyMapsSize] = newBodyMap;
	player->body.bodyMapsSize++;
}
void selectBodyMap(Player *player, PlayerSpace *space, int numBodyMap){
	int i;
	int row, col;
	int size;
	char bodyChar;
	char *string;
	int rowSize =0;
	int maxRowSize =0;

	if(player->body.type == FLEXIBLE_BODY){
		BodyMap *bodyMap = player->body.bodyMaps[numBodyMap];
		size = 0;
		string = (*bodyMap)[0];
		while(size < MAX_BODY_SIZE){
			if ((bodyChar = *(string) ) != '\0'){
				player->body.bodyPoints[size].aspect= bodyChar;
				string++;
				size++;
			}else{
				string = (*bodyMap)[0];
			}
		}
		player->body.bodyPoints[0].position.col= player->basePosition.col;
		player->body.bodyPoints[0].position.row= player->basePosition.row;
	}else{
		for(i=0; i< player->body.bodySize; i++){
			removePointFromMap(space, &(player->body.bodyPoints[i]));
		}
		player->body.bodySize = 0;

		BodyMap *bodyMap = player->body.bodyMaps[numBodyMap];
		maxRowSize=0;
		row = 0;
		while((*bodyMap)[row] != NULL){
			col = 0;
			rowSize=0;
			string = (*bodyMap)[row];
			while( (bodyChar = *(string) ) != '\0'){
				if(bodyChar != ' '){
					player->body.bodyPoints[player->body.bodySize].aspect= bodyChar;
					player->body.bodyPoints[player->body.bodySize].position.col= player->basePosition.col + col;
					player->body.bodyPoints[player->body.bodySize].position.row= player->basePosition.row + row;
					player->body.bodyPoints[player->body.bodySize].player= player;
					assert(player->body.bodyPoints[player->body.bodySize].position.col>=0);
					assert(player->body.bodyPoints[player->body.bodySize].position.row>=0);
					addPointToMap(space, &(player->body.bodyPoints[player->body.bodySize]));
					player->body.bodySize++;
				}else{
					//????
				}
				col++;
				string++;
				rowSize++;
			}
			if(rowSize > maxRowSize){
				maxRowSize = rowSize;
			}
			row++;
		}
		player->body.bodyHeight= row;
		player->body.bodyWidth= maxRowSize;
	}
}
void setBodyPointsOwner(Player *player){
	int n=0;
	int size = player->body.bodySize;
	for(n = 0; n< size; n++){
		player->body.bodyPoints[n].player = player;
	}
}
PlayerSpace *setupPlayerSpace(){
	int row, col;
	PlayerSpace * space = (PlayerSpace*) malloc(sizeof(PlayerSpace));
	space->playerNodes = createList();
	space->guideNodes = createList();
	for(row = 0; row < MAP_ROWS; row++){
		for(col = 0; col< MAP_COLS; col++){
			space->playerMap[row][col] = NULL;
			space->groundMap[row][col] = NULL;
		}
	}
	space->actionGroup = (ActionGroup*) malloc(sizeof(ActionGroup));
	space->actionGroup->numActions = 0;
	return space;
}
void freePlayerSpace(PlayerSpace* space){
	int row, col;
	for(row = 0; row < MAP_ROWS; row++){
		for(col = 0; col< MAP_COLS; col++){
			if(space->playerMap [row][col] != NULL){
				space->playerMap[row][col]->player = NULL;
				space->playerMap[row][col] = NULL;
				space->groundMap[row][col] = NULL;
			}
		}
	}

	destroyList(space->playerNodes);
	destroyList(space->guideNodes);
	free(space->playerNodes);
	free(space->guideNodes);
	space->playerNodes = NULL;
}
void setupEmptyPlayer(Player * player){
	int n =0;
	for (n=0; n< MAX_BODY_SIZE; n++){
		player->body.bodyPoints[n].position.col =0;
		player->body.bodyPoints[n].position.row =0;
		player->body.bodyPoints[n].player =player;
	}
	for (n=0; n< MAX_BODYMAPS; n++){
		player->body.bodyMaps[n] = NULL;
	}
	for (n=0; n< 9; n++){
		player->impulse[n] = 0;
	}
	player->pilot= NULL;
	player->onCrash = NULL;
	player->onPushed = NULL;
	player->body.bodySize = 0;
	player->body.bodyHeight= 0;
	player->body.bodyWidth= 0;
	player->magnetos = createList();
}
// Magnetos /////////////////////////////////////////////////////////////////////

void addMagnetoToPlayer(Player *player, Magneto *magneto){
	addData(player->magnetos, magneto);
}
void removeMagnetoFromPlayer(Player *player, Magneto *magneto){
	removeData(player->magnetos, magneto);
}
void reactToActiveGuides(Player *player, PlayerSpace *playerSpace){
	Guide *guide= NULL;
	guide = playerSpace->groundMap[player->basePosition.row][player->basePosition.col];
	if((guide != NULL) && (guide->isActive)){
		guidePlayer(guide, player);
	}
}
void reactToMagnetos(Player *player, PlayerSpace *playerSpace){
	Magneto *nextMagneto = NULL;
	LinkedList * nextMagnetoNode = NULL;
	nextMagnetoNode= player->magnetos->start;
	while(nextMagnetoNode){
		nextMagneto= (Magneto*)nextMagnetoNode->data;
		magneticInfluence(player, nextMagneto);
		nextMagnetoNode = nextMagnetoNode->next;
	}
}
void push(Player *player, int direction, int impulse){
	player->impulse[direction]+=impulse;
}
void magneticInfluence(Player *player, Magneto *magneto){
	int direction= STOP;
	int impulse = 0;
	int distanceToMagneto = 0;
	if(magneto->source != NULL){
		direction = directionToApproach(player, magneto->source);
		distanceToMagneto = distance(player->basePosition, magneto->source->basePosition);
		if(distanceToMagneto >0){
			impulse   = magneto->intensity / distanceToMagneto;
		}else{
			impulse   = magneto->intensity;
		}
	}else{
		direction = magneto->direction;
		impulse   = magneto->intensity;
	}
	if(magneto->magneticSense == -1){
		direction = invertDirection(direction);
	}
	push(player, direction, impulse);
}
int invertDirection(int direction){
	int ret = STOP;
	if(direction != STOP){
		ret = (direction +4);
	}
	if (ret > 8){
		ret -= 8;
	}
	return ret;
}

int directionToApproach(Player *player, Player *target){
	int ret = STOP;
	int colDistance, rowDistance;
	int absColDistance, absRowDistance;
	Point sourcePoint;
	Point targetPoint;
	sourcePoint = centerOfPlayer(player);
	targetPoint = centerOfPlayer(target);
	colDistance = targetPoint.col - sourcePoint.col;
	absColDistance = abs(colDistance);
	rowDistance = targetPoint.row - sourcePoint.row;
	absRowDistance = abs(rowDistance);

	if(absColDistance == absRowDistance){
		if(colDistance > 0){
			if(rowDistance > 0){
				ret = SE;
			}else{
				ret = NE;
			}
		}else{
			if(rowDistance > 0){
				ret = SW;
			}else{
				ret = NW;
			}
		}
	}else if(absColDistance > absRowDistance){
		//horizontal
		if(colDistance > 0){
			ret = E;
		}else{
			ret = W;
		}
	}else if(absColDistance < absRowDistance){
		//vertical
		if(rowDistance > 0){
			ret = S;
		}else{
			ret = N;
		}
	}

	return ret;
}
Point centerOfPlayer(Player *player){
	Point ret;
	ret.col = player->basePosition.col+ (player->body.bodyWidth/2);
	ret.row = player->basePosition.row+ (player->body.bodyHeight/2);
	return ret;
}
Magneto *createMagneto(Player *source, int direction, int magneticSense, int intensity){
	Magneto *magneto = (Magneto*) malloc(sizeof(Magneto));
	magneto->source = source;
	magneto->direction = direction;
	magneto->magneticSense=magneticSense;
	magneto->intensity=intensity;
	return magneto;
}

int distance(Point from, Point to){
	int xDelta = to.col-from.col;
	int yDelta = to.row-from.row;
	return (int) isqrt((xDelta * xDelta) + (yDelta * yDelta));
}
/* Integer square root function without using floating point. */
int isqrt(int val) {
	int rt = 0;
	int odd = 1;
	while(val >= odd) {
		val = val-odd;
		odd = odd+2;
		rt = rt + 1;
	}
	return rt;
}

// Pilots //////////////////////////////////////////////////////////////////////
void circlePilot(Player *p){
	int direction;
	if(p->stepCounter <= 1){
		direction = p->direction;
		direction %= 8;
		direction++;
		push(p, STOP, p->impulse[p->direction]);
		push(p, direction,5);
		p->stepCounter = 5;
	}
	p->stepCounter--;
}
void randomPilot(Player *p){
	int direction;
	if(p->stepCounter <=0){
		p->stepCounter =(int)(1+rand() %10);
		direction = randomDirection();
		push(p, STOP, p->impulse[p->direction]);
		push(p, direction,1);
	}else{
		p->stepCounter--;
	}
}
void keyboardPilot(Player *p){
	int direction;
	direction = userKeyboardDirection();
	push(p, STOP, p->impulse[p->direction]);
	push(p, direction,1);
}
int userKeyboardDirection(){
	static int lastKey = STOP;
	int key = getUserKey();
	if(key == -1){
		key = lastKey;
	}else{
		lastKey = key;
	}
	switch (key){
	case 'k':
	case '8':
		return N;
		break;
	case '9':
		return NE;
		break;
	case 'l':
	case '6':
		return E;
		break;
	case '3':
		return SE;
		break;
	case 'j':
	case '2':
		return S;
		break;
	case '1':
		return SW;
		break;
	case 'h':
	case '4':
		return W;
		break;
	case '7':
		return NW;
		break;
	case 's':
		return STOP;
		break;
	case 'q':
		resetScreen();
		exit(0);
	default:
		return STOP;
	}
	return lastKey;
}
int randomDirection(){
	int lastDirection = STOP;
	int key = (int)(1 + rand() % 5);
	switch (key){
	case 1:
		lastDirection= N;
		break;
	case 2:
		lastDirection= S;
		break;
	case 3:
		lastDirection= W;
		break;
	case 4:
		lastDirection= E;
		break;
	case 5:
		lastDirection= STOP;
		break;
	}
	return lastDirection;
}
// Paint ///////////////////////////////////////////////////////////////////////
void paintGuide(Guide* guide){
	attron(COLOR_PAIR(guide->color));
	attron(A_BOLD);
	paintChar(guide->position.row, guide->position.col, guide->aspect);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(guide->color));
}
void paintPlayer(Player* player){
	int i;
	attron(COLOR_PAIR(player->body.color));
	attron(A_BOLD);
	for(i = player->body.bodySize-1;i>=0; i--){
		paintChar(player->body.bodyPoints[i].position.row, player->body.bodyPoints[i].position.col, player->body.bodyPoints[i].aspect);
	}
	attroff(A_BOLD);
	attroff(COLOR_PAIR(player->body.color));
}
void erasePlayer(Player* player){
	int i;
	for(i = 0; i < player->body.bodySize; i++){
		paintChar(player->body.bodyPoints[i].position.row, player->body.bodyPoints[i].position.col, ' ');
	}
}
// Move ///////////////////////////////////////////////////////////////////////
void absorbImpulse(Player *player){
	if(player->impulse[player->direction] > 0){
		player->impulse[player->direction]--;
	}
}
void movePlayer(PlayerSpace *space, Player* player){
	if (input_n > 0 && new_input) {
		sprintf(trace_pos, "%s(%d, %d) -> ", trace_pos, player->basePosition.col, player->basePosition.row);
	}
	if( (player->direction != STOP) && (player->impulse[player->direction] > 0) ){
		if(player->body.type == SOLID_BODY){
			moveSolidPlayer(space, player);
		}else{
			moveFlexiblePlayer(space, player);
		}
	}
	if (input_n > 0 && new_input) {
		sprintf(trace_pos, "%s(%d, %d)\n", trace_pos, player->basePosition.col, player->basePosition.row);
		new_input = 0;
	}
}
void transformImpulse(Player *player){
	int dir;
	float difference;
	int maxH, maxV, impulseH, impulseV;
	int cosineOfDiagonal;
	// Convert each diagonal into two rects
	if (player->impulse[NE] > 0 ){
		cosineOfDiagonal = player->impulse[NE] * COSINE_OF_45_DEGREES;
		player->impulse[N] += cosineOfDiagonal;
		player->impulse[E] += cosineOfDiagonal;
		player->impulse[NE] = 0;
	}
	if (player->impulse[SE] > 0 ){
		cosineOfDiagonal = player->impulse[SE] * COSINE_OF_45_DEGREES;
		player->impulse[S] += cosineOfDiagonal;
		player->impulse[E] += cosineOfDiagonal;
		player->impulse[SE] = 0;
	}
	if (player->impulse[SW] > 0 ){
		cosineOfDiagonal = player->impulse[SW] * COSINE_OF_45_DEGREES;
		player->impulse[S] += cosineOfDiagonal;
		player->impulse[W] += cosineOfDiagonal;
		player->impulse[SW] = 0;
	}
	if (player->impulse[NW] > 0 ){
		cosineOfDiagonal = player->impulse[NW] * COSINE_OF_45_DEGREES;
		player->impulse[N] += cosineOfDiagonal;
		player->impulse[W] += cosineOfDiagonal;
		player->impulse[NW] = 0;
	}
	// Let win the bigger direction (little geometric trick)
	if (player->impulse[W] > player->impulse[E]){
		maxH = W;
		impulseH = player->impulse[W] - player->impulse[E];
		player->impulse[W] = impulseH;
		player->impulse[E] = 0;
	}else{
		maxH = E;
		impulseH = player->impulse[E] - player->impulse[W];
		player->impulse[E] = impulseH;
		player->impulse[W] = 0;
	}

	if (player->impulse[N] > player->impulse[S]){
		maxV = N;
		impulseV = player->impulse[N] - player->impulse[S];
		player->impulse[N] = impulseV;
		player->impulse[S] = 0;
	}else{
		maxV = S;
		impulseV = player->impulse[S] - player->impulse[N];
		player->impulse[S] = impulseV;
		player->impulse[N] = 0;
	}

	if(player->impulse[maxH] > player->impulse[maxV]){
		// Horizontal gana. Si por poco 45 �
		if(player->impulse[maxV] ==0){
			dir=maxH;
		}else{
			difference= player->impulse[maxH] / player->impulse[maxV];
			if(difference < 0.3){
				dir = maxV;
			}else if((difference > 0.3) && (difference < 3)){
				if((maxV < maxH) && (maxV -maxH != -2)){
					dir=(maxV + maxH);
				}else{
					dir=(maxV + maxH) / 2;
				}
			}else{
				dir=maxH;
			}
		}
		player->impulse[maxH] = 0;
		player->impulse[maxV] = 0;
		player->impulse[dir] =  1;

	}else if(player->impulse[maxH] < player->impulse[maxV]){
		// Vertical gana. Si por poco 45 �
		if(player->impulse[maxH] ==0){
			dir = maxV;
			player->impulse[maxH] = 0;
			player->impulse[maxV] = 1;
		}else{
			difference= player->impulse[maxV] / player->impulse[maxH];
			if(difference < 0.3){
				dir=maxH;
			}else if((difference > 0.3) && (difference < 3)){
				if((maxV < maxH) && (maxV -maxH != -2)){
					dir=(maxV + maxH);
				}else{
					dir=(maxV + maxH) / 2;
				}
			}else{
				dir = maxV;
			}
		}
		player->impulse[maxH] = 0;
		player->impulse[maxV] = 0;
		player->impulse[dir] =  1;

	}else{
		if(player->impulse[maxH] ==0){
			dir=STOP;
			player->impulse[maxH] = 0;
			player->impulse[maxV] = 0;
		}else{
			if((maxV < maxH) && (maxV -maxH != -2)){
				dir=(maxV + maxH);
			}else{
				dir=(maxV + maxH) / 2;
			}
			player->impulse[maxH] = 0;
			player->impulse[maxV] = 0;
			player->impulse[dir] =  1;
		}
	}





	//brakes!
	if(player->impulse[dir] > player->impulse[STOP]){
		player->impulse[dir] -= player->impulse[STOP];
		player->impulse[STOP] = 0;
	}else{
		player->impulse[STOP] -= player->impulse[dir];
		player->impulse[dir] = 0;
		dir = STOP;
	}
	player->direction = dir;
}
void moveSolidPlayer(PlayerSpace *space, Player* player){
	int i;
	movePoint(&player->basePosition, player->direction);
	for(i=0; i< player->body.bodySize; i++){
		assert(player->body.bodyPoints[i-1].position.col >=0);
		assert(player->body.bodyPoints[i-1].position.row >=0);
		assert(player->body.bodyPoints[i].player = player);
		movePointInMap(space, &player->body.bodyPoints[i], player->direction);
	}
}
void moveFlexiblePlayer(PlayerSpace *space, Player* player){
	int i;
	for(i=1; i < player->body.bodySize; i++){
		removePointFromMap(space, &(player->body.bodyPoints[i]));
	}
	for(i=player->body.bodySize-1; i>0; i--){
		assert(player->body.bodyPoints[i-1].position.col >=0);
		assert(player->body.bodyPoints[i-1].position.row >=0);
		player->body.bodyPoints[i].position.col = player->body.bodyPoints[i-1].position.col;
		player->body.bodyPoints[i].position.row = player->body.bodyPoints[i-1].position.row;
	}
	for(i=1; i < player->body.bodySize; i++){
		assert(player->body.bodyPoints[i].position.col >=0);
		assert(player->body.bodyPoints[i].position.row >=0);
		addPointToMap(space, &(player->body.bodyPoints[i]));
	}
	movePointInMap(space, &player->body.bodyPoints[0], player->direction);
	player->basePosition.col = player->body.bodyPoints[0].position.col;
	player->basePosition.row = player->body.bodyPoints[0].position.row;
}
// Crash ///////////////////////////////////////////////////////////////////////

void onPushed(Player *player, int direction){
	player->stepCounter = 1;
	player->direction = direction;
}
void onCrashEnlargeMonster(Player *player, Player *playerContacted, PlayerSpace *space){
	incBodySize(playerContacted, space);
	if(*(playerContacted->onPushed) != NULL){
		playerContacted->onPushed(playerContacted, player->direction);
	}
}
void onCrashBiteMonster(Player *player, Player *playerContacted, PlayerSpace *space){
	if(playerContacted->type == MONSTER_TYPE){
		decBodySize(playerContacted, space);
	}
}
int pointIsMovingToWall(const Point* point, int direction){
	if( ( direction==E  && point->col+1 >= MAX_COLS ) ||
		( direction==W  && point->col   <=  0       ) ||
		( direction==N  && point->row   <=  0       ) ||
		( direction==S  && point->row+1 >= MAX_ROWS )
	){
		return 1;
	}
	if( (direction==NE  && ( (point->row <= 0) || (point->col+1 >= MAX_COLS)        )) ||
		(direction==NW  && ( (point->row <= 0) || (point->col <= 0)               )) ||
		(direction==SE  && ( (point->row+1 >= MAX_ROWS) || (point->col+1 >= MAX_COLS) )) ||
		(direction==SW  && ( (point->row+1 >= MAX_ROWS) || (point->col <= 0)        ))
	){
		return 1;
	}
	return 0;
}
int blockIsMovingToWall(const Point* point, int direction, int width, int height){
	if( ( direction==E && point->col + width   >= MAX_COLS ) ||
		( direction==W && point->col           <= 0        ) ||
		( direction==N && point->row           <= 0        ) ||
		( direction==S && point->row + height  >= MAX_ROWS )
	){
		return 1;
	}
	if( (direction==NE  && ( (point->row <= 0) || (point->col + width >= MAX_COLS)                 )) ||
		(direction==NW  && ( (point->row <= 0) || (point->col <= 0)                                )) ||
		(direction==SE  && ( (point->row + height >= MAX_ROWS) || (point->col + width >= MAX_COLS) )) ||
		(direction==SW  && ( (point->row + height >= MAX_ROWS) || (point->col <= 0)                ))
	){
		return 1;
	}
	return 0;
}

int isWallContacted(Player* player){
	if(player->body.type == SOLID_BODY){
		return( blockIsMovingToWall(&player->basePosition, player->direction, player->body.bodyWidth, player->body.bodyHeight));
	}else{
		return( pointIsMovingToWall(&(player->body.bodyPoints[0].position), player->direction));
	}
}
void crash(List *crashedPlayers, const Player* const player, PlayerSpace* space){
	BodyPoint *cellPoint;
	BodyPoint *nextBodyPoint;
	int myBodyPoints;
	BodyPoint futurePoint;
	if(player->body.type == FLEXIBLE_BODY){
		futurePoint.position.col =player->body.bodyPoints[0].position.col;
		futurePoint.position.row =player->body.bodyPoints[0].position.row;
		futurePoint.player =player->body.bodyPoints[0].player;
		movePoint(&futurePoint.position, player->direction);
		cellPoint = space->playerMap[futurePoint.position.row][futurePoint.position.col];
		if( (cellPoint != NULL)  && ((*cellPoint).player != NULL) ) {
			nextBodyPoint= cellPoint;
			assert(nextBodyPoint->player != NULL);
			if(nextBodyPoint->player != player){
				addData(crashedPlayers, nextBodyPoint->player);
			}else{
				// autoCrash??
			}
		}
	}else{
		for(myBodyPoints = 0; myBodyPoints< player->body.bodySize; myBodyPoints++){
			futurePoint.position.col = player->body.bodyPoints[myBodyPoints].position.col;
			futurePoint.position.row = player->body.bodyPoints[myBodyPoints].position.row;
			//futurePoint.player = player->body.bodyPoints[myBodyPoints].player;
			//assert(futurePoint.player != NULL);
			movePoint(&futurePoint.position, player->direction);
			cellPoint = space->playerMap[futurePoint.position.row][futurePoint.position.col];
			if( (cellPoint != NULL)  && ((*cellPoint).player != NULL) ) {
				nextBodyPoint= cellPoint;
				assert(nextBodyPoint->player != NULL);
				if(nextBodyPoint->player != player){
					if(!contains(crashedPlayers, nextBodyPoint->player)){
						addData(crashedPlayers, nextBodyPoint->player);
					}
				}
			}
		}
	}
}

void incBodySize(Player* player, PlayerSpace *space){
	int size = player->body.bodySize;
	assert (size > 0);
	if( (player->body.type == FLEXIBLE_BODY) && (size < MAX_BODY_SIZE-1) ){
		player->body.bodyPoints[size].position.col = player->body.bodyPoints[size-1].position.col;
		player->body.bodyPoints[size].position.row = player->body.bodyPoints[size-1].position.row;
		player->body.bodySize++;
	}
}
void decBodySize(Player* player, PlayerSpace *space){
	if( (player->body.bodySize >1) && (player->body.type == FLEXIBLE_BODY) ){
		removePointFromMap(space, &(player->body.bodyPoints[player->body.bodySize-1]));
		player->body.bodySize--;
	}
}
void addPlayerToMap(PlayerSpace *space, Player *player){
	int i = 0;
	for(i = 0; i < player->body.bodySize; i++){
		assert(player->body.bodyPoints[i].position.col >=0);
		assert(player->body.bodyPoints[i].position.row >=0);
		addPointToMap(space, &player->body.bodyPoints[i]);
	}
}
void removePlayerFromMap(PlayerSpace *space, Player *player){
	int i = 0;
	for(i = 0; i < player->body.bodySize; i++){
		assert(player->body.bodyPoints[i].position.col >=0);
		assert(player->body.bodyPoints[i].position.row >=0);
		removePointFromMap(space, &player->body.bodyPoints[i]);
	}
}
void setPointInMap(PlayerSpace *space, BodyPoint *point, int col, int row){
	point->position.col = col;
	point->position.row = row;
	addPointToMap(space, point);
}
void movePointInMap(PlayerSpace *space, BodyPoint *point, int direction){
	assert(point->player != NULL);
	removePointFromMap(space, point);
	assert(point->position.col >=0);
	assert(point->position.row >=0);
	movePoint(&point->position, direction);
	assert(point->position.col >=0);
	assert(point->position.row >=0);
	addPointToMap(space, point);
}
void removePointFromMap(PlayerSpace *space, BodyPoint *point){
	BodyPoint **mapPoint;
	mapPoint = &space->playerMap[point->position.row][point->position.col];
	if(*mapPoint == point){
		*mapPoint = NULL;
	}
}
void addPointToMap(PlayerSpace *space, BodyPoint *point){
	assert(point->player != NULL);
	assert(point->position.col >=0);
	assert(point->position.row >=0);
	BodyPoint **mapPoint;
	mapPoint = &space->playerMap[point->position.row][point->position.col];
	*mapPoint = point;
	return;
}

void movePoint(Point* point, int direction){
	assert (point != NULL);
	if(direction==STOP){
		return;
	}
	send_initial_message();
	send_value_to_server(1, point->row);
	send_value_to_server(2, point->col);
	send_value_to_server(3, direction);

	point->row = ask_value_from_server(1, 1);
	point->col = ask_value_from_server(2, 2);

}
// Console /////////////////////////////////////////////////////////////////////
void initScreen(){
	int n=0, m=0;
    (void) initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
    (void) noecho();       /* don't echo input */
    (void) curs_set(0);    /* no cursor */
    if (has_colors()) {
        start_color();
        /*
         * Simple color assignment, often all we need.
         */
        init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
        init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    }
	for(n = 0; n<MAX_COLS; n++){
			mvaddch(START_ROW-1,n+START_COL,'-');
	}
	for(n = 0; n<MAX_COLS; n++){
			mvaddch(START_ROW+MAX_ROWS,n+START_COL,'-');
	}
	for(m = 0;m<MAX_ROWS; m++){
		mvaddch(START_ROW+m,START_COL-1,'|');
		mvaddch(START_ROW+m,START_COL+ MAX_COLS,'|');
	}
}
void resetScreen(){
    finish(0);
}
static void finish(int sig) {
    endwin();
}
int getUserKey(){
	if (input_n > 0) {
		new_input = 1;
		return (input_i < input_n ? input_key[input_i++] : 'q');
	}
	if(halfdelay(1) != ERR){
		return getch();
	}else{
		return -1;
	}
}
void paintChar(int row, int col, char c){
	mvaddch(row+START_ROW,col+START_COL,c);
}
void repaintScreen(){
	refresh();
}
void statusBar(char *string){
	mvprintw(0,0, string);
//	mvprintw(1,0, "Pulsa 0 para seguir");
//	while(getUserKey() != '0');
//	mvprintw(0,0, "                                                                                  ");
//	mvprintw(1,0, "                                                                                  ");
}

void salir(){
	if (input_n > 0){
		printf("%s", trace_pos);
	}
	printf("FIN DEL PROGRAMA\n");
}

int readMapFile(char * fileName, char *charMap[MAP_ROWS]){
	int numRows= 0;
	char *line = (char*)malloc(MAP_COLS);
	FILE * fp = fopen(fileName,"r");
	while(fgets(line, MAP_COLS, fp) != NULL){
		strcpy(charMap[numRows++], line);
	}
	return numRows;
}
void guidePlayer(Guide *guide, Player *player){
	int impulse=player->impulse[player->direction]*2;
	if(guide->directionOneEnd == STOP){
		push(player, guide->directionOtherEnd,impulse);
	}else if(player->direction == invertDirection(guide->directionOneEnd)){
		push(player, guide->directionOtherEnd,impulse);
	}else if(player->direction == invertDirection(guide->directionOtherEnd)){
		push(player, guide->directionOneEnd,impulse);
	}else{
		push(player, STOP, impulse);
	}
}

void loadGuideMapFromCharMap(PlayerSpace *playerSpace, char **charMap){
	Guide *newGuide = NULL;
	int row=0, col=0;
	char * rowContent;
	char c;
	rowContent = charMap[row];
	//while( rowContent != NULL){
	while( row < MAP_ROWS){
		c = rowContent[col];
		while(c != '\0'){
			switch(c){
			case '.':
				newGuide = createCurveGuide(charMap, row, col);
				break;
			case ':':
				newGuide = createStopGuide(charMap, row, col);
				break;
			case '|':
				newGuide = createVerticalGuide(charMap, row, col);
				break;
			case '-':
				newGuide = createHorizontalGuide(charMap, row, col);
				break;
			case '\\':
				newGuide = createDownDiagonalGuide(charMap, row, col);
				break;
			case '/':
				newGuide = createUpDiagonalGuide(charMap, row, col);
				break;
			case '%':
				newGuide = createChangePointGuide(charMap, row, col);
				break;
			case 'N':
			case 'n':
			case 'E':
			case 'e':
			case 'S':
			case 's':
			case 'W':
			case 'w':
				newGuide = createFixedDirectionGuide(charMap, row, col, c);
				break;
			default:
				newGuide = NULL;
			}
			playerSpace->groundMap[row][col] = newGuide;
			if(newGuide != NULL){
				addData(playerSpace->guideNodes, newGuide);
			}
			newGuide = NULL;

			col++;
			c = rowContent[col];
		}
		row++;
		col = 0;
		rowContent = charMap[row];
	}
}

Guide *createFixedDirectionGuide(char **charMap, int row, int col, char c){
	Guide * newGuide = (Guide*) malloc(sizeof(Guide));
	newGuide->directionOneEnd = STOP;
	switch(c){
	case 'N':
		newGuide->directionOtherEnd=N;
		break;
	case 'n':
		newGuide->directionOtherEnd=NE;
		break;
	case 'E':
		newGuide->directionOtherEnd=E;
		break;
	case 'e':
		newGuide->directionOtherEnd=SE;
		break;
	case 'S':
		newGuide->directionOtherEnd=S;
		break;
	case 's':
		newGuide->directionOtherEnd=SW;
		break;
	case 'W':
		newGuide->directionOtherEnd=W;
		break;
	case 'w':
		newGuide->directionOtherEnd=NW;
		break;

	}
	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = c;
	newGuide->key = 0; //provisional
	newGuide->isActive = TRUE;
	return newGuide;
}
Guide *createStopGuide(char **charMap, int row, int col){
	Guide * newGuide = (Guide*) malloc(sizeof(Guide));
	newGuide->directionOneEnd = STOP;
	newGuide->directionOtherEnd= STOP;
	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = ':';
	newGuide->key = 0; //provisional
	newGuide->isActive = FALSE;
	newGuide->type=STOP_GUIDE;
	return newGuide;
}
Guide *createDownDiagonalGuide(char **charMap, int row, int col){
	Guide * newGuide = (Guide*) malloc(sizeof(Guide));
	newGuide->directionOneEnd = NW;
	newGuide->directionOtherEnd=SE;
	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = '\\';
	newGuide->key = 0; //provisional
	newGuide->isActive = FALSE;
	newGuide->type=DOWNDIAGONAL_GUIDE;
	return newGuide;
}
Guide *createUpDiagonalGuide(char **charMap, int row, int col){
	Guide * newGuide = (Guide*) malloc(sizeof(Guide));
	newGuide->directionOneEnd = NE;
	newGuide->directionOtherEnd=SW;
	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = '/';
	newGuide->key = 0; //provisional
	newGuide->isActive = FALSE;
	newGuide->type=UPDIAGONAL_GUIDE;
	return newGuide;
}
Guide *createHorizontalGuide(char **charMap, int row, int col){
	Guide * newGuide = (Guide*) malloc(sizeof(Guide));
	newGuide->directionOneEnd = W;
	newGuide->directionOtherEnd= E;
	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = '-';
	newGuide->key = 0; //provisional
	newGuide->isActive = FALSE;
	newGuide->type=HORIZONTAL_GUIDE;
	return newGuide;
}
Guide *createVerticalGuide(char **charMap, int row, int col){
	Guide * newGuide = (Guide*) malloc(sizeof(Guide));
	newGuide->directionOneEnd = N;
	newGuide->directionOtherEnd= S;
	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = '|';
	newGuide->key = 0; //provisional
	newGuide->isActive = FALSE;
	newGuide->type=VERTICAL_GUIDE;
	return newGuide;
}
Guide *createCurveGuide(char **charMap, int row, int col){
	Guide * newGuide;
	int dirsByRow =0, dirsByCol=0;
	int fromRow = 0, toRow = 0;
	int fromCol = 0, toCol = 0;
	int r,c;
	int foundDirections[2];
	int numFoundDirections = 0;
	// X in place of point to skip . detection!
	char railChars[3][3]={
		{'\\','|', '/'},
		{'-' ,'X', '-'},
		{'/' ,'|','\\'},
	};
	int directions[3][3]={
		{NW,  N,   NE},
		{W , STOP, E},
		{SW,  S,   SE},
	};
	newGuide = (Guide*) malloc(sizeof(Guide));
	if(row < 1){
		fromRow = 0;
	}else{
		fromRow = -1;
	}
	if(toRow == MAP_ROWS-1){
		toRow = 0;
	}else{
		toRow = 1;
	}
	if(col< 1){
		fromCol= 0;
	}else{
		fromCol = -1;
	}
	if(toCol == MAP_COLS-1){
		toCol=0;
	}else{
		toCol=1;
	}
	for(r = fromRow; r <=toRow; r++){
		for(c = fromCol; c <= toCol; c++){
			if(charMap[row+r][col+c] == railChars[1+r][1+c] ){
				foundDirections[numFoundDirections] = directions[1+r][1+c];
				numFoundDirections++;
			}
		}
	}
	// Buscar direcci�n de entrada:
	for(r = fromRow; r <=toRow; r++){
		dirsByRow = 0;
		for(c = fromCol; c <= toCol; c++){
			dirsByCol = 0;
			if(charMap[row+r][col+c] == railChars[1+r][1+c] ){
				foundDirections[numFoundDirections] = directions[1+r][1+c];
				numFoundDirections++;
			}
		}
	}

	for(c = 0; c < 3; c++){
		newGuide->posibleDirections[c] = STOP;
	}
	newGuide->directionOneEnd = STOP;
	newGuide->directionOtherEnd= STOP;

	if(numFoundDirections >= 2){
		newGuide->directionOtherEnd = foundDirections[1];
	}
	if(numFoundDirections >= 1){
		newGuide->directionOneEnd= foundDirections[0];
	}
	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = '.';
	newGuide->key = 0; //provisional
	newGuide->isActive = FALSE;
	newGuide->type=CURVE_GUIDE;
	return newGuide;

}
Guide *createChangePointGuide(char **charMap, int row, int col){
	Guide * newGuide;
	int foundCol, foundRow;
	int directionsByRow = 0;
	int multiDirectionsRow = -1;
	int uniDirectionRow = -1;
	int directionsByCol= 0;
	int multiDirectionsCol= -1;
	int uniDirectionCol=-1;
	int numPosibleDirections = 0;
	int fromRow = 0, toRow = 0;
	int fromCol = 0, toCol = 0;
	int r,c;
	int numFoundDirections = 0;
	// X in place of point to skip % detection!
	char railChars[3][3]={
		{'\\','|', '/'},
		{'-' ,'X', '-'},
		{'/' ,'|','\\'},
	};
	int foundDirections[3][3]={
		{STOP, STOP, STOP},
		{STOP, STOP, STOP},
		{STOP, STOP, STOP}
	};
	int directions[3][3]={
		{NW,  N,   NE},
		{W , STOP, E},
		{SW,  S,   SE},
	};
	newGuide = (Guide*) malloc(sizeof(Guide));
	if(row < 1){
		fromRow = 0;
	}else{
		fromRow = -1;
	}
	if(toRow == MAP_ROWS-1){
		toRow = 0;
	}else{
		toRow = 1;
	}
	if(col< 1){
		fromCol= 0;
	}else{
		fromCol = -1;
	}
	if(toCol == MAP_COLS-1){
		toCol=0;
	}else{
		toCol=1;
	}
	for(r = fromRow; r <=toRow; r++){
		for(c = fromCol; c <= toCol; c++){
			if(charMap[row+r][col+c] == railChars[1+r][1+c] ){
				foundDirections[1+r][1+c] = directions[1+r][1+c];
				numFoundDirections++;
			}
		}
	}

	foundCol = 0;
	directionsByRow = 0;
	multiDirectionsRow = -1;
	uniDirectionRow = -1;
	// search a row with only one direction
	for(r = 0; r < 3; r++){
		directionsByRow=0;
		for(c = 0; c < 3; c++){
			if(foundDirections[r][c] != STOP){
				directionsByRow++;
				foundCol = c;
			}
		}
		// if this is the row with the input direction...
		if((directionsByRow == 1) && (foundCol == 1)){
			uniDirectionRow = r;
			multiDirectionsRow = (r + 2) % 4; // trick to find the opposite row
		}
	}
	foundRow = 0;
	directionsByCol= 0;
	multiDirectionsCol= -1;
	uniDirectionCol=-1;
	// search a col with only one direction
	for(c = 0; c < 3; c++){
		directionsByCol = 0;
		for(r = 0; r < 3; r++){
			if(foundDirections[r][c] != STOP){
				directionsByCol++;
				foundRow = r;
			}
		}
		// if this is the col with the input direction...
		if((directionsByCol== 1) && (foundRow == 1)){
			uniDirectionCol=c;
			multiDirectionsCol= (c + 2) % 4;
		}
	}
	for(c = 0; c < 3; c++){
		newGuide->posibleDirections[c] = STOP;
	}
	numPosibleDirections = 0;
	// Put the posible directions in the array
	if(uniDirectionRow != -1){
		newGuide->directionOneEnd = foundDirections[uniDirectionRow][1];
		for(c = 0; c < 3; c++){
			if(foundDirections[multiDirectionsRow][c] != STOP){
				newGuide->posibleDirections[numPosibleDirections] = foundDirections[multiDirectionsRow][c];
				numPosibleDirections++;
			}
		}
	}else if(uniDirectionCol != -1){
		newGuide->directionOneEnd = foundDirections[1][uniDirectionCol];
		for(r = 0; r < 3; r++){
			if(foundDirections[r][multiDirectionsCol] != STOP){
				newGuide->posibleDirections[numPosibleDirections] = foundDirections[r][multiDirectionsCol];
				numPosibleDirections++;
			}
		}
	}else{
		//invalid guide!!
		assert(FALSE);
	}



	newGuide->directionOtherEnd= newGuide->posibleDirections[0];

	newGuide->position.col = col;
	newGuide->position.row = row;
	newGuide->aspect = '%';
	newGuide->key = 0; //provisional
	newGuide->isActive = FALSE;
	newGuide->type=CHANGEPOINT_GUIDE;
	return newGuide;

}
// Actions ///////////////////////////////////////////////////////////////

void addAction(ActionGroup *actionGroup, void (*function)(void), int key){
	Action *action= (Action*) malloc(sizeof(Action));
	action->function = function;
	action->key =key;
	actionGroup->actions[actionGroup->numActions] = action;
	actionGroup->numActions++;
}

void processKeyboardEvent(int key, ActionGroup *actionGroup){
	int n;
	for(n=0; n < actionGroup->numActions; n++){
		if(actionGroup->actions[n]->key == key){
			actionGroup->actions[n]->function();
		}
	}
}
