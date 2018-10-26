#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fstream>
#include <iostream>


#define MAX_LINE_LENGTH 100
#define MAX_ARGUMENTS 10
#define NUM_OWN_COMMANDS 4

#define clear() printf("\033[H\033[J")

using namespace std;

fstream log;

char* parsed[MAX_ARGUMENTS];     //Array of argmuents
int lastArgument;                //Last Argument
char  line [MAX_LINE_LENGTH];    //Line

bool readLine();
void parseArguments();
bool handleCommands();
bool executeArguments();
void signalHandler(int sigNum);
bool simple_shell();

/* -----------------------------------------------------------------------------
                            Reading User Input
1. fgets gets user input from terminal "stdin" with length of "MAX_LINE_LENGTH"
2. Removing '\n' at the end of the line (When a user press enter)
----------------------------------------------------------------------------- */

bool readLine()
{   printf("semsem@shell™ -> ");
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
        log << line << " executed successfully" << endl;
        exit(0);
    case 2:
        chdir(parsed[1]);
        log << line << " executed successfully" << endl;
        return true;
    case 3:
        puts("\n***WELCOME TO MY SHELL HELP***"
         "\nList of Commands supported:"
         "\n>cd"
         "\n>ls"
         "\n>exit"
         "\n>all other general commands available in UNIX shell");
          log << line << " executed successfully" << endl;
        return true;
    case 4:
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
               "not a place to play around."
               "\nUse help to know more..\n",
               username);
        log << line << " executed successfully" << endl;
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

bool executeArguments()
{
    pid_t pid;
    const char* amp = "&";
    bool found_amp = false;

    /* Getting value of last argument */
   if (strcasecmp((*(parsed + lastArgument)),amp) == 0)
        found_amp = true;

    /* Fork our process */
    pid = fork();

    if (pid < 0)
        perror("Error (pid < 0)");

    /* child process */
    else if (pid == 0)
        if (execvp(*(parsed), parsed) < 0){
            perror("execvp error");
            log << line << " error occured" << endl;
            return false;
        }

    /* Only wait if no ampersand */
    if(found_amp)
        signal(SIGCHLD, signalHandler);
    else
        waitpid(pid, NULL, 0);

    return true;
}

void signalHandler(int sigNum){
    log << "Child terminated" << endl;
}

/* -----------------------------------------------------------------------------
                        S I M P L E     S H E L L
    You can simply create a shell by readLine(), the parseArguments() and
    execute commands if it's not handled by execvp default handler, log file
    whill help youu to keep track of what actions has been executed/terminated.
----------------------------------------------------------------------------- */
bool simple_shell()
{
    log.open("log.txt",log.out | log.app);

    if(readLine())
        parseArguments();

    if (!handleCommands())
        if(executeArguments())
            log << line << " executed successfully" << endl;

    log.close();
    memset(&line[0], 0, sizeof(line));

    return true;
}

int main(void)
{
    while(true)
        simple_shell();
    return 0;
}
