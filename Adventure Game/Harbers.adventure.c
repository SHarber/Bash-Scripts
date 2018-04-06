/**************************************************************************************************
 * Author:              Sarah Harber
 *
 * Last Updated:        7/21/17
 *
 * Description:         Program to be the interface for an adventure game.
 *
 * Specifications:      - Program must find the Directory with the earliest time.
 *						- Program must read room information from files.
 *						- Rules of the game are as follows:
 *							1.) The player wins by getting from the start room to the end room.
 *							2.) Trying to go into an incorrect path does not increment the step
 *								count.
 *							3.) Player must type the name exactly as listed.
 *************************************************************************************************/

// Add the Include statements needed for the program
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

// Add necessary variables & types.
	//File type to store room type.
		enum Type{START_ROOM, MID_ROOM, END_ROOM};

	// Char Array to store newest directory.
		char newestDirName[256];

	// Create Variables needed for buffer
		char buffer[32];
    	char *b = buffer;
    	size_t bufsize = 32;
    	size_t characters;

	// Counting Variables.
		int i;
		int a;

	//Variable to hold index of current location
	   int currentLocation;

	// Declare a New Struct Which will contain an Array
		struct NAMES{char connName[256];};

	// Create a struct to hold all of the room information to be read from a file.
	struct ROOM{
        	char Name[256];     			// Each room will have a distinct room name
	        enum Type RoomType; 			// Each room will either be a starting, mid or end room.
        	struct NAMES Connection[6];		// Each room will be connected to at most 6 rooms.
			int numOfConnections;			// Each room will have an integer to show the number of connections.
			};

	// Declare 7 room structs to store all room information from a file to play the game.
	   struct ROOM PLAYROOMS[7];

	   struct NAMES PATH[100];

// Declare Function Prototypes.
	void getNewestFile();		// Function to obtain the newest directory with a certain prefix.
	void readInFiles();			// Function to read in the rooms from a directory to initate the game.
	void findStart();			// Function to find the strting location.
	void playGame();			// Function to play the game.



/************************************************************************************************
 * Function Name: userInterface
 * Description:  Function to run the user interface.
 *************************************************************************************************/
void playGame(){
	printf("*****************************************************************************\n");
	printf("Welcome to Sarah Harber's Adventure Game!  Are You Ready to Play?\n\n");
	printf("Goal: The goal of the game is to get to the start room to the end room.\n\n");
	printf("How to Play:  You will start out in the start room.  Type the Name of the\n");
	printf("room you would like to enter.  If it is the end room, you win! IF not\n");
	printf("choose another room\n");
	printf("*****************************************************************************\n");


	//Find the Starting Location & set Current Location to that Index.
		findStart();
			memset(PATH, '\0', sizeof(PATH));
			int numMoves = 0;

		do{
			int valid = 0; //If move is valid valid = 1;
			do{

			//Print Current Location Name
			printf("\n\nCURRENT LOCATION: %s\n", PLAYROOMS[currentLocation].Name);

			// Print all connections
				printf("POSSIBLE CONNECTIONS: ");
				for(i = 0; i < PLAYROOMS[currentLocation].numOfConnections; i++)
				{
					printf("%s", PLAYROOMS[currentLocation].Connection[i].connName);
					if((i+1) != PLAYROOMS[currentLocation].numOfConnections)
						printf(", ");
					else
						printf(".\n");
				}

				// Print Where to Command.
					printf("WHERE TO? >");

				// Get user Input
					characters = getline(&b,&bufsize,stdin);
					strtok(buffer, "\n");

					// Variable to determine if Valid Move

					for(a = 0; a < 6; a++)
					{
						strtok(PLAYROOMS[currentLocation].Connection[a].connName, "\n");
						if(strcmp(PLAYROOMS[currentLocation].Connection[a].connName, buffer) == 0)
							valid = 1;
					}

					if (valid == 0)
					{
						printf("\n\nHUH? I DON'T UNDERSTAND THAT ROOM, TRY AGAIN\n\n");
					}
				//Check to see if its a Valid MOve
			}while(valid < 1);

			// If Move is Valid, Then Find New room & update current Location.
			for(i = 0; i < 7; i++)
			{
				strtok(PLAYROOMS[i].Name, "\n");
				if(strcmp(PLAYROOMS[i].Name, buffer) == 0)
					currentLocation = i;
			}
			strcpy(PATH[numMoves].connName, PLAYROOMS[currentLocation].Name);
			numMoves++;

		}while(PLAYROOMS[currentLocation].RoomType != 2);
		printf("\n\nCongratulations!  You Won!");
		printf("You took %d Steps.  You Path to Victory Was:", numMoves);
		for(i=0; i<numMoves; i++)
			printf("\n%s", PATH[i].connName);
			printf("\n");
	}


int main(){
	getNewestFile(); 		// Get Newest Directory
	readInFiles(); 		// Read In Files to Play the Game.
	playGame();			// Play the Game
	exit(0);
};

/*************************************************************************************************
 * Function Name: 		getNewestFile
 * Description:  		Function to find the Newest directory with a specific.  Code taken from
						additional reading material.
 *************************************************************************************************/
