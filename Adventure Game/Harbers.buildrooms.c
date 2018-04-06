/**************************************************************************************************
 * Author:              Sarah Harber
 *
 * Last Updated:        7/25/17
 *
 * Description:         Program to generate 7 different room files to be used to play a
 *                      text based adventure game.
 *
 * Specifications:      - Each room should have a distinct name chosen from 1 of 10 room names.
 *                      - Each room should have a minimum of 3 connections
 *                      - Each room should have a maximum of 6 connections
 *                      - No room should have 2 connections to the same room
 *                      - A room cannot be connected to itself.
 *                      - If a room is connected to another room that room must also be
 *                        connected to the first room.
 *                      - Each room should have its own file
 *                      - Room files should be stored in a directory called Harbers.Rooms.$$
 *************************************************************************************************/
 
// Set #include files for rooms program
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// Declare necessary variables, data types, and structs for the program.
	// Decalre global constants.
		const MAX_NUM_ROOMS = 7; // Maximum number of rooms
		const MAX_NUM_CONNECTIONS = 6; // Maximum number of connections a room can have.
		const NUM_ROOM_NAMES = 10; // Number of room names to be used.
		const MIN_NUM_CONNECTIONS = 3;
	
	// Declare Counting/Loop Variable
		int i;
		int a;

		
	 // Variable to hold the directory path name.
	 	char directoryPath[256];

	// Declare enum for room types.
		enum Type{START_ROOM, MID_ROOM, END_ROOM};

	// Create a struct to hold all of the room information to be written to a file.
		struct ROOM{
        	char Name[256];     			// Each room will have a distinct room name
	        enum Type RoomType; 			// Each room will either be a starting, mid or end room.
        	int roomConnections[6];			// Each room can have a maximum of 6 connections
	        int numOfConnections;			// Each room will have an integer to show the number of connections.
		};

	// Declare 7 room structs to hold information f
	   struct ROOM MYROOMS[10];

	//Declare function prototypes.
		int getRandomNumber(int); 		//	Function to get a Random Number.
		void getRoomNames();			//  Function to assign room names.
		void getRoomType();				// 	Fuction to assign room type
		void initiateConnections();		// 	Function to initiate Room Connections
		void checkConnections();		//	Check to see if all rooms have required connections.
		void connectRooms(int, int);	// 	Function to connect two rooms to each other.
		void getNewDirectory(); 		// 	Function to create a new Directory.
		void generateRoomFiles();		// 	Function to generate room files with room information.


int main(){
	srand(time(NULL));
	getRoomNames();
	getRoomType();
	initiateConnections();
	checkConnections();
	getNewDirectory();
	generateRoomFiles();

}


/************************************************************************************************
 * Function Name:       getRandomNumber															*
 * Description:         Function to get a random number between 0 and max.						*
 ***********************************************************************************************/
int  getRandomNumber(int max){
	int randomNum = rand() % max;
	return randomNum;

}

 /***********************************************************************************************
 * Function Name:       getRoomNames															*
 * Description:         Function to get the 7 random names of the rooms to be used and assign 	*
 *						the each name to a room.												*
 ***********************************************************************************************/
void  getRoomNames(){
	// Declare the names of the rooms to be used in the game
	// Game is set up to use names from a MEdieval Castle.
		char * rooms[10] = {
			"Great Hall",
			"Kitchen",
			"Gardens",
			"Gate House",
			"Bath House",
			"Chapel",
			"Tower",
			"Dungeon",
			"Stables",
			"Courtyard" 
		};

	// Array to determine if name is taken.
		int nameTaken[NUM_ROOM_NAMES];

	// Declare an array to hold the names for all 7 names.	
	   char * roomsUsed[MAX_NUM_ROOMS];	

	//Initialize Array to all 0's or Null Terminators Accordingly.
		memset(nameTaken, 0, sizeof(nameTaken));
		memset(roomsUsed, '\0', sizeof(roomsUsed)); // Initialize to Null Terminators.
	   	memset(MYROOMS[i].Name, '\0', sizeof(MYROOMS[i].Name));
	
	// Randomly choose 7 names to be used in the game.
		for(i=0; i < 7; i++) // Loop through all 7 rooms.
		{
			// Choose Random number 0-9.
				int randNum = getRandomNumber(NUM_ROOM_NAMES);
			// While name is taken then pick another number.
				while (nameTaken[randNum] == 1)
				{
					randNum = getRandomNumber(NUM_ROOM_NAMES);		
				}
			// Indicate Name is Taken
				nameTaken[randNum] = 1;
			// Store Names to Rooms.
				strcpy(MYROOMS[i].Name, rooms[i]);
		}
}

