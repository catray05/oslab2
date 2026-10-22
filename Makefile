## you have command.cc command.h tokenizer.cc tokenizer.h you need compile them and link
## them to one object file called  : "myshell" 
## so we should run make , that will create our object myshell ,
## ./myshell should run your application 
CC= g++ -g

all: myshell

myshell: command tokenizer
	$(CC) -o myshell command tokenizer
command: command.cc command.h
	$(CC) -o command command.cc command.h
tokenizer: tokenizer.cc tokenizer.h
	$(CC) -o tokenizer tokenizer.cc tokenizer.h
clean:
	rm -f tokenizer myshell command *.o