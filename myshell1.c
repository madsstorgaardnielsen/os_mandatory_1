#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
char input[1000];

char *builtin_str[] = {"cd", "help", "exit"};

int (*builtin_func[])(char **) = {&lsh_cd, &lsh_help, &lsh_exit};

int lsh_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

int lsh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args) {
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int lsh_exit(char **args) { return 0; }

int lsh_launch(char **args) {
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int lsh_execute(char *args) {
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

int parseInput(char *args[]) {
  // Get the command with getline()
  char *line = NULL;            // The chars of the line
  size_t len = 0;               // buffer length
  getline(&line, &len, stdin);  // Reads from stdin

  // Get the rest of the tokens into array args
  char delimit[] = {'|'};
  args[0] = strtok(line, delimit);
  char *p = args[0];
  int i = 1;

  // Get the rest of the args - last element will be NULL
  while (p != NULL) {
    p = strtok(NULL, delimit);
    args[i++] = p;
  }
  return i - 1;
}

void lsh_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    char dir[FILENAME_MAX];
    getcwd(dir, sizeof(dir));  // gets cwd into dir
    printf("%s: ", dir);       // Prints cwd
    fflush(stdout);

    fgets(input, 1000, stdin);

    int t = parseInput(input);

    printf("%s", t);

  } while (status);
}

int main(int argc, char **argv) {
  // Run command loop.
  lsh_loop();

  return EXIT_SUCCESS;
}