#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static char userInput[1024];

static char *arg1[512];
static char *arg2[512];
void executeCommand(pid_t pid, int pipefileDesc[2], int isPipe);
void getDir();

int main(int argc, char *argv[]) {
  while (1) {
    getDir();
    pid_t pid;
    int pipefileDesc[2];

    pipe(pipefileDesc);

    if (fgets(userInput, 1024, stdin) == NULL) {
      return 0;
    }

    char delimit[] = "\n ";
    char *command = strtok(userInput, delimit);
    int argIndex1 = 0;
    arg1[argIndex1] = command;

    while (command != NULL) {
      command = strtok(NULL, delimit);
      argIndex1++;
      arg1[argIndex1] = command;
    }

    if (arg1[0] != NULL && strcmp(arg1[0], "exit") == 0) {
      exit(0);
    }

    arg1[argIndex1] = NULL;

    int argIndex2 = 0;
    int isPipe = 0;
    int x = 0;
    for (; argIndex2 < argIndex1; argIndex2++) {
      if (isPipe){
        arg2[x] = arg1[argIndex2];
        x++;
      }
      if (strcmp(arg1[argIndex2], "|") == 0){
        arg1[argIndex2] = NULL;
        isPipe = 1;
      }
    }

    pid = fork();

    executeCommand(pid, pipefileDesc, isPipe);
  }
}

void getDir(){
  char dir[FILENAME_MAX];
    getcwd(dir, sizeof(dir));  // gets cwd into dir
    printf("%s: ", dir);       // Prints cwd
    fflush(stdout);
}

void executeCommand(pid_t pid, int pipefileDesc[2], int isPipe) {
  if (pid < 0) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {
    if (isPipe) {
      dup2(pipefileDesc[1], STDOUT_FILENO);
      close(pipefileDesc[0]);
    }
    execvp(arg1[0], arg1);

    printf("execvp has not been executed\n");
  } else {
    if (!isPipe) {
      usleep(10000);
      pid = wait(NULL);
    }

    if (isPipe) {
      pid = fork();
      if (pid == 0) {
        dup2(pipefileDesc[0], STDIN_FILENO);
        close(pipefileDesc[1]);

        execvp(arg2[0], arg2);
      } else {
        usleep(10000);
        pid = wait(NULL);
      }
    }
  }

  close(pipefileDesc[0]);
  close(pipefileDesc[1]);
}
