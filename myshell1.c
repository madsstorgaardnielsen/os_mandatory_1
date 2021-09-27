
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static char lineInput[1024];

static char *args1[512];
static char *args2[512];

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

    char delimiter[] = "|";
    char *command = strtok(lineInput, delimiter);
    int index1 = 0;
    args1[index1] = command;

    if (args1[0] != NULL && strcmp(args1[0], "exit") == 0) {
      exit(0);
    }

    printf("%s\n",  args1[index1]);
  }
}