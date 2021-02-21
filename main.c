// Headers we need
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "header.h"

// Define pointers
char *tempCommandPtr = tempCommand;
char *argsPtr = args;
char *cwdPtr = cwd;

// Create a variable for the current open file
FILE *fp;

// Create a variable to contain a pid for our forks
pid_t pid;

int main(){

    while (ready){
        /*--Clean up for the next command--*/

        // Clear any garbage data from the argument strings on first run
        // Otherwise, clear the previous argument from memory
        memset(argsString, 0, sizeof(argsString));
        memset(args, 0, SIZE);
        memset(argsStringFix, 0, 3);
        memset(fileString, 0, sizeof(fileString));
        memset(command, 0, SIZE);

        // Clear any flags and counters
        argNum = 0;
        fileIndex = 0;
        fileNum = 0;
        argIndex = 0;
        m = 0;
        n = 0;
        hasIn = false;
        hasOut = false;
        hasArgs = false;
        oneArg = false;
        err = false;
        
        /* ---END Clean Up--- */

        // Print to the screen the user can input something
        printf("Shell ready: ");

        // Scan for user input
        scanf(" %[^\n]", command);

        // Find the beginning of the command
        for(i = 0; i < SIZE; i++){
            if (*(commandPtr + i) == ' '){
                tempCommand[i] = '\0';
                i++;
                break;
            }
            tempCommand[i] = *(commandPtr + i);
        }

        // Point the first index of argsString to the address of the index of args
        //  we left off from in the first for loop
        argsString[0] = &args[0];
        
        // Seperate and store and arguments
        for ( j = i; j < SIZE; j++){
            char c = command[j];
            if (c == ' '){
                args[argIndex] = '\0';
                argsString[++argNum] = args + argIndex + 1;
                argIndex++;
            }
            else if (c == '\0'){
                break;
            }
            else {
                args[argIndex] = c;
                argIndex++;
                hasArgs = true;
            }
        }

        // Look for "<" and ">", then set the appropriate flags
        for ( l = 0; l < argNum; l++){
            // If we find ">", open the file specified in the next arg
            if (strcmp(argsString[l], ">") == 0){
                hasOut = true;
                if (l == 0){
                    oneArg = true;
                }
                outIndex = l;
                fp = freopen(argsString[l + 1], "w+", stdout);
                break;
            }
            // Otherwise, if we find "<", open the file specified in the next arg
            else if (strcmp(argsString[l], "<") == 0){
                hasIn = true;
                inIndex = l;
                fp = fopen(argsString[l + 1], "r");
                // If something goes wrong with opening the file, report it to the user
                if (fp == NULL){
                    perror("Failed to open file");
                    err = true;
                    break;
                } 

                fileString[0] = fileRead;
                
                do {
                    // Store each character of data as a char in our array
                    charIn = fgetc(fp);
                    if (charIn == ' ' || charIn == '\n'){
                        fileRead[m] = '\0';
                        fileString[++fileIndex] = fileRead + m + 1;
                        fileNum++;
                    }
                    else{
                        fileRead[m] = charIn;
                    }
                    if( feof(fp) ) {
                        fileRead[m] = '\0';
                        break ;
                    }
                    m++;
                } while(ready);
                break;
            }
        }
       
        if(!err){
            // If the command was "cd", run cd
            if (strcmp(tempCommand, "cd") == 0){
                //   If a "<" was found, use the file as the argument
                if (hasIn){
                    printf("%s\n", fileRead);
                    if (chdir(fileRead) != 0){
                        perror("chdir() failed");
                    } else {
                        continue;
                    }
                }
                // Otherwise, run cd normally
                else {
                    if (chdir(argsString[0]) != 0){
                        perror("chdir() failed");
                    } else {
                        continue;
                    }
                }

            }

            // Otherwise, if the command was "pwd", print the CWD
            else if (strcmp(tempCommand, "pwd") == 0){
                printf("The PWD is: %s\n", getcwd(cwd, sizeof(cwd)));
            }

            // Otherwise, if the command was "exit", exit with code 0
            else if (strcmp(tempCommand, "exit") == 0){
                exit(0);
            }

            // Otherwise, attempt to make a child process using fork()
            else {
                pid = fork();
                if (pid < 0){
                    fprintf(stderr, "Fork Failed");
                    exit(-1);
                }
                else if (pid == 0){
                    // If a "<" was found, use the file as the argument
                    if (hasIn){
                        argsStringFix[0] = tempCommand;
                        // Itterate through each argument passed in the file
                        for( n = 0; n < fileIndex+1; n++){
                            argsStringFix[n+1] = fileString[n];
                        }
                        argsStringFix[n+2] = NULL;
                        execvp(tempCommand, argsStringFix);
                    } 
                    // Otherwise, run the command with the args given
                    else {
                        if (hasOut){
                            argsStringFix[0] = tempCommand;

                            // If we only have one argument, terminate the list early
                            if (oneArg){
                                argsStringFix[1] = NULL;
                            }
                            else {
                                argsStringFix[1] = argsString[0];
                            }
                            argsStringFix[2] = NULL;
                            execvp(tempCommand, argsStringFix);
                        } else {
                            if (hasArgs){
                                // Itterate through each argument passed
                                argsStringFix[0] = tempCommand;
                                for( n = 0; n < argIndex+1; n++){
                                    argsStringFix[n+1] = argsString[n];
                                }
                                argsStringFix[n+2] = NULL;
                            } else{
                                argsStringFix[0] = tempCommand;
                                argsStringFix[1] = NULL;
                                argsStringFix[2] = NULL;
                            }
                            execvp(tempCommand, argsStringFix);
                        }
                    }
                    exit(0);
                }
                else {
                    wait(NULL);
                }
            }
        }

        // Clear the previous file contents from memory
        memset(fileRead, 0, sizeof(fileRead));
        fclose(fp);

        // If a file was opened, resume printing to the terminal (UNIX Specific)
        freopen ("/dev/tty", "a", stdout);
    }

}