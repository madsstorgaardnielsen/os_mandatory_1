
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static char lineInput[1024];

static char *args1[512];
static char *args2[512];
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


int main(int argc, char *argv[]) {
  while (1) {
    char dir[FILENAME_MAX];
    getcwd(dir, sizeof(dir));
    printf("%s: ", dir);

    pid_t pid;
    int pipefd[2];

    pipe(pipefd);

    if (fgets(lineInput, 1024, stdin) == NULL) {
      return 0;
    }

    if (args1[0] != NULL && strcmp(args1[0], "exit") == 0) {
      exit(0);
    }


  }
}

