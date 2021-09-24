#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

//Extra functionality that is nice to have:
//Break instantly on empty line
//Cd + case sensitivity

//NEED to have:
//USe PATH variables
//Pipe

//TODO
//1) Lav Git repo
//Lav Pipe funktionalitet
//Brug PATH variables
//finjuster og går koden læselig + kommenter resourser osv.

void prompt();
int getArgs(char *input, char *args[]);
int getInputCommands(char *args[]);
void handleCd();
int executeInput(char *args[], int inputFd);


int main()
{
while (1)
{

	prompt();
	
	char *pipeArgs[100];
	
	//Go through each pipe-arg
	int length = getInputCommands(pipeArgs);

	for (int i = 0; i < length;i++)
	{
		int isFirst = i ==0;
		int isLast = i == length-1;
		char *currentCommand = pipeArgs[i];
		//Get args 
		char *args[100];
		getArgs(currentCommand,args);	

		//Execute input
		int pipefd[2];
		pipe(pipefd);

		int pipefd2[2];
		pipe(pipefd2); 

		pid_t pid = fork();
		int isChild = pid == 0;
		
		if (isChild){
			if (!isLast){
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}

			if (!isFirst){
				close(pipefd[1]);
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[0]);
			}

			int status = execvp(args[0], args); 
			if (status <0){
				perror(args[0]);
			} 
			return 0;
		}
		wait(NULL);
	}
}	
	
return 0;	
}


int getInputCommands(char *args[]){
    //Get the command with getline()
    char *line = NULL;//The chars of the line
    size_t len =0;//buffer length
    getline(&line,&len,stdin);//Reads from stdin

        //Get the rest of the tokens into array args  
    char delimit[] = {'|'};
    args[0] = strtok(line,delimit);
    char *p =args[0];
    int i = 1;

    //Get the rest of the args - last element will be NULL
    while (p != NULL){
    	p = strtok(NULL,delimit);
	    args[i++] = p;
    }
    return i-1;
}



int getArgs(char *input, char *args[]){
    //Get the rest of the tokens into array args  
    char delimit[] = {' ','\n'};
    args[0] = strtok(input,delimit);
    char *p =args[0];
    int i = 1;

    //Get the rest of the args - last element will be NULL
    while (p != NULL){
    	p = strtok(NULL,delimit);
	    args[i++] = p;
    }
    return 1;
}


void prompt(){
	char dir[FILENAME_MAX];
    	getcwd(dir,sizeof(dir));//gets cwd into dir
    	printf("%s: ",dir);//Prints cwd
    	fflush(stdout);

}


int executeInput(char *args[], int inputFd){
	int pipefd[2];
	pipe(pipefd);
	dup2(inputFd,pipefd[0]);

	int childPid = fork();
	int isChild = childPid == 0;
	if (isChild){
		int isCd = strcmp("cd",args[0]) == 0;
		if (isCd){
			handleCd();
			return;
		}
	
		execvp(args[0],args);
		int status = execvp(args[0], args); 
		if (status <0){
		    perror(args[0]);
		} 
		return;
	} 
	int status;
	wait(&status);


}


void handleCd(){

}
/*

else if (length==2)
	{
		
		int pipefd[2];
		pipe(pipefd);

		pid_t p1, p2;
		p1 = fork();
		int isChild = p1 == 0;
		if (isChild){

			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);

			char *x[] = {"ls", NULL};
			int status = execvp("ls", x); 
			if (status <0){
				perror(pipeArgs[0][0]);
			} 
			return;
		} else {
			p2 = fork();

			// Child 2 executing..
			// It only needs to read at the read end
			if (p2 == 0) {
				close(pipefd[1]);
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[0]);
				char *x[] = {"grep", "s", NULL};
				if (execvp("grep", x) < 0) {
					printf("\nCould not execute command 2..");
					exit(0);
				}
			} 
		} 

		//parent executing, waiting for two children
		wait(NULL);
		wait(NULL);


	}
*/