#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#define MAX_LINE_LENGTH 100
#define MAX_ARGUMENTS 10
#define NUM_OWN_COMMANDS 4

#define clear() printf("\033[H\033[J")

using namespace std;

char* parsed[MAX_ARGUMENTS];     //Array of argmuents
int lastArgument;                //Last Argument
char  line [MAX_LINE_LENGTH];    //Line

bool simple_shell();
bool readLine();
void parseArguments();
void executeArguments();

/* -----------------------------------------------------------------------------
                            Reading User Input
1. fgets gets user input from terminal "stdin" with length of "MAX_LINE_LENGTH"
2. Removing '\n' at the end of the line (When a user press enter)
----------------------------------------------------------------------------- */

bool readLine()
{
    fgets(line,100,stdin);
    line[strlen(line) - 1] = '\0';
    return (strlen(line) > 0 && line != NULL) ? true : false;
}

/* -----------------------------------------------------------------------------
                            Parsing User Input
    Using strtotk function to split arguments by space delimiter and save the
    very last arguemnt to check further if it contains '&' or not.
----------------------------------------------------------------------------- */

void parseArguments()
{
    parsed[0] = strtok (line," ");
    int i = 0;
    while(parsed[i]!=NULL)
        parsed[++i]= strtok (NULL, " ");
    lastArgument = i - 1;
}

/* -----------------------------------------------------------------------------
            Execute commands can't be handled by execvp e.x (cd exit)
    Handling exit, cd, help, and support hello command, saving user's choice and
    executing those commands manually.
----------------------------------------------------------------------------- */

bool handleCommands()
{
    int choice = 0;
    char* ListOfOwnCmds[NUM_OWN_COMMANDS] = {"exit","cd","help","hello"};
    char* username;

    for (int i = 0; i < NUM_OWN_COMMANDS; i++)
    {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0){
            choice = i + 1;
            break;
        }
    }

    switch (choice)
    {
    case 1:
        puts("\nGoodbye\n");
        exit(0);
    case 2:
        chdir(parsed[1]);
        return true;
    case 3:
        puts("\n***WELCOME TO MY SHELL HELP***"
         "\nList of Commands supported:"
         "\n>cd"
         "\n>ls"
         "\n>exit"
         "\n>all other general commands available in UNIX shell");
        return true;
    case 4:
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
               "not a place to play around."
               "\nUse help to know more..\n",
               username);
        return true;
    default:
        return false;
    }
    return false;
}

/* -----------------------------------------------------------------------------
                Execute commands can be handled by execvp
    Our command can be followed by '&' or not, if it's followd by '&' we should
    Handle this case, the parent should wait iff 'found_amp' == false else,
    parent must continue normally.
----------------------------------------------------------------------------- */

void executeArguments()
{
    pid_t pid;
    const char *amp;
    amp = "&";
    bool found_amp = false;

    /* Getting value of the last character in last argument and equal it to '\0' */
   if (*(*(parsed + lastArgument)+(strlen(parsed[lastArgument]) - 1)) == '&')
    {
        found_amp = true;
      *(*(parsed + lastArgument)+(strlen(parsed[lastArgument]) - 1)) = '\0';
    }

    /* Fork our process */
    pid = fork();

    if (pid < 0)
        perror("Error (pid < 0)");

    /* child process */
    else if (pid == 0)
        if (execvp(*(parsed), parsed) < 0)
            perror("execvp error");

    /* Only wait if no ampersand */
    found_amp == true ? simple_shell() : waitpid(pid, NULL, 0);
}

/* -----------------------------------------------------------------------------
                        S I M P L E     S H E L L
    You can simply create a shell by readLine(), the parseArguments() and
    execute commands if it's not handled by execvp default handler.
----------------------------------------------------------------------------- */
bool simple_shell()
{
    if(readLine())
        parseArguments();

    if (!handleCommands())
        executeArguments();

    memset(&line[0], 0, sizeof(line));
    return true;
}

int main(void)
{
    while(true)
        simple_shell();
    return 0;
}
