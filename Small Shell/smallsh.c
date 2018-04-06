/*******************************************************************************************************
 * Author:			Sarah Harber
 * Last Update:		8/6/17
 * Description: 	Create a shell in C that will run command line instructions.
 * Specifications:	- Shell will support 3 built in commands: exit, cd, status.
 * 					- Comments will begin with '#'.
 * 					- If it is a background process & will be the last character of the command.
 * 					- A colon is the symbol as a prompt for the command line.
 * 					- Shell will allow for redirection of standard input & standard output
 * 					- Shell will support both foreground and background processes.
 *					- Debug changed to 2 when Working Properly
 ******************************************************************************************************/

// Set Header Files
	#include <signal.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <sys/wait.h>
	#include <sys/types.h>



// Declare Neccesary Variables
	int Debug = 1; 				// Debugging Variable.
	char buffer[2048]; 			// Character Array Buffer
	char *b = buffer; 			// Pointer to Buffer
	size_t bufsize = 512; 		// Size_t of Buffer size
	size_t arguments; 			// Arguments temp holds buffer
	int toexit = 0; 			// Int Variable to exit.
	char * args[512]; 			// Variable to store arguments;
	int numberOfArguments; 		// Variable to store the current number of arguments.
	int status= 0;				// Variable to hold status of foreground process.
	int bgProcess = 0;			// Variable to hold if it is background process.
	pid_t cpid;					// Variable to hold Child pid.
	pid_t ppid;					// Variable to hold Parent pid.
	int i;						// Variable for counting for loops.
	struct sigaction act;
// Declare function definitions
	void getInput(); 			// Display command line & get input.
	void builtInCommands(); 	// Check to see if command is built in & execut if it is.
	void otherCommands(); 		// All other Commands will be handled.

// Declare Signal Handlers


/****************************************************************
 *			  			 MAIN FUNCTION
 ***************************************************************/
 int main()
 {

 	// Set SIG_INT to be Ignored

	act.sa_handler = SIG_IGN;
	sigaction(SIGINT, &act, NULL);

	while(toexit == 0)
	{
		getInput();
		builtInCommands();
	}
	exit(0);
 }

 /******************************************************************************************************
 *				 					Define Functions Declared Above
 ******************************************************************************************************/


/****************************************************************
 * Function Name: 	getInput
 * Description: 	Function to display the command line and
 *					get user input.
 * Specifications:	If user inputs a comment (line that begins
 * 					with '#' or a blank line, the command line
 * 					will reprompt the user for a new command.
 **************************************************************/
void getInput()
{
	// Create do loop to loop while user is inputting a comment or a blank line.
	do{
		// Clear buffer & args
			memset(buffer, '\0', sizeof(buffer));
			memset(args, '\0', sizeof(args));

		// Flush out prompt
			fflush(stdin);
			fflush(stdout);

		// Print Command Line Prompt
			printf(": ");

		// Flush out stdin again.
			fflush(stdin);

		// Get user's input into promptInput
			arguments = getline(&b,&bufsize,stdin);

		// Set Debug Test Prints

	}while(buffer[0] == '\n' || buffer[0] == '#');

	// Split buffer into tokens & store in arg
		char * token; // Set char pointer to hold tokens temporarily.
		token = strtok(buffer, " \n"); // Get First Token (Will not be null)
		numberOfArguments = 0; // Reset number of Arguments to 0.

	// While There are still tokens, store the current token and get the next.
	while(token != NULL && numberOfArguments < 512)
	{
		args[numberOfArguments] = token;
		if(Debug ==2)
			printf("token %d: %s \n", numberOfArguments, args[0]);
		numberOfArguments++;
		token=strtok(NULL, "  \n");
	}

	// Check to see if command is asking for a background process.
	if (strcmp(args[numberOfArguments-1], "&") != 0)	// This is a background process
	{
		bgProcess = 0;      // Make a Background Process.
		if (Debug == 2)     // PRINT FOR DEBUGGING
			printf("This is a foreground Process.  bgProcess = %d\n", bgProcess);
	}
	else    											// This is a foreground process.
	{
		bgProcess=1;
		numberOfArguments = numberOfArguments - 1;
		if (Debug == 2)
			printf("This is a background Process.  bgProcess = %d\n", bgProcess);
	}

}



/***************************************************************
 * Function Name: 	builtInCommands
 * Description: 	Function to handle 3 built in commands.
 * Specifications:	Fuction will handle exit, status and cd.
 * 					Exit will exit the shell. cd will change
 * 					directories.  Without arguments, it will
 * 					change to the HOME environment.  Status
 * 					will print out either the exit status or
 * 					the terminating signal of the last
 * 					foreground process.
 **************************************************************/
