#include "list.h"
#define MAX_BODY_SIZE 300
#define NUM_PLAYERS 100
#define MAX_PLAYERS 50
#define MAX_COLS 80
#define MAX_ROWS 25
#define NUM_RANDOM_PLAYERS 20 
#define NUM_STATIC_PLAYERS 20 
#define RANDOM_PLAYER_BODY_CHAR '.' 
#define HUMAN_PLAYER_BODY_CHAR '#' 
#define START_COL 5 
#define START_ROW 5
#define MAP_COLS 80 
#define MAP_ROWS 25 
#define CELL_SIZE 1 
#define MAX_BODYMAPS 10
#define MAX_BODYMAP_WIDTH 200 
#define MAX_BODYMAP_HEIGHT 200 
#define MAX_ACTIONS 20 

// hum...
#define COSINE_OF_45_DEGREES 1 

#define WA_BOLD A_BOLD

typedef struct player Player;
typedef struct magneto Magneto;
typedef struct bodyPoint BodyPoint;
typedef struct point Point;
typedef struct playerBody PlayerBody;
typedef struct playerSpace PlayerSpace;
typedef char *BodyMap[MAX_BODYMAP_HEIGHT]; 
typedef struct newPlayerSpace NewPlayerSpace;
typedef struct guide Guide;
typedef struct action Action;
typedef struct actionGroup ActionGroup;

struct action{
	int key;
	void (*function)(void);
};

struct actionGroup{
	int numActions;
	Action *actions[MAX_ACTIONS];
};

enum{
	STOP=0,
	N   =1,
	NE  =2,
	E   =3,
	SE  =4,
	S   =5,
	SW  =6,
	W   =7,
	NW  =8 
};

struct point{
	int col;
	int row;
};

struct guide{
	Point position;
	char aspect;
	int color;
	int isActive;
	int directionOneEnd;
	int directionOtherEnd;
	int posibleDirections[3];
	char key;
	int type;
};
enum{
	CURVE_GUIDE,
	STOP_GUIDE,
	VERTICAL_GUIDE,
	HORIZONTAL_GUIDE,
	DOWNDIAGONAL_GUIDE,
	UPDIAGONAL_GUIDE,
	CHANGEPOINT_GUIDE,
	FIXEDDIRECTION_GUIDE 
};
struct bodyPoint{
	Point position;
	Player* player;
	char aspect;
};

struct playerBody{
	int type;
	int visible;
	int tangible;
	BodyPoint bodyPoints[MAX_BODY_SIZE];
	BodyMap* bodyMaps[MAX_BODYMAPS];
	int bodyMapsSize;
	int bodySize;
	int bodyWidth;
	int bodyHeight;
	int color;
};

struct player{
	int id;
	int type;
	int retired;
	PlayerBody body;
	Point basePosition;
	int impulse[9]; // impulse in 8 directions + stop (brakes)
	int direction;
	List *magnetos;
	int stepCounter;
	void (*pilot)(Player *p);
	void (*onCrash)(Player *p, Player *playerContacted, PlayerSpace *space);
	void (*onPushed)(Player *p, int direction);
};

struct magneto{
	Player *source;
	int direction;
	int magneticSense; // 1: attraction, -1: repulsion
	int intensity; 
};

struct playerSpace{
	List *playerNodes;
	BodyPoint *playerMap[MAP_ROWS][MAP_COLS];
	List *guideNodes;
	Guide *groundMap[MAP_ROWS][MAP_COLS];
	ActionGroup *actionGroup;
};

enum{
	SOLID_BODY=0,
	FLEXIBLE_BODY=1 
};
enum{
	MONSTER_TYPE=0,
	FRIEND_TYPE=1
	//...
};
enum{
	NO_HUMAN_PLAYER_ID=0,
	HUMAN_PLAYER_ID=1 
};
enum{
	 WALL_CRASH=0,
	 PLAYER_CRASH=1 
};


