#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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
			if (!strstr(dir->d_name, "."))
			{
				strcpy(Rooms[i]->name, dir->d_name);
				i++;
			}
		}
		closedir(directory);
	}
}

void main()
{
	FindRecentDirectory();
	NameRooms();
	int i;
	for (i = 0; i < USABLE_ROOMS; i++)
	{
		printf("%s\n", Rooms[i]->name);
	}
}