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
static void exec_pipeline(char* cmds[], size_t pos, int in_fd, int length);


void executeInput(char *args[]){
	int childPid = fork();
	int isChild = childPid == 0;
	if (isChild){
		execvp(args[0],args);
		int status = execvp(args[0], args); 
		if (status <0){
		    perror(args[0]);
		} 
		exit(0);
	} 
	int status;
	wait(&status);
}

void Close(int FD){
	int Close_fd = (FD);                                
    if (close(Close_fd) == -1) {                        
      perror("close");                                  
      fprintf(stderr, "couldnt close %d\n",Close_fd);     
    }    
} 


int main()
{
while (1)
{
	prompt();
	fflush(stdin);
	
	char *pipeArgs[100];
	
	//Go through each pipe-arg
	int length = getInputCommands(pipeArgs);

	if (length > 0){
		exec_pipeline(pipeArgs,0,STDIN_FILENO,length);
	}

}	
	
return 0;	
}


int getInputCommands(char *args[]){
    //Get the command with getline()
    char *line = NULL;//The chars of the line
    size_t len =0;//buffer length
    if (getline(&line,&len,stdin)==-1){
		perror("Input error");
	}

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



/** move oldfd to newfd */
static void redirect(int oldfd, int newfd) {
	if (dup2(oldfd, newfd)==-1){
		perror("redirect");                                  
      	fprintf(stderr, "couldnt redirect %d to %d\n",oldfd, newfd);   
	};
	Close(oldfd);

}

/** execute `cmds[pos]` command and call itself for the rest of the commands.
    `cmds[]` is NULL-terminate array
    `exec_pipeline()` never returns.
*/
static void exec_pipeline(char* cmds[], size_t pos, int in_fd, int length) {

	int isLast = pos == length-1;

	char *currentCommand = cmds[pos];		
	char *args[100];
	getArgs(currentCommand,args);	

	if (isLast) { 
		if (fork()==0){
			redirect(in_fd, STDIN_FILENO); /* read from in_fd, write to STDOUT */
			executeInput(args);
			exit(0);
		} else {
			wait(NULL);
		}
	}
	else { /* $ <in_fd cmds[pos] >fd[1] | <fd[0] cmds[pos+1] ... */
		int fd[2]; /* output pipe */
		if (pipe(fd)<0){
			printf("ERROR");
		};
		switch(fork()) {
		case 0: /* child: execute current command `cmds[pos]` */
			Close(fd[0]); /* unused */
			redirect(in_fd, STDIN_FILENO);  /* read from in_fd */
			redirect(fd[1], STDOUT_FILENO); /* write to fd[1] */
			execvp(args[0], args);
		default: /* parent: execute the rest of the commands */
			Close(fd[1]); /* unused */
			exec_pipeline(cmds, pos + 1, fd[0],length); /* execute the rest */
			wait(NULL);
		}
	}
}

