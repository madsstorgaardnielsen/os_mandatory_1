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
int executeInput(char *args[], int inputFd);
static void exec_pipeline(char* cmds[], size_t pos, int in_fd, int length);


int main()
{
while (1)
{
	prompt();
	
	char *pipeArgs[100];
	
	//Go through each pipe-arg
	int length = getInputCommands(pipeArgs);

	exec_pipeline(pipeArgs,0,STDIN_FILENO,length);
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

/** move oldfd to newfd */
static void redirect(int oldfd, int newfd) {
	dup2(oldfd, newfd);
	close(oldfd);
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
		redirect(in_fd, STDIN_FILENO); /* read from in_fd, write to STDOUT */
		execvp(args[0],args);
	}
	else { /* $ <in_fd cmds[pos] >fd[1] | <fd[0] cmds[pos+1] ... */
		int fd[2]; /* output pipe */
		pipe(fd);
		switch(fork()) {
		case 0: /* child: execute current command `cmds[pos]` */
			close(fd[0]); /* unused */
			redirect(in_fd, STDIN_FILENO);  /* read from in_fd */
			redirect(fd[1], STDOUT_FILENO); /* write to fd[1] */
			execvp(args[0], args);
		default: /* parent: execute the rest of the commands */
			close(fd[1]); /* unused */
			close(in_fd); /* unused */
			wait(NULL);
			exec_pipeline(cmds, pos + 1, fd[0],length); /* execute the rest */
		}
	}
}