void onPushed(Player *player, int direction);
void onCrashBiteMonster(Player *player, Player *playerContacted, PlayerSpace *space);
void onCrashEnlargeMonster(Player *player, Player *playerContacted, PlayerSpace *space);
void onCrashPush(Player *player, Player *playerContacted, PlayerSpace *space);
void pushedPilot(Player *p);
void gravitationalPilot(Player *p);
void randomPilot(Player *p);
void circlePilot(Player *p);
void keyboardPilot(Player *p);
void paintPlayer(Player* player);
void moveSolidPlayer(PlayerSpace * space, Player* player);
void moveFlexiblePlayer(PlayerSpace * space, Player* player);
void movePlayer(PlayerSpace * space, Player* player);
int pointIsMovingToWall(const Point* point, int direction);
int blockIsMovingToWall(const Point* point, int direction, int width, int height);
int isWallContacted(Player* player);
void crash(List *crashedPlayers, const Player* const player, PlayerSpace* space);
void playerCrash(Player* from, Player* to);
void incBodySize(Player* player, PlayerSpace *space);
void decBodySize(Player* player, PlayerSpace *space);
void movePoint(Point* point, int direction);
int userKeyboardDirection();
int randomDirection();
void initScreen();
void resetScreen();
int getUserKey();
void paintChar(int col, int row, char c);
void setRandomDirection(Player* player);
void repaintScreen();
void erasePlayer(Player* player);
void setupHumanPlayer(PlayerSpace * space, Player* player);
void setupRandomPlayer(PlayerSpace * space, Player* player);
PlayerSpace *setupPlayerSpace();
void freePlayerSpace(PlayerSpace* space);
void setPointInMap(PlayerSpace *space, BodyPoint *point, int x, int y);
void movePointInMap(PlayerSpace *space, BodyPoint *point, int direction);
void removePointFromMap(PlayerSpace *space, BodyPoint *point);
void removePlayerFromMap(PlayerSpace *space, Player *player);
void addPointToMap(PlayerSpace *space, BodyPoint *point);
void setBodyPointsOwner(Player *player);
void setupEmptyPlayer(Player * player);
void setupFlexiblePlayer(PlayerSpace *space, Player* player);
void gameTic();
void salir();
void addPlayerToMap(PlayerSpace *space, Player *player);
void setupBodyMap(Player *player); 
void loadBodyMap(Player *player, BodyMap * bodyMap);
void selectBodyMap(Player *player, PlayerSpace *space, int numBodyMap);
void statusBar(char *string);
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
	);
void loadBodyMaps(Player *player, BodyMap * bodyMap);
void addMagnetoToPlayer(Player *player, Magneto *magneto);
void removeMagnetoFromPlayer(Player *player, Magneto *magneto);
void reactToMagnetos(Player *player, PlayerSpace *playerSpace);
int invertDirection(int direction);
int directionToApproach(Player *player, Player *target);
Point centerOfPlayer(Player *player);
Magneto *createMagneto(Player *source, int direction, int magneticSense, int intensity);
void transformImpulse(Player *player);
void absorbImpulse(Player *player);
void push(Player *player, int direction, int impulse);
void magneticInfluence(Player *player, Magneto *magneto);
int distance(Point from, Point to);
int isqrt(int val);
void freeNewPlayerSpace(NewPlayerSpace* space);
void guidePlayer(Guide *guide, Player *player);
void loadGuideMapFromCharMap(PlayerSpace *playerSpace, char **charMap);
	
Guide *createDownDiagonalGuide(char **charMap, int row, int col);
Guide *createUpDiagonalGuide(char **charMap, int row, int col);
Guide *createHorizontalGuide(char **charMap, int row, int col);
Guide *createVerticalGuide(char **charMap, int row, int col);
Guide *createCurveGuide(char **charMap, int row, int col);
Guide *createChangePointGuide(char **charMap, int row, int col);
Guide *createFixedDirectionGuide(char **charMap, int row, int col, char c);
Guide *createStopGuide(char **charMap, int row, int col);
int readMapFile(char * fileName, char *charMap[MAX_ROWS]); 
void reactToActiveGuides(Player *player, PlayerSpace *playerSpace);
void paintGuide(Guide* guide);
void addAction(ActionGroup *actionGroup, void (*function)(void), int key);
void processKeyboardEvent(int key, ActionGroup *actions);
