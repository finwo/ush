#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "linenoise.h"

char **tokenize(char *input) {
  char **command = NULL;
  char *separator = " ";
  char *parsed;
  int index = 0;

  parsed = strtok(input, separator);
  while (parsed != NULL) {
    command = realloc(command, (index+2) * sizeof(char*));
    command[index] = parsed;
    index++;
    parsed = strtok(NULL, separator);
  }

  command[index] = NULL;
  return command;
}

int builtin_cd(char *path) {
  return chdir(path);
}

void sigint_handler(int signo) {
  printf("Caught SIGINT\n");
}

int main() {
  char *line;
  char *ps1 = "> ";
  char *histfile = "history.txt";
  char **command;
  pid_t child_pid;
  int stat_loc;

  linenoiseHistoryLoad(histfile); /* Load the history at startup */

  signal(SIGINT, sigint_handler);
  while((line = linenoise(ps1)) != NULL) {

    linenoiseHistoryLoad(histfile); /* Load the history at startup */
    linenoiseHistoryAdd(line); /* Add to the history. */
    linenoiseHistorySave(histfile); /* Save the history on disk. */

    command = tokenize(line);

    if (strcmp(command[0], "cd") == 0) {
      if (builtin_cd(command[1]) < 0) {
        perror(command[1]);
      }
      continue;
    }

    if (strcmp(command[0], "clear") == 0) {
      linenoiseClearScreen();
      continue;
    }

    if (strcmp(command[0], "exit") == 0) {
      if (command[1]) {
        exit(atoi(command[1]));
      } else {
        exit(0);
      }
    }

    child_pid = fork();
    if (child_pid < 0) {
      perror("Fork failed");
      exit(1);
    } else if (child_pid == 0) {
      if (execvp(command[0], command) < 0) {
        perror(command[0]);
        exit(1);
      }
    } else {
      waitpid(child_pid, &stat_loc, WUNTRACED);
    }

    free(line);
    free(command);
  }

  return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
