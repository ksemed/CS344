#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <limits.h>
#include <string.h>

struct prompt
{
    char* command;
    char* arg;
    char* input;
    char* output;
    char* AND;
};

struct prompt* establishCommand(char* input) {
    
    struct prompt* currPrompt = malloc(sizeof(struct prompt));

    // For use with strtok_r
    char* saveptr;
    // The first token is the Command
    char* token = strtok_r(input, " ", &saveptr);
    currPrompt->command = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currPrompt->command, token);

    if (strtok_r(NULL, " ", &saveptr) != "\n") {
        // The next token is the Arguments
        token = strtok_r(NULL, " ", &saveptr);
        currPrompt->arg = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currPrompt->arg, token);
    }
    if (strtok_r(NULL, " ", &saveptr) != "\n") {
        // The next token is the Input
        token = strtok_r(NULL, " ", &saveptr);
        currPrompt->input = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currPrompt->input, token);
    }
    if (strtok_r(NULL, " ", &saveptr) != "\n") {
        // The next token is the Output
        token = strtok_r(NULL, " ", &saveptr);
        currPrompt->output = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currPrompt->output, token);
    }
    if (strtok_r(NULL, " ", &saveptr) != "\n") {
        // The last token is the &
        token = strtok_r(NULL, "\n", &saveptr);
        currPrompt->AND = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currPrompt->AND, token);
    }
    // Set the next node to NULL in the newly created movie entry

    return currPrompt;
}

size_t readTokens(char*** toks, size_t* tok_num) {
    char input[252];

   
    scanf("%s", &input);

 

    struct prompt* prom = establishCommand(input);

    printf("%s\n", prom->command);
    //printf("%s\n", prom->arg);


}

int
main(int argc, char* argv[])
{
    char** toks = NULL;
    size_t toks_size;
    size_t num_toks;
    char* cwd = NULL;
    char* tmp_cwd;
    char* login;
    char hostname[HOST_NAME_MAX + 1];
    //do
    //{
        { /* Increase cwd buffer size until getcwd is successful */
            size_t len = 0;
            while (1)
            {
                len += 16;
                cwd = realloc(cwd, len * sizeof * cwd);
                if (getcwd(cwd, len) == NULL)
                {
                    if (errno == ERANGE) continue;
                    err(errno, "getcwd failed");
                }
                else break;
            }
        }
        char* homedir = getenv("HOME");

        { /* Replace home directory prefix with ~ */
            size_t len = strlen(homedir);
            tmp_cwd = cwd;
            if (strncmp(tmp_cwd, homedir, len) == 0)
            {
                tmp_cwd += len - 1;
                *tmp_cwd = '~';
            }
        }

        login = getlogin();
        gethostname(hostname, sizeof(hostname));

        /* Print out a simple prompt */
        fprintf(stderr, "%s@%s %s$ ", login, hostname, tmp_cwd);

        /* Call custom tokenizing function */
        num_toks = readTokens(&toks, &toks_size);
        size_t i;
        /*
        if (num_toks > 0)
        {
          if (strcmp(toks[0], "cd")==0)
          {  // cd command -- shell internals
            if (num_toks == 1)
            {
              if(chdir(homedir) == -1)
              {
                perror("Failed to change directory");
              }
            }
            else if (chdir(toks[1]) == -1)
            {
              perror("Failed to change directory");
            }
          }
          else if (strcmp(toks[0], "echo")==0)
          { // echo command! -- shell internals
            if (num_toks > 1) printf("%s", toks[1]);
            for (i=2; i<num_toks; ++i) printf(" %s", toks[i]);
            printf("\n");
          }
          else
          { // Default behavior: fork and exec
            pid_t pid = fork();
            if (pid == 0)
            { // child
              execvp(toks[0], toks);
              err(errno, "failed to exec");
            }
            wait(NULL);
          }
        }
      } while (num_toks > 0);


      size_t i;
      for (i=0; i<toks_size; ++i)
      {
        free(toks[i]);
      }
      free(toks);
      free(cwd);
      */
    }
