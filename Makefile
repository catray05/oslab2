## you have command.cc command.h tokenizer.cc tokenizer.h you need compile them and link
## them to one object file called  : "myshell" 
## so we should run make , that will create our object myshell ,
## ./myshell should run your application 
CC= g++ -g

all: myshell

myshell: command.o tokenizer.o
	$(CC) -o myshell command.o tokenizer.o
command.o: command.cc command.h tokenizer.h
	$(CC) -c command.cc
tokenizer.o: tokenizer.cc tokenizer.h
	$(CC) -c tokenizer.cc
clean:
	rm -f *.o myshell