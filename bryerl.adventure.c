#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define USABLE_ROOMS		7
#define MAX_CONNECTIONS		6

struct Room {
	char name[15];
	int connectionCount;	
	char type[11];
	struct Room* connections[MAX_CONNECTIONS];	
};

struct Room A;
struct Room B;
struct Room C;
struct Room D;
struct Room E;
struct Room F;
struct Room G;
struct Room* Rooms[USABLE_ROOMS] = { &A, &B, &C, &D, &E, &F, &G };
char mostRecentDirectory[50];
pthread_t mainThread;
pthread_t timeThread;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;


//This function finds the most recently added rooms directory
void FindRecentDirectory()
{
  int newestDirTime = -1; // Modified timestamp of newest subdir examined
  char targetDirPrefix[32] = "bryerl.rooms."; // Prefix we're looking for
  char newestDirName[256]; // Holds the name of the newest dir that contains prefix
  memset(newestDirName, '\0', sizeof(newestDirName));

  DIR* dirToCheck; // Holds the directory we're starting in
  struct dirent *fileInDir; // Holds the current subdir of the starting dir
  struct stat dirAttributes; // Holds information we've gained about subdir
  dirToCheck = opendir("."); // Open up the directory this program was run in

  if (dirToCheck > 0) // Make sure the current directory could be opened
  {
    while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
    {
      if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
      {
        stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

        if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
        {
          newestDirTime = (int)dirAttributes.st_mtime;
          memset(newestDirName, '\0', sizeof(newestDirName));
          strcpy(newestDirName, fileInDir->d_name);
        }
      }
    }
  }
  closedir(dirToCheck); // Close the directory we opened
  sprintf(mostRecentDirectory, "%s/", newestDirName);
}

//This function gets all the file names (which are identical to room names) and assigns these names to the 7 different room structs
void NameRooms()
{
	int i = 0;
	DIR *directory;
	struct dirent *dir;
	struct stat stBuf;
	directory = opendir(mostRecentDirectory);
	if (directory)
	{
		while ((dir = readdir(directory)) != NULL)
		{
			stat(dir->d_name, &stBuf);
			if (!strstr(dir->d_name, ".")) //this simply checks for the two directories "." and ".." because they were appearing when I was printing all given files in a directory
			{
				strcpy(Rooms[i]->name, dir->d_name);
				i++;
			}
		}
		closedir(directory);
	}
}

//This recreates all the connections from each room file
void CreateConnections()
{
	int i = 0;
	DIR *directory;
	struct dirent *dir;
	struct stat stBuf;
	char currentFileName[50];
	FILE* file;
	int bufferSize = 256;
	char readLine[bufferSize];
	struct Room* roomPointer;
	directory = opendir(mostRecentDirectory);
	struct Room* connectedRoom;
	
	if (directory)
	{
		while ((dir = readdir(directory)) != NULL)
		{
			stat(dir->d_name, &stBuf);
			if (!strstr(dir->d_name, "."))							//This line simply filters out the directories "." and ".."
			{
				for (i = 0; i < USABLE_ROOMS; i++)					
				{
					if (strcmp(dir->d_name, Rooms[i]->name) == 0)
					{
						roomPointer = Rooms[i];
					}
				}
				
				sprintf(currentFileName, "%s%s", mostRecentDirectory, dir->d_name);
				file = fopen(currentFileName, "r");
				
				while ((fgets(readLine, bufferSize, file)) != 0)				//This reads in data from the file
				{
					readLine[strcspn(readLine, "\n")] = 0;						//This simply trims off the newline character from each file line
					
					if (strstr(readLine, "CONNECTION"))							//If the line contains the word CONNECTION, we know its a room connection
					{
							strcpy(readLine, readLine+14);						//This trims the starting text and leaves us with only the room name
							
							for (i = 0; i < USABLE_ROOMS; i++)
							{
								if (strcmp(Rooms[i]->name, readLine) == 0)		//Finds the room struct that has the same name as the file line
								{
									connectedRoom = Rooms[i];
								}
							}
							roomPointer->connections[roomPointer->connectionCount] = connectedRoom;
							roomPointer->connectionCount++;
					}
					else if (strcmp(readLine, "TYPE"))
					{
						strcpy(readLine, readLine+11);
						strcpy(roomPointer->type, readLine);
					}
				}
			}
		}
		closedir(directory);
	}
}

