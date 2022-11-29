#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int foreground = 0;
int confirm = 0;
int pidstorelength = 0;
int pidstart = 0;

void handle_sigtstp(int sig) { // Reference: https://www.youtube.com/watch?v=jF-1eFhyz1U&t=365s&ab_channel=CodeVault
	
	if (confirm == 0) // Fore-Ground Mode
	{
		printf(" Entering Fore-Ground Only Mode\n");
		foreground = 1;
		confirm = 1;
	}
	else if (confirm == 1) // Exiting Fore-Groud Mode
	{
		printf(" Exiting Fore-Ground Only Mode\n");
		foreground = 0;
		confirm = 0;
	}

}

// Used to utilize signal in order to cancel CTRL C 
void handler (int sig) {
	

}

// Prints Status
void printStatus(int status) {
	
	if (WIFEXITED(status)) {
		printf("Exit Value: %d\n", WEXITSTATUS(status));
	}
	else {
		printf("Terminated Signal: %d\n", WTERMSIG(status));
	}
	
}

int
main(int argc, char* argv[])
{
	// To deal with CTRL Z		Reference: https://www.youtube.com/watch?v=jF-1eFhyz1U&t=365s&ab_channel=CodeVault
	struct sigaction sa;
	sa.sa_handler = &handle_sigtstp;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &sa, NULL);

	// To deal with CTRL C		Reference: Used Discord
	signal(SIGINT, handler);

	char input[2048]; // Reperesents stdin input
	char* arguments[512]; // Where we store our arguments, treated as individual strings
	int status = 0;

	while (1 == 1)
	{

		printf(": ");
		fflush(stdout);
		fgets(input, 2048, stdin);

		char* saveptr;

		// Removing New Line for Inputs
		for (int i = 0; i < 2048; i++)
		{
			if (input[i] == '\n')
			{
				input[i] = '\0';

			}
		}

		// Resetting Array for future use
		for (int i = 0; i < 512; i++)
		{
			arguments[i] = '\0';
		}

		// Getting array of strings through parsing input
		char* token = strtok_r(input, " ", &saveptr);

		int arraylength = 0;
		while (token != NULL)
		{
			arguments[arraylength] = token; // Store component into arguments
			arraylength++;

			token = strtok_r(NULL, " ", &saveptr);
		}

		// To skip comments and whatnot
		if (arguments[0][0] == '#' || arguments[0][0] == '\0')
		{
			continue;
		}

		// Variable Expansion
		int pid = getpid(); 

		/*
		for (int i = 0; i < arraylength; i++)
		{
			int len = strlen(arguments[i]) - 1;
			char* formatString = strdup(arguments[i]);
			int newLen = len + 12;
			char* pidString = malloc(newLen);

			for (int j = 0; arguments[i][j]; j++)
			{
				if (arguments[i][j] == '$' && arguments[i][j + 1] == '$')
				{
					arguments[i][j] = '%';
					arguments[i][j + 1] = 'd';
					snprintf(pidString, newLen - 1, formatString, pid);
					break;
				}
			}
			
			snprintf(pidString, newLen - 1, formatString, pid);
			printf("PidString: %s\n", pidString);
		}
		*/
		for (int i = 0; i < arraylength; i++)
		{
			int len = strlen(arguments[i]) - 1;
			char* formatString = strdup(arguments[i]);
			int newLen = len + 12;
			char* pidString = malloc(newLen);
			for (int i = 0; i < len; i++)
			{
				if (formatString[i] == '$' && formatString[i + 1] == '$') {
					formatString[i] = '%';
					formatString[i + 1] = 'd';
				}
			}
			snprintf(pidString, newLen - 1, formatString, pid);
			arguments[i] = pidString;
		}
		
		// Dealing with Built In Commands

		int confirmbuiltin = 0;

		char* command;

		if (arraylength > 0)
		{
			command = arguments[0]; // The Command is going to be the first string
		}
		else if (arraylength == 0)
		{
			command = "N/A";
		}

		if(strcmp(command, "exit") == 0)
		{
			return 0;
		}
		else if (strcmp(command, "cd") == 0)
		{
			confirmbuiltin++;
			if (arraylength == 1) // If there's no argument after cd
			{
				chdir(getenv("HOME"));
			}
			else // If there is an argument after cd
			{
				if (chdir(arguments[1]) == -1) // Making sure if we can find file
				{
					printf("Directory cannot be located\n");
					fflush(stdout);
				}
				else // If we can find file
				{
					chdir(getenv(arguments[1]));
				}
			}
		}
		else if (strcmp(command, "status") == 0)
		{
			confirmbuiltin++;
			printStatus(status);

		}
	
		if (confirmbuiltin == 0) {

			// Dealing with Non - Built In Commands

			int background = 0; // Used to indicate whether we're sending to background or not

			for (int i = 0; i < arraylength; i++)
			{
				if (i == arraylength - 1 && strcmp(arguments[i], "&") == 0)
				{
					if (foreground == 0)
					{
						background = 1; // We are sending to the background
					}
					arguments[i] = '\0'; // To get rid off the & for the command to work
					arraylength--;
				}
			}

			int pidc;
			int pidstore[500];

			int id = fork();

			if (id == 0) // Child Process
			{

				for (int i = 0; i < arraylength; i++)
				{
					// Input and Output Redirection
					if (strcmp(arguments[i], "<") == 0)
					{
						char inputtext[100];
						char* file = arguments[i + 1]; // Determines input file
						arguments[i] = '\0'; // Getting rid of the <
						int fp = open(file, O_RDONLY); // Opens file
						if (fp != -1)
						{
							dup2(fp, 0);
							fflush(stdout);
							close(fp);
						}
						else if (fp == -1)
						{
							printf("File not available\n");
							fflush(stdout);
							exit(1);
						}
					}
					else if (strcmp(arguments[i], ">") == 0)
					{
						char outputtext[100];
						char* thefile = arguments[i + 1]; // Determines output file
						arguments[i] = '\0'; // Getting rid of the >
						int fp = open(thefile, O_WRONLY | O_CREAT | O_TRUNC, 0666); // Opens file
						dup2(fp, 1);
						close(fp);
					}

				}

				int error = execvp(arguments[0], arguments);

				if (error == -1) // If there is an error
				{
					printf("Unable to Commit Execution\n");
					exit(1);
				}

			}
			else {

				if (background == 0)
				{
					waitpid(id, &status, 0); // Wait like normal if we're not doing anything in the background
				}
				else if (background == 1) // Background is activated
				{
					pidc = id;
					printf("Background Pid is: %d\n", pidc);
					pidstore[pidstorelength] = pidc; // Storing in Static Array
					pidstorelength++;
					//waitpid(id, &status, WNOHANG);
					fflush(stdout);

				}
				while (id = waitpid(-1, &status, WNOHANG | WUNTRACED) > 0) { // To print when child dies
					pidc = pidstore[pidstart];
					pidstore[pidstart] = '\0';
					pidstart++;
					printf("Child %d is done\n", pidc);
					fflush(stdout);
					printStatus(status);

				}

			}

		}

	}


}

	
