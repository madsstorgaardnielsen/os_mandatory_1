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


int main(){
while (1){

	prompt();
	
	char *pipeArgs[100];
	
	int inputFd = STDIN_FILENO;
	 
	//Go through each pipe-arg
	int length = getInputCommands(pipeArgs);
	for (int i = 0; i < length;i++){
		char *currentCommand = pipeArgs[i];
		//Get args 
		char *args[100];
		getArgs(currentCommand,args);	

		//Execute input
		inputFd = executeInput(args,inputFd);
		int status;
		wait(&status);
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