void getNewestFile(){
	// Modified timestamp of newest subdir examined
		int newestDirTime = -1;

	// Prefix we're looking for
		char targetDirPrefix[32] = "harbers.rooms.";

	// Holds the name of the newest dir that contains prefix
		memset(newestDirName, '\0', sizeof(newestDirName));

	// Holds the directory we're starting in
		DIR* dirToCheck;

	// Holds the current subdir of the starting dir
		struct dirent *fileInDir;

	// Holds information we've gained about subdir
		struct stat dirAttributes;

	// Open up the directory this program was run in
		dirToCheck = opendir(".");

	// Make sure the current directory could be opened
		if (dirToCheck > 0)
		{
		// Check each entry in dir
		while ((fileInDir = readdir(dirToCheck)) != NULL)
		{
			// If entry has prefix
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
				// Get attributes of the entry
					stat(fileInDir->d_name, &dirAttributes);

				// If this time is bigger
					if ((int)dirAttributes.st_mtime > newestDirTime)
					{
						newestDirTime = (int)dirAttributes.st_mtime;
						memset(newestDirName, '\0', sizeof(newestDirName));
						strcpy(newestDirName, fileInDir->d_name);

					}
			}
		}
	  }
	  closedir(dirToCheck); // Close the directory we opened
}

/*************************************************************************************************
 * Function Name: 	readInFiles
 * Description:  	Function to read in the information from each file.
 *************************************************************************************************/
void readInFiles(){
	// Move into directory.
		chdir(newestDirName);

	// Create file pointer
		FILE * filePointer = NULL;

	// Create fileNames and buffers
		char fileName[256];
		char lineBuffer[256]; // To Store Line
		char valueBuffer[256]; // To Store Variable Needed.


	// Loop to read all files
	for (i=0; i < 7; i++)
	{
		// Initialize to null terminators
			memset(fileName, '\0', sizeof(fileName));
			memset(lineBuffer, '\0', sizeof(lineBuffer));
			memset(valueBuffer, '\0', sizeof(valueBuffer));
			memset(PLAYROOMS[i].Connection, '\0', sizeof(PLAYROOMS[i].Connection));


		// Get File Name
			sprintf(fileName, "Room%d", i);

		// Open first file to read
			filePointer = fopen(fileName, "r");

		// Check that the file opens.
			if(filePointer == NULL)
			{
				printf("Error while opening file");
				exit(1);
			}

		// Variable to initialize connections to -2 (Since each room will have a Room Name & Type)
			int countConnections = -2;

		// Get the Lines from the File.
			while (fgets(lineBuffer, sizeof(lineBuffer), filePointer) != NULL)
			{
				// Cut Table at the Colon
					strtok(lineBuffer,":");
					strcpy(valueBuffer,strtok(NULL, "\n"));

					// Remove Leading Space Char
						if (valueBuffer[0] == ' ')
								memmove(valueBuffer, valueBuffer+1, strlen(valueBuffer));

						strtok(valueBuffer, "\n");
				// Copy Information from File into Structs
					if (strcmp(lineBuffer, "ROOM NAME") == 0)
						strcpy(PLAYROOMS[i].Name, valueBuffer);
					else if (strcmp(lineBuffer, "CONNECTION 1") == 0)
						strcpy((PLAYROOMS[i].Connection[0].connName), valueBuffer);
					else if (strcmp(lineBuffer, "CONNECTION 2") == 0)
						strcpy((PLAYROOMS[i].Connection[1].connName), valueBuffer);
					else if (strcmp(lineBuffer, "CONNECTION 3") == 0)
						strcpy((PLAYROOMS[i].Connection[2].connName), valueBuffer);
					else if (strcmp(lineBuffer, "CONNECTION 4") == 0)
						strcpy((PLAYROOMS[i].Connection[3].connName), valueBuffer);
					else if (strcmp(lineBuffer, "CONNECTION 5") == 0)
						strcpy((PLAYROOMS[i].Connection[4].connName), valueBuffer);
					else if (strcmp(lineBuffer, "CONNECTION 6") == 0)
						strcpy((PLAYROOMS[i].Connection[5].connName), valueBuffer);
					else if (strcmp(lineBuffer, "ROOM TYPE") == 0)
						{
						if (strcmp(valueBuffer, "START_ROOM") == 0)
								PLAYROOMS[i].RoomType = 0;
							else if (strcmp(valueBuffer, "MID_ROOM") == 0)
								PLAYROOMS[i].RoomType = 1;
							else if (strcmp(valueBuffer, "END_ROOM") == 0)
								PLAYROOMS[i].RoomType =2;

						}



				// Incriment Connections
					countConnections++;
			}
		//Store Room Connections
			PLAYROOMS[i].numOfConnections = countConnections;
							// printf("%s, %s, %s, %s, %s, %s", PLAYROOMS[i].Connection[0].connName, PLAYROOMS[i].Connection[1].connName, PLAYROOMS[i].Connection[2].connName, PLAYROOMS[i].Connection[3].connName, PLAYROOMS[i].Connection[4].connName, PLAYROOMS[i].Connection[5].connName);
    }
}

/*************************************************************************************************
 * Function Name: 	findStart
 * Description:  	Function to Find the Start Location.
 *************************************************************************************************/
void findStart(){
	// Loop through PLAYROOMS and find the starting room.
			for (i = 0; i < 7; i++)
			{
				if(PLAYROOMS[i].RoomType == 0)
					currentLocation = i;
			}
}
