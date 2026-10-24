
/*
 * 
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <glob.h>
#include <wordexp.h>
#include <vector>
#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
#include "command.h"
#include "tokenizer.h"


void parse(std::vector<Token> &tokens)
{
    Command currentCommand;
    SimpleCommand *currentSimpleCommand = new SimpleCommand();

    // currentCommand.execute();
    /*
      Implement this function 
      you must parse for all possible tokens scenario
      right now command.execute only print the format , you will inject it in your logic.

    */
   for(std::size_t i=0;i<tokens.size();i++){
        if(tokens[i].type==TOKEN_COMMAND){
            char *argument=(char *)malloc(tokens[i].value.size()+1);
            strcpy(argument,tokens[i].value.c_str());
            currentSimpleCommand->insertArgument(argument);
        } 
        if(tokens[i].type==TOKEN_PIPE){
            currentCommand.insertSimpleCommand(currentSimpleCommand);
            SimpleCommand *nextSimpleCommand = new SimpleCommand();
            currentSimpleCommand=nextSimpleCommand;
        }
        if(tokens[i].type==TOKEN_REDIRECT){
            currentCommand._outFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._outFile,tokens[i].value.c_str());
            continue;
        }
        if(tokens[i].type==TOKEN_APPEND){
            currentCommand._outFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._outFile,tokens[i].value.c_str());
            currentCommand._append=1;
            continue;
        }
        if(tokens[i].type==TOKEN_INPUT){
            currentCommand._inputFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._inputFile,tokens[i].value.c_str());
            continue;
        }
        if(tokens[i].type==TOKEN_BACKGROUND){
            currentCommand._background=1;
        }
        if(tokens[i].type==TOKEN_ERROR){
            currentCommand._errFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._errFile,tokens[i].value.c_str());
            continue;
        }
        if(tokens[i].type==TOKEN_REDIRECT_AND_ERROR){
            currentCommand._errFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._errFile,tokens[i].value.c_str());
            currentCommand._outFile=(char *)malloc(tokens[i].value.size()+1);
            strcpy(currentCommand._outFile,tokens[i].value.c_str());
            currentCommand._out_error=1;
            continue;
        }
        if(tokens[i].type==TOKEN_ARGUMENT){
            char *argument=(char *)malloc(tokens[i].value.size()+1);
            strcpy(argument,tokens[i].value.c_str());
            currentSimpleCommand->insertArgument(argument);
        }
        if(tokens[i].type==TOKEN_EOF){
            currentCommand.insertSimpleCommand(currentSimpleCommand);
            currentCommand.execute();
        }
   }
}

SimpleCommand::SimpleCommand()
{
    _numberOfAvailableArguments = 5;
    _numberOfArguments = 0;
    _arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
}

void SimpleCommand::insertArgument(char *argument)
{
    if (_numberOfAvailableArguments == _numberOfArguments + 1)
    {
        _numberOfAvailableArguments *= 2;
        _arguments = (char **)realloc(_arguments,
                                      _numberOfAvailableArguments * sizeof(char *));
    }

    _arguments[_numberOfArguments] = argument;
    _arguments[_numberOfArguments + 1] = NULL;

    _numberOfArguments++;
}

Command::Command()
{
    _numberOfAvailableSimpleCommands = 1;
    _simpleCommands = (SimpleCommand **)
        malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

    _numberOfSimpleCommands = 0;
    _outFile = 0;
    _inputFile = 0;
    _errFile = 0;
    _background = 0;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
    if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
    {
        _numberOfAvailableSimpleCommands *= 2;
        _simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
                                                    _numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
    }

    _simpleCommands[_numberOfSimpleCommands] = simpleCommand;
    _numberOfSimpleCommands++;
}

void Command::clear()
{
    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
            free(_simpleCommands[i]->_arguments[j]);
        }

        free(_simpleCommands[i]->_arguments);
        free(_simpleCommands[i]);
    }

    if (_outFile)
    {
        free(_outFile);
    }

    if (_inputFile)
    {
        free(_inputFile);
    }

    if (_errFile)
    {
        free(_errFile);
    }

    _numberOfSimpleCommands = 0;
    _outFile = 0;
    _inputFile = 0;
    _errFile = 0;
    _background = 0;
}

void Command::print()
{
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        printf("  %-3d ", i);
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
            printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
        }
    }

    printf("\n\n");
    printf("  Output       Input        Error        Err&Out       Background\n");
    printf("  ------------ ------------ ------------ ------------ ------------\n");
    printf("  %-12s %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
           _inputFile ? _inputFile : "default", _errFile ? _errFile : "default", _out_error == 1 ? _errFile : "default"
           ,_background ? "YES" : "NO");
    printf("\n\n");
}
void terminatedChild(int signal){
    int status;
    pid_t pid=waitpid(-1,&status,WNOHANG);
    if(pid>0){
        FILE *logfile = fopen("logfile.log", "a");
        time_t now=time(NULL);
        if(logfile)
        {fprintf(logfile,"Child %d terminated at %s",pid,ctime(&now));
        fclose(logfile);}
        else{
            printf("Error creating file");
        }
    }
}
void Command::execute()
{
    print();
    /*
        Write your code here
    */
   if(_numberOfSimpleCommands==0||_simpleCommands[0]->_numberOfArguments==0)
        return;
    if(!strcmp(_simpleCommands[0]->_arguments[0],"exit")){
        printf("byebye!!!! :D\n");
        clear();
        exit(0);
    }
    if(!strcmp(_simpleCommands[0]->_arguments[0], "cd")){
    if(_simpleCommands[0]->_numberOfArguments == 1){
        chdir(getenv("HOME"));
    } 
    else{
        if(chdir(_simpleCommands[0]->_arguments[1]) != 0){
            perror("cd");
        }
    }
    clear();
    prompt();
    return;
}
    signal(SIGCHLD, terminatedChild);
    pid_t pid=fork();
    if(!pid){
        if(_outFile){
            int file_fd;
            if(_append){
                file_fd=open(_outFile,O_WRONLY|O_CREAT|O_APPEND,0666);
            }
            else {
                file_fd=open(_outFile,O_WRONLY|O_CREAT|O_TRUNC,0666);
            }
            if(file_fd<0){
                perror("Error opening file");
                exit(1);
            }
            dup2(file_fd,STDOUT_FILENO);
            close(file_fd);
        }
        if(_inputFile){
            int file_fd=open(_inputFile,O_RDONLY);
            if(file_fd<0){
                perror("Error opening file");
                exit(1);
            }
            dup2(file_fd,STDIN_FILENO);
            close(file_fd);
        }
        signal(SIGINT,SIG_DFL);
        execvp(_simpleCommands[0]->_arguments[0], _simpleCommands[0]->_arguments);
    }
    else if(pid>0){
        if(!_background)
            waitpid(pid,NULL,0);
    }
    else
        perror("fork");
    clear();
    prompt();
}

void Command::prompt()
{
    signal(SIGINT, SIG_IGN);
    printf("myshell>");
    fflush(stdout);
    std::string input;
    while (true)
    {
        std::getline(std::cin, input); 
        std::vector<Token> tokens = tokenize(input);
        parse(tokens);
    }
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int main()
{
    Command::_currentCommand.prompt();
    return 0;
}
