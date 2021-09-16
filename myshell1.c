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
int getInput(char *args[]);
void handleCd();
void executeInput(char *args[]);


int main(){
while (1){

	prompt();
	
	char *pipeArgs[100];
	getInput(pipeArgs);
	
	//Go through each pipie-arg
	int length = sizeof(pipeArgs)/sizeof(pipeArgs[0]);
	for (int i = 0; i < length;i++){
		char *currentCommand = pipeArgs[i];
		printf("%s\n",currentCommand);
	
		//Get args 
		char *args[100];
		getArgs(currentCommand,args);	
		//Execute input
		executeInput(args);
	}

}	
	
	
	
	
return 0;	
}


int getInput(char *args[]){
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
    return 1;
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

void executeInput(char *args[]){
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