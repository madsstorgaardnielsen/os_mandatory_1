#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>

/**
 Prints the cwd to the user
*/
void prompt()
{
	char dir[FILENAME_MAX];
	getcwd(dir, sizeof(dir)); //gets cwd into dir
	printf("%s: ", dir);	  //Prints cwd
	fflush(stdout);
}

/**
Reads a line from the console, splitting it by the | symbol 
The tokens are saved into args[]
Returns the number of args there were
*/
int getPipeArgs(char *args[])
{
	//Get the command with getline()
	char *line = NULL; //The chars of the line
	size_t len = 0;	   //buffer length
	if (getline(&line, &len, stdin) == -1)
	{
		perror("Input error");
	}

	//Get the rest of the tokens into array args
	char delimit[] = {'|'};
	args[0] = strtok(line, delimit);
	char *p = args[0];
	int i = 1;

	//Get the rest of the args - last element will be NULL
	while (p != NULL)
	{
		p = strtok(NULL, delimit);
		args[i++] = p;
	}
	return i - 1;
}

/**
 Splits the input by space into substrings in args
 args is a null-terminated array, that can be used by execvp()
 */
void getArgs(char *input, char *args[])
{
	//Get the first token into array args
	char delimit[] = {' ', '\n'};
	args[0] = strtok(input, delimit);
	char *p = args[0];
	int i = 1;

	//Get the rest of the args - last element will be NULL
	while (p != NULL)
	{
		p = strtok(NULL, delimit);
		args[i++] = p;
	}
}

/**
Close filedescriptor handling errors
*/
void Close(int fd)
{
	if (close(fd) == -1)
	{
		perror("close");
		fprintf(stderr, "couldnt close %d\n", fd);
	}
}

/** 
 Copies one filedescriptor "oldfd" into a another, "newfd" 
 Old one is closed
*/
static void redirect(int oldfd, int newfd)
{
	//Dup2 overwrites newfd with oldfd
	if (dup2(oldfd, newfd) == -1)
	{
		perror("redirect");
		fprintf(stderr, "couldnt redirect %d to %d\n", oldfd, newfd);
	};
	//Make sure to close the old fd now, that it is no longer used
	Close(oldfd);
}

/**
Executes a command specified in args[0] with parameters given in the following args
Accesses the PATH variable, so commands like "ls" can be used
*/
void executeInput(char *args[])
{
	int childPid = fork();

	if (childPid == -1) 
	{
		printf("Cant fork process");
	}

	int isChild = childPid == 0;
	if (isChild)
	{
		//Execute command in args[0] with parameters from args
		//Acessesing PATH variable
		//Child procces image is changed completely, so it will not continue this code
		//if successful
		int status = execvp(args[0], args);
		if (status < 0)
		{
			//Prints the last error that was encountered
			perror(args[0]);
		}
		//If an error was encountered, the child process should die
		exit(0);
	}
	//parent proces waits for child to finish before it returns
	wait(NULL);
}

void handleCd(char *args[]){
	char buffer[1024];
	char *path;
	char *dir;
	char *nextDir;
	path = getcwd(buffer, sizeof(buffer)); //get the current path
	dir = strcat(path, "/");			   // add a forward slash to the end of the current path
	nextDir = strcat(dir, args[1]);  	   //add the directory you want to navigato to
	chdir(nextDir);						   // cd into the path builded above
}

void handleHelp()
{
	printf("\nAvailable commands: ls, cd, grep, piping (|).\n\n");
	printf("\nls prints a list of items in the current directory, ls also accepts arguements");
	printf("\ne.g ls | grep argument");

	printf("\n\ncd changes directory");
	printf("\ne.g mkdir abc ENTER");
	printf("\ncd abc");
	printf("\nwould return a list with items where the filename contains the grep argument");
	
	printf("\n\nPiping is implemented recursively, which means you can chain pipes n times");
	printf("\ne.g arg1 | arg2 | arg3 | ... | argn\n\n");
}

/**
Recursive function to execute a pipeline
cmds contains all the commands that should be executed
pos indicates which command we are executing this time
in_fd is the file-descripter where cmds[pos] should read it's input from
length is simply the length of cmds
*/
static void exec_pipeline(char *cmds[], int pos, int in_fd, int length)
{
	char *currentCommand = cmds[pos]; //Find the command that we are about to execute
	char *args[100];
	getArgs(currentCommand, args); //Split the command into it's arguments

	//Handle special commands - these have nothing to do with piping
	if (strcmp(currentCommand, "cd") == 0)
	{
		handleCd(args);
		return;
	}
	else if (strcmp(currentCommand, "help") == 0)
	{
		handleHelp();
		return;
	} else if (strcmp(currentCommand, "exit") == 0)
	{
		printf("Exiting shell..\n");
		exit(0);
	}

	//If it is the last command in the pipe 
	int isLast = pos == length - 1; 
	if (isLast)
	{
		//The last command should read it's input form in_fd
		//Output fd should not be changed (should be stdout)
		//Fork so we don't overwrite stdin in parent process
		if (fork() == 0)
		{
			//Child executes command and writes output to stdout
			redirect(in_fd, STDIN_FILENO);
			executeInput(args);
			exit(0);
		}
		//Parent waits on child and returns to main loop
		wait(NULL);
	}
	else //If it is not the last command in the pipe
	{			   
		int fd[2]; //alloc space for two ends of a pipe
		if (pipe(fd) < 0) //Init fd with actual pipe ends
		{
			printf("ERROR");
		};

		//Fork now so child can execute the command
		if (fork()==0)
		{
			//child closes one end of the pipe, reads from in_fd and writes to other end
			Close(fd[0]);					//close unused end
			redirect(in_fd, STDIN_FILENO);	//Overwrite stdin with gived in_fd
			redirect(fd[1], STDOUT_FILENO); //write to other end of pipe
			execvp(args[0], args); //execute and terminate
		} else {
			//Parent closes one end of the pipe (opposite of child)
			//And recursively calls this function, where the in_fd is set to the 
			//end of the pipe, where child's output can be read
			Close(fd[1]);								 
			exec_pipeline(cmds, pos + 1, fd[0], length); 
			wait(NULL); //Waits for child to terminate
		}
	}
}



int main()
{
	while (1)
	{
		//Prints prompt to user
		prompt();

		//Allocate space for user input
		char *pipeArgs[100]; 

		//Read input from user and save it separated by | in pipeArgs
		int length = getPipeArgs(pipeArgs);

		if (length > 0) //If there is some input - handle it
		{
			//Recursively exectues pipe commands, piping input of one command to next
			exec_pipeline(pipeArgs, 0, STDIN_FILENO, length);
		}
	}

	return 0;
}