//This function gets the current time
void* GetTime()
{
	pthread_mutex_lock(&myMutex);
	FILE* filePtr;
	char currentTime[100];
	filePtr = fopen("currentTime.txt", "w");
	time_t rawTime;
	struct tm *tm;
	
	time(&rawTime);
	tm = localtime(&rawTime);
	
	strftime(currentTime, 100, "%I:%M%p, %A, %B %d, %G", tm);
	fprintf(filePtr, "%s", currentTime);
	fclose(filePtr);
	pthread_mutex_unlock(&myMutex);
}

void* RunGame(void* args)
{
	struct Room* currentRoom;
	char connectionString[100];
	char userEntry[100];
	int i;
	int steps = 0;
	int found = 0;
	struct Room** roomsVisited = malloc(sizeof(struct Room*));
	FILE* timeFile;
	char timeData[100];
	pthread_mutex_lock(&myMutex);
	pthread_create(&timeThread, NULL, GetTime, NULL);

	//This finds the one room with type "STARTT_ROOM" and sets it as currentRoom to get the game started
	for (i = 0; i < USABLE_ROOMS; i++)
	{
		if (strcmp(Rooms[i]->type, "START_ROOM") == 0)
		{
			currentRoom = Rooms[i];
			break;
		}
	}
	
	//Continues to loop while the current room's roomtype is not "END_ROOM"
	while (strcmp(currentRoom->type, "END_ROOM") != 0)
	{
		found = 0;
		printf("CURRENT ROOM: %s\n", currentRoom->name);
		strcpy(connectionString, "POSSIBLE CONNECTIONS: ");
		
		//This loops through all of the rooms connections and constructs a string of connecting room names
		for (i = 0; i < currentRoom->connectionCount; i++)
		{
			strcat(connectionString, currentRoom->connections[i]->name);
			if (i == currentRoom->connectionCount - 1)
			{
				strcat(connectionString, ".");
			}
			else
			{
				strcat(connectionString, ", ");
			}
		}
		
		printf("%s\n", connectionString);
		printf("WHERE TO? >");
		fgets(userEntry, 100, stdin);
		userEntry[strcspn(userEntry, "\n")] = 0;
		printf("\n");
		
		//if user is asking for time, run time function and return
		if(strcmp(userEntry, "time") == 0)
		{
			pthread_mutex_unlock(&myMutex);
			pthread_join(timeThread, NULL);
			timeFile = fopen("currentTime.txt", "r");
			fgets(timeData, 100, timeFile);
			printf("%s\n\n", timeData);
			pthread_mutex_lock(&myMutex);
			pthread_create(&timeThread, NULL, GetTime, NULL);
		}
		//otherwise, loop through connections and see if the room the user is requesting is a connecting room
		else
		{
		for (i = 0; i < currentRoom->connectionCount; i++)
		{
			if (strcmp(userEntry, currentRoom->connections[i]->name) == 0)
			{
				steps++;
				roomsVisited = realloc(roomsVisited, steps * sizeof(struct Room *)); //this grows the array of room structs
				currentRoom = currentRoom->connections[i];
				roomsVisited[steps-1] = currentRoom;
				found = 1;
				break;
			}
		}
		
		if (found == 0)
		{
			printf("HUH? I DON'T UNDERSTAND THAT ROOM, TRY AGAIN.\n\n");
		}
		}
	}
	
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
	for (i = 0; i < steps; i++)
	{
		printf("%s\n", roomsVisited[i]->name);
	}
	
	free(roomsVisited);
}

void main()
{
	int result_int;
	FindRecentDirectory();
	NameRooms();
	CreateConnections();
	(void) pthread_create(&mainThread, NULL, RunGame, NULL);
	(void) pthread_join(mainThread, NULL);
}