void builtInCommands()
{
	// If it is a foregound process wait.
	int errorTest; // Set error test variable

	if(strcmp(args[0], "exit") == 0) // If user entered "exit"
	{
		toexit=1; // Change exit status of toexit to 1.
	}
	else if (strcmp(args[0], "status") == 0)
	{
		if (WIFEXITED(status))
			printf("Exit Status: %d\n", WEXITSTATUS(status));
		else
			printf("Terminating Signmal %d\n", status);

	}
	else if (strcmp(args[0], "cd") == 0)
	{
		// Check to see if there is only one argument
		if (numberOfArguments ==1) // If only one then go to HOME directory.
		{
			errorTest = chdir(getenv("HOME"));

			// Check cd worked.
			if (errorTest == -1)
				perror("chdir");
			if (Debug ==2) // PRINT FOR DEBUGGING
				printf("Changed to HOME directory\n");
		}

		else if (numberOfArguments == 2) // Built in command only takes 2 commands
		{
			// If a second argument then change to path directed to by user.
			errorTest = chdir(getenv(args[1]));

			// Check cd Worked
			if (errorTest == -1)
				perror("chdir");
			if (Debug == 2) // PRINT FOR DEBUGGING
				printf("Changed to %s  directory\n", args[1]);

		}
		else // If more than 2 arguments
			otherCommands(); // Invalid for built in shell move to other commands

	}
	else
		otherCommands();
}


/***************************************************************
 * Function Name: 	otherCommands
 * Description: 	Function to nonBuiltInCommands
 * Specifications:	Fuction will handle exit, status and cd.
 **************************************************************/
void otherCommands()
{
	int positionOfRedirection=0; 		// Variable for holding redirection needed. (Set at 0)
	int inputRN=0;	 					// Variable for input redirection (Set to false)
	int outputRN=0;						// Variable for output redirection (Set to False)
	char * inputFile;
	char * outputFile;


	//Check to see if file redirection is needed.
	for (i = 0; i < numberOfArguments; i++)
	{
		if (strcmp(args[i], "<") == 0) 			// If Input Redirection is Needed.
		{
			positionOfRedirection = i;
			inputRN =1;
			inputFile = args[i+1];
			if(Debug == 2)
				printf("Input redirection needed\n");
		}
		if (strcmp(args[i], ">") == 0)		// If Output Redirection is Needed.
		{
			positionOfRedirection = i;
			outputRN =1;
			outputFile = args[i+1];
			if(Debug == 2)
				printf("Output redirection needed\n");
		}
	}

	//Fork the Process.
	cpid = fork();

	// Make sure fork was sucessful
	if (cpid < 0)									// If fork was not sucessful
	{
		perror("Error while forking");
		exit(1);
	}
	else if (cpid == 0) 							// This is the Child
	{
		// Handle foreground Processes First
		if (bgProcess == 0)
		{	// Set SIG_INT to Default.
			act.sa_handler = SIG_DFL;
			act.sa_flags = 0;
			sigaction(SIGINT, &act, NULL);

		}
		// Handle all File Input Redirection.
		if (inputFile != NULL) 						// If there is an input Files
		{
			// Redirect input file.
			int f1 = open(inputFile, O_RDONLY);		// Open the input file.
			if (f1 == -1)							// If cannot open file.
			{
				status = 1;
				exit(1);
			}
			else 									// If A Valid Files
			{
				int test = dup2(f1, 0); 			// Redirect Files
				if (test == -1)						// If redirection didn't work.
				{
					exit(1);
					status=1;
				}
			}
			close(f1); 								// Close fi
		}

		if(bgProcess ==1 && inputFile == NULL)		// If it is a background Process && inputFile not specified.
		{
			int f1 = open("/dev/null", O_RDONLY); 	// Redirect file to /dev/null

			if (f1 == -1)							// If cannot open file.
			{
				status = 1;
				exit(1);
			}
			else 									// If A Valid Files
			{
				int test = dup2(f1, 0); 			// Redirect Files
				if (test == -1)						// If redirection didn't work.
				{
					exit(1);
					status=1;
				}
			}
			close(f1); 								// Close fi
		}

		if(outputFile != NULL) // If there is a specified output file.
		{
			// Open/Create the output file
			int f1 = open(outputFile, O_WRONLY | O_CREAT|O_TRUNC, 0644);
			if (f1 == -1)							// If cannot open file.
			{	struct sigaction act;
				status = 1;
				exit(1);
			}
			else 									// If A Valid Files
			{
				int test = dup2(f1, 1); 			// Redirect Files
				if (test == -1)						// If redirection didn't work.
				{
					exit(1);
					status=1;
				}
			}
			close(f1); 								// Close fi
		}

		// Execute Command in args[0]
		execvp(args[0], args);

		// If a foreground Process Wait for Process to Complete
		if (bgProcess == 0)
			{
				do{
				waitpid(cpid, &status, 0);
			}while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		else
		{
			printf("Background Pid: %d\n", cpid);
			fflush(stdout);
		}

		// See if any background Processes Have stopped/Terminated with a Signal.
		while(cpid > 0)
		{
			printf("Background pid Completed: %d\n", cpid);
			if (WIFEXITED(status))
				printf("Exit Status: %d\n", status);
			else {
				printf("Terminating Signal: %d\n", status);
			}
			cpid = waitpid(-1, &status, WNOHANG);
		}
	}
}
