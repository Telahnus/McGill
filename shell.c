#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80 // chars per line, per command, should be enough

// setup() reads in the next command line, separating it into distinct tokens using whitespace as delimiters. setup() sets the args parameter as a null-terminated string.

void setup(char inputBuffer[], char *args[], int *background){

  int length, // # of chararacters in the command line
      i,      // loop index for accessing inputBuffer array
      start,  // index where beginning of next command parameter is
      ct;     // index of where to place the next parameter into args[]

  ct = 0;

  // read what the user enters on the command line
  length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
  
  inputBuffer[length]='\0';
  start = -1;

  if (length == 0) { // ctrl-d was entered, quit the shell normally
    printf("\n");
    exit(0);
  }
  if (length < 0){ //something wrong, terminate with error code -1
    perror("error reading the command");
    exit(-1); 
  }

  // examine every character in the inputBuffer
  for (i=0; i<length; i++){
    switch (inputBuffer[i]){
      case ' ':
      case '\t': // arguement separators
        if (start != -1){
	  args[ct]= &inputBuffer[start]; // set up pointer
	  ct++;
        }
        inputBuffer[i]='\0'; // add a null char; make a C string
        start =-1;
        break;

      case '\n': // should be the final char examined
        if (start != -1){
	  args[ct] = &inputBuffer[start];
	  ct++;
        }
        inputBuffer[i] = '\0';
        args[ct] = NULL; // no more arguments to this command
        break;

      default: // some other character
	if (inputBuffer[i] == '&'){
	  *background = 1;
	  inputBuffer[i] = '\0';
          start = -1;
        } else if (start == -1) start = i;
    }
  }
  args[ct]=NULL; // just in case the input line was > 80
}




/*
void CP(void){
  int i;
  for (i=1; i<10; i++) printf("This line is from child, value = %d\n", i);
  printf("CP is done\n");
  exit(0);
}

void PP(void){
  int i;
  for (i=1; i<10; i++) printf("This line is from parent, value = %d\n", i);
  printf("PP is done\n");
}
*/

int main (void)
{
  char inputBuffer[MAX_LINE];	// buffer to hold the command entered
  int background;            	// equals 1 if a command is followed by '&'
  char *args[MAX_LINE + 1]; 	// command line (of 80) has max of 40 arguments
  pid_t pid; 			// child id


  while (1){
    background = 0;
    printf("COMMAND->\n");
    setup(inputBuffer, args, &background); // get next command
    
    /* my code goes here */

    pid=fork();			//forks a child process

    if (pid<0) { 			//check forking
      printf("forking error\n");	
      exit(1);
    }
    else if (pid == 0) { 		//start of child process
      printf("child process started: %d\n", getpid());
      if (execvp(*args, args)<0){ 	//check execution
        printf("execution error\n");	
        exit(1);
      }				
    }
    else {				//start of parent process
      if (background == 0){
        waitpid(pid);
        printf("waiting for child to finish\n");
      }			
      printf("parent process continuing\n");
    }

  }
}