/************************************************************************************************
 * Function Name:       getRoomType																*
 * Description:         Function to randomly assign room type to each room.						*
 ***********************************************************************************************/
void getRoomType(){

	// Pick a Random Number to Determine the Start Room.
		int startRoomIndex = getRandomNumber(MAX_NUM_ROOMS);

	// Pick a Random Number to Determine the End Room.
		int endRoomIndex = getRandomNumber(MAX_NUM_ROOMS);

	// Keep picking an endRoomIndex until it does not equal the start room.
		while (startRoomIndex == endRoomIndex)
			endRoomIndex = getRandomNumber(MAX_NUM_ROOMS);

	// Assign start, mid & end room types
		for (i = 0; i < MAX_NUM_ROOMS; i++)
		{
			if (i == startRoomIndex) 
				MYROOMS[i].RoomType = START_ROOM;
			else if (i == endRoomIndex)
				MYROOMS[i].RoomType = END_ROOM;
			else
				MYROOMS[i].RoomType = MID_ROOM;
		}
}	

/************************************************************************************************
 * Function Name:       createConnections
 * Description:         Function to initiate the MYROOMS Array to begin connecting rooms.
 ************************************************************************************************/
void initiateConnections(){

	// Initialize all Number of Connections to 0.
	// Set all connections to \0.
	int i;
	for (i = 0; i < 7; i++)
		{
			MYROOMS[i].numOfConnections = 0;
			memset(MYROOMS[i].roomConnections, '\0' , sizeof(MYROOMS[i].roomConnections));
		}
}

/************************************************************************************************
 * Function Name:       checkConnections														*
 * Description:         Function to check if minimum connections have been made.  If they do 	*
 *						not have the minimum connections keep making random connections until 	*
 *						all rooms have the minimum number of connections.
 ***********************************************************************************************/
void checkConnections(){

	// Variable to check if all connections are made.
		int allConnectionsMade; // 1 = All Connections Made - 0 = Connections Needed.
	
	// Do until all rooms have atleast 3 connections
	do{
		
			// Reset to 1.
				allConnectionsMade = 1;

			// Loop through all rooms to determine if min connections are made
			for (i = 0; i < MAX_NUM_ROOMS; i++)
			{   
				// If any room has less than min connections then allConnectionsMade = 0.
					if (MYROOMS[i].numOfConnections < MIN_NUM_CONNECTIONS)
						allConnectionsMade = 0;
			}

			// Variables to hold Room Indexes.
				int Num1;
				int Num2;

			// Get A Valid Room with Connections Available.
				do{
					Num1 = getRandomNumber(MAX_NUM_ROOMS);
				}while (MYROOMS[Num1].numOfConnections > (MAX_NUM_CONNECTIONS-1));

				// Get a Second Valid Room With Connections Available
				// Provided it is not the same as Room 1.
					do{
						do{
							Num2 = getRandomNumber(MAX_NUM_ROOMS);
						} while (Num1 == Num2);
					} while ( MYROOMS[Num2].numOfConnections > (MAX_NUM_CONNECTIONS-1));
			// Connect each room to each other.
				connectRooms(Num1, Num2); 
	} while (allConnectionsMade == 0);						
}

/************************************************************************************************
 * Function Name:       connectRooms															*
 * Description:         Function to Connect A room at index NumA to another room at index NumB	*
 *						provided they are not already connected to each other.					*
 ***********************************************************************************************/
