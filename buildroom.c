#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { false, true } bool;
#define MAX_CONNECTIONS		6
#define MIN_CONNECTIONS		3
#define TOTAL_ROOMS			10
#define USABLE_ROOMS		7
#define ROOM_TYPE_COUNT		3

struct Room {
	char name[10];
	int connectionCount;	
	char type[11];
	struct Room* connections[MAX_CONNECTIONS];	
};

const char* roomPool[TOTAL_ROOMS] = { "Dungeon", "Cellar", "Library", "Hallway", "Kitchen", "Attic", "Observatory", "Bedroom", "Labratory", "Test Chambers" };
const char* roomTypes[ROOM_TYPE_COUNT] = { "START_ROOM", "MID_ROOM", "END_ROOM" };
int roomNameAssigned[TOTAL_ROOMS] = { 0 };
int roomTypeAssigned[USABLE_ROOMS] = { 0 };
struct Room A;
struct Room B;
struct Room C;
struct Room D;
struct Room E;
struct Room F;
struct Room G;
struct Room* Rooms[USABLE_ROOMS] = { &A, &B, &C, &D, &E, &F, &G };

//Randomly assigns room names to rooms
void randomizeRoomNames()
{
	int roomSelection = rand() % TOTAL_ROOMS;

	for (int i = 0; i < USABLE_ROOMS; i++)
	{
		while (roomNameAssigned[roomSelection] == 1)
		{
			roomSelection = rand() % TOTAL_ROOMS;
		}

		strcpy(Rooms[i]->name, roomPool[roomSelection]);
		roomNameAssigned[roomSelection] = 1;
	}
}

//Randomly assigns room types to each room
void randomizeRoomTypes()
{
	int roomSelection = rand() % USABLE_ROOMS;

	for (int i = 0; i < USABLE_ROOMS; i++)
	{
		while (roomTypeAssigned[roomSelection] == 1)
		{
			roomSelection = rand() % USABLE_ROOMS;
		}

		if (i == 0)
		{
			strcpy(Rooms[roomSelection]->type, "START_ROOM");
			roomTypeAssigned[roomSelection] = 1;
		}
		else if (i > 0 && i < USABLE_ROOMS - 1)
		{
			strcpy(Rooms[roomSelection]->type, "MID_ROOM");
			roomTypeAssigned[roomSelection] = 1;
		}
		else
		{
			strcpy(Rooms[roomSelection]->type, "END_ROOM");
			roomTypeAssigned[roomSelection] = 1;
		}
	}
}

void initializeRooms()
{
	for (int i = 0; i < USABLE_ROOMS; i++)
	{
		Rooms[i]->connectionCount = 0;
		strcpy(Rooms[i]->name, "");
		strcpy(Rooms[i]->type, "");		

		for (int j = 0; j < MAX_CONNECTIONS; j++)
		{
			Rooms[i]->connections[j] = NULL;
		}
	}	
}


// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()
{
	bool correctConnections = true;

	for (int i = 0; i < USABLE_ROOMS; i++)
	{
		if (Rooms[i]->connectionCount < 3 || Rooms[i]->connectionCount > 6)
		{
			correctConnections = false;
		}
	}
	
	return correctConnections;
}

// Returns true if a connection can be added from Room x, false otherwise
bool CanAddConnectionFrom(struct Room* x)
{
	if (x->connectionCount < MAX_CONNECTIONS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room* x)
{
	bool firstFound = false;
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (firstFound == false && x->connections[i] == NULL)
		{
			x->connections[i] = y;
			x->connectionCount++;
			firstFound = true;
		}
	}
}

struct Room* GetRandomRoom()
{
	int roomSelection = rand() % 7;
	struct Room* A = Rooms[roomSelection];	
	return A;
}

//Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct Room* x, struct Room* y)
{
	if (strcmp(x->name, y->name) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AddRandomConnection()
{
	struct Room* A;  //Maybe a struct, maybe global arrays of ints
	struct Room* B;	

	while (1)
	{
		A = GetRandomRoom(Rooms);

		if (CanAddConnectionFrom(A) == 1)
			break;
	}

	do
	{
		B = GetRandomRoom(Rooms);
	} while (CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1);

	ConnectRoom(A, B);
	ConnectRoom(B, A);
}

void main()
{
	srand((unsigned int)time(NULL));
	randomizeRoomNames();
	randomizeRoomTypes();

	for (int i = 0; i < USABLE_ROOMS; i++)
	{
		printf("%s: %s\n", Rooms[i]->name, Rooms[i]->type);
	}

	/*initializeRooms(Rooms);	

	// Create all connections in graph
	while (IsGraphFull(Rooms) == false)
	{
	AddRandomConnection(Rooms);
	}*/

	system("pause");
}