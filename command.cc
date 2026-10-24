
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
   bool error=false;
   for(std::size_t i=0;i<tokens.size();i++){
        if(tokens[i].type==TOKEN_COMMAND){
            char *argument=(char *)malloc(tokens[i].value.size()+1);
            strcpy(argument,tokens[i].value.c_str());
            currentSimpleCommand->insertArgument(argument);
        } 
        if(tokens[i].type==TOKEN_PIPE){
            if(currentSimpleCommand->_numberOfArguments==0){
                printf("Error! Missing command near pipe\n");
                error=true;
                break;
            }
            currentCommand.insertSimpleCommand(currentSimpleCommand);
            SimpleCommand *nextSimpleCommand = new SimpleCommand();
            currentSimpleCommand=nextSimpleCommand;
        }
        if(tokens[i].type==TOKEN_REDIRECT){
            if(i+1 >= tokens.size() ||
            tokens[i+1].type == TOKEN_EOF ||
            tokens[i+1].type == TOKEN_PIPE ||
            tokens[i+1].type == TOKEN_REDIRECT ||
            tokens[i+1].type == TOKEN_APPEND ||
            tokens[i+1].type == TOKEN_INPUT ||
            tokens[i+1].type == TOKEN_ERROR ||
            tokens[i+1].type == TOKEN_REDIRECT_AND_ERROR){
                printf("Syntax error: missing filename after >\n");
                error = true;
                break;
            }
            currentCommand._outFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._outFile,tokens[i].value.c_str());
            continue;
        }
        if(tokens[i].type==TOKEN_APPEND){
            if(i+1 >= tokens.size() ||
            tokens[i+1].type == TOKEN_EOF ||
            tokens[i+1].type == TOKEN_PIPE ||
            tokens[i+1].type == TOKEN_REDIRECT ||
            tokens[i+1].type == TOKEN_APPEND ||
            tokens[i+1].type == TOKEN_INPUT ||
            tokens[i+1].type == TOKEN_ERROR ||
            tokens[i+1].type == TOKEN_REDIRECT_AND_ERROR){
                printf("Syntax error: missing filename after >>\n");
                error = true;
                break;
            }
            currentCommand._outFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._outFile,tokens[i].value.c_str());
            currentCommand._append=1;
            continue;
        }
        if(tokens[i].type==TOKEN_INPUT){
            if(i+1 >= tokens.size() ||
            tokens[i+1].type == TOKEN_EOF ||
            tokens[i+1].type == TOKEN_PIPE ||
            tokens[i+1].type == TOKEN_REDIRECT ||
            tokens[i+1].type == TOKEN_APPEND ||
            tokens[i+1].type == TOKEN_INPUT ||
            tokens[i+1].type == TOKEN_ERROR ||
            tokens[i+1].type == TOKEN_REDIRECT_AND_ERROR){
                printf("Syntax error: missing filename after <\n");
                error = true;
                break;
            }
            currentCommand._inputFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._inputFile,tokens[i].value.c_str());
            continue;
        }
        if(tokens[i].type==TOKEN_BACKGROUND){
            currentCommand._background=1;
        }
        if(tokens[i].type==TOKEN_ERROR){
            if(i+1 >= tokens.size() ||
            tokens[i+1].type == TOKEN_EOF ||
            tokens[i+1].type == TOKEN_PIPE ||
            tokens[i+1].type == TOKEN_REDIRECT ||
            tokens[i+1].type == TOKEN_APPEND ||
            tokens[i+1].type == TOKEN_INPUT ||
            tokens[i+1].type == TOKEN_ERROR ||
            tokens[i+1].type == TOKEN_REDIRECT_AND_ERROR){
                printf("Syntax error: missing filename after 2>\n");
                error = true;
                break;
            }
            currentCommand._errFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._errFile,tokens[i].value.c_str());
            continue;
        }
        if(tokens[i].type==TOKEN_REDIRECT_AND_ERROR){
            if(i+1 >= tokens.size() ||
            tokens[i+1].type == TOKEN_EOF ||
            tokens[i+1].type == TOKEN_PIPE ||
            tokens[i+1].type == TOKEN_REDIRECT ||
            tokens[i+1].type == TOKEN_APPEND ||
            tokens[i+1].type == TOKEN_INPUT ||
            tokens[i+1].type == TOKEN_ERROR ||
            tokens[i+1].type == TOKEN_REDIRECT_AND_ERROR){
                printf("Syntax error: missing filename after >>&\n");
                error = true;
                break;
            }
            currentCommand._errFile=(char *)malloc(tokens[++i].value.size()+1);
            strcpy(currentCommand._errFile,tokens[i].value.c_str());
            currentCommand._outFile=(char *)malloc(tokens[i].value.size()+1);
            strcpy(currentCommand._outFile,tokens[i].value.c_str());
            currentCommand._out_error=1;
            currentCommand._append=1;
            continue;
        }
        if(tokens[i].type==TOKEN_ARGUMENT){
            char *argument=(char *)malloc(tokens[i].value.size()+1);
            strcpy(argument,tokens[i].value.c_str());
            currentSimpleCommand->insertArgument(argument);
        }
        if(tokens[i].type==TOKEN_EOF){
            if(currentSimpleCommand->_numberOfArguments==0){
                printf("Error! Missing command near pipe\n");
                error=true;
            }
            if(error)
                break;
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
    _append=0;
    _out_error=0;
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
    pid_t pid;
    while((pid=waitpid(-1,&status,WNOHANG))>0){
        FILE *logfile = fopen("logfile.log", "a");
        time_t now=time(NULL);
        if(logfile)
        {fprintf(logfile,"Child %d terminated at %s",pid,ctime(&now));
        fclose(logfile);}
    }
}
void Command::execute()
{
    if(_numberOfSimpleCommands==0||_simpleCommands[0]->_numberOfArguments==0){
        clear();        
        return;
    }
    print();
    /*
        Write your code here
    */
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
    // prompt();
    return;
}
    signal(SIGCHLD, terminatedChild); 
    int prev=-1;
    int lastpid=-1;
    for(int i=0;i<_numberOfSimpleCommands;i++){
        int fdpipe[2];
        if(i<_numberOfSimpleCommands-1){
            if(pipe(fdpipe)==-1){
                perror("pipe");
                exit(2);
            }
        }
        pid_t pid=fork();
        if(pid==-1){
            perror("fork");
            exit(2);
        }
        lastpid=pid;
        if(!pid){
            if(i<_numberOfSimpleCommands-1){
                dup2(fdpipe[1],1);
            }
            if(i>0){
                dup2(prev,0);
            }
            if(prev!=-1){
                close(prev);
            }
            if(i<_numberOfSimpleCommands-1){
                close(fdpipe[0]);
                close(fdpipe[1]);
            }
            if(_outFile && !_out_error && i==_numberOfSimpleCommands-1){
                int file_fd;
                if(_append){
                    file_fd=open(_outFile,O_WRONLY|O_CREAT|O_APPEND,0666);
                }
                else {
                    file_fd=open(_outFile,O_WRONLY|O_CREAT|O_TRUNC,0666);
                }
                if(file_fd<0){
                    perror("Error opening file");
                    _exit(1);
                }
                dup2(file_fd,STDOUT_FILENO);
                close(file_fd);
            }
            if(_inputFile && i==0){
                int file_fd=open(_inputFile,O_RDONLY);
                if(file_fd<0){
                    perror("Error opening file");
                    _exit(1);
                }
                dup2(file_fd,STDIN_FILENO);
                close(file_fd);
            }
            if(_errFile && i==_numberOfSimpleCommands-1){
                int file_fd;
                if(_append){
                    file_fd=open(_errFile,O_WRONLY|O_CREAT|O_APPEND,0666);
                }
                else {
                    file_fd=open(_errFile,O_WRONLY|O_CREAT|O_TRUNC,0666);
                }
                if(file_fd<0){
                    perror("Error opening file");
                    _exit(1);
                }
                if(_out_error){
                    dup2(file_fd,STDOUT_FILENO);
                }
                dup2(file_fd,STDERR_FILENO);
                close(file_fd);
            }
            signal(SIGINT,SIG_DFL);
            std::vector<char *> newargs;
            for(int j=0;_simpleCommands[i]->_arguments[j]!=NULL;j++){
                if(strchr(_simpleCommands[i]->_arguments[j],'*')||strchr(_simpleCommands[i]->_arguments[j],'?')){
                    glob_t match;
                    int res=glob(_simpleCommands[i]->_arguments[j],0,NULL,&match);
                    if(res==0){
                        for(size_t k=0; k<match.gl_pathc;k++){
                            newargs.push_back(strdup(match.gl_pathv[k]));
                        }
                    }
                    else{
                        newargs.push_back(strdup(_simpleCommands[i]->_arguments[j]));
                    }
                    globfree(&match);
                }
                else{
                    newargs.push_back(strdup(_simpleCommands[i]->_arguments[j]));
                }
            }
            newargs.push_back(NULL);
            char **args=new char*[newargs.size()];
            for (size_t k=0; k<newargs.size();k++) {
                args[k]=newargs[k];
            }
            execvp(args[0], args);
            perror("exec error");
            _exit(2);
        }
        if(prev!=-1){
            close(prev);
        }
        if(i<_numberOfSimpleCommands-1){
            close(fdpipe[1]);
            prev=fdpipe[0];
        }
    }
    if(!_background){
        waitpid(lastpid,0,0);
    }  
    clear();
    // prompt();
    return;
}

void Command::prompt()
{
    signal(SIGINT, SIG_IGN);
    // printf("myshell>");
    fflush(stdout);
    std::string input;
    while (true)
    {
        printf("myshell>");
        std::getline(std::cin, input); 
        std::vector<Token> tokens = tokenize(input);
        parse(tokens);
    }
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int main()
{
    signal(SIGINT, SIG_IGN);
    Command::_currentCommand.prompt();
    return 0;
}