void connectRooms(int NumA, int NumB){
	
	// Find how many connections both rooms have to determine
	// the index the new connection will be stored.
		int numConnectionsA = MYROOMS[NumA].numOfConnections;
		int numConnectionsB = MYROOMS[NumB].numOfConnections;

	// Variable to determine if the room is already connected
		int isConnected = 0; //Initialize to 0 = not connected

	// Check to see if Rooms Are Already Connected
		int i;
		for (i = 0; i < 7; i++)
		{   
			// If the index in the roomConnections is the same as the 
			//  second room's index then the rooms are connected.
			if (MYROOMS[NumA].roomConnections[i] == NumB)
				isConnected = 1; // Set to 1 = is connected.
		}

	// If the rooms are not already connected, connect each room (Backward Compatibility)
	if (isConnected ==0)
	{
		// Connect Room A to Room B
			MYROOMS[NumA].roomConnections[numConnectionsA] = NumB;
		// Connect Room B to Room A
			MYROOMS[NumB].roomConnections[numConnectionsB] = NumA;
		
		// Increase Each Room's number of Connections.
			MYROOMS[NumA].numOfConnections++; 
			MYROOMS[NumB].numOfConnections++;
	}
}

/************************************************************************************************
 * Function Name: 	getNewDirectory																* * Description:  	Function to create a new directory with a specific prefix and process ID	*
 ***********************************************************************************************/
void getNewDirectory(){

        // Create a character array to store the directory name.
                char newDirName[256];

        // Get the Process ID
                int pid = getpid();

        // Assign the prefix name for the new directory
                char * dirPrefix = "harbers.rooms.";

        // Fill newDirName with null terminators.
                memset(newDirName, '\0', sizeof(newDirName));

        // Combine the prefix of the name with the pid to obtain the new directory name.
                sprintf(newDirName, "%s%d", dirPrefix, pid);

        // Create the new direectory with correct permissions.
                mkdir(newDirName, 0755);
};

/************************************************************************************************
 * Function Name:       generateRoomFiles														*
 * Description:         Function to generate the files to store the previously created room		*
 *						information according to a specific format.								*
 ***********************************************************************************************/
void generateRoomFiles(){

	//Set directory path name.
		memset(directoryPath, '\0', sizeof(directoryPath)); // Set to all null terminators.
		strcpy(directoryPath, "./harbers.rooms."); // Copy Name of Path into Variable.
		sprintf(directoryPath, "%s%d", directoryPath, getpid());
	
	//Enter into the directory.
		chdir(directoryPath);
	// Create a Room file in the directory for each room.
		FILE * filePointer = NULL;
	
	// Loop through for all Rooms.
	for (i=0; i < MAX_NUM_ROOMS; i++)
	{
		// Variable to hold room Name;
			char roomNames[256];
		// Set roomNames to null terminators
        	memset(roomNames, '\0', sizeof(roomNames));
		
		// Concatenate room name with the id to get the files Names.
			sprintf(roomNames, "Room%d", i);
		
		// Set file pointer to open a file with room name to write.
			filePointer = fopen(roomNames, "w");
		
		// Write Room Name to File.
			fprintf(filePointer, "ROOM NAME: %s\n", MYROOMS[i].Name);

		// For Each Room Connection, write the connection to the file.
			for(a = 0; a < MYROOMS[i].numOfConnections; a++)
			{
					int b = a + 1; // Sets Room Number to Correct Number.
				// Write which connection, and the name of the room connected to the file.
					fprintf(filePointer, "CONNECTION %d: %s\n", b, MYROOMS[MYROOMS[i].roomConnections[a]].Name); 
			}
		// Write Room Type to File
			if (MYROOMS[i].RoomType == 0)
				fprintf(filePointer, "ROOM TYPE: START_ROOM\n");
			else if (MYROOMS[i].RoomType == 1)
				fprintf(filePointer, "ROOM TYPE: MID_ROOM\n");
			else if (MYROOMS[i].RoomType == 2)
				fprintf(filePointer, "ROOM TYPE: END_ROOM\n");
		
		// Close The File
			fclose(filePointer);
	}
	// Return to the Parent Directory
	chdir(".."); 
}