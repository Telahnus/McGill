#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* Jordan Quan, 260565221, Sept 26, 2014 */
/* Comp 310, Assignment 1 */

#define MAX_LINE 80 // chars per line, per command, should be enough

char hist[10][MAX_LINE];
int histCount = 0;
pid_t children[50];
int childCount = 0;

// setup() reads in the next command line, separating it into distinct tokens 
// using whitespace as delimiters. setup() sets the args parameter as a null-terminated string.

void setup(char inputBuffer[], char *args[], int *background){

  int length, // # of chararacters in the command line
      i,      // loop index for accessing inputBuffer array
      start,  // index where beginning of next command parameter is
      ct;     // index of where to place the next parameter into args[]

  ct = 0;

  // read what the user enters on the command line
  length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
  
  //add input to history
  strcpy(hist[histCount%10],inputBuffer);
  histCount++;

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


/**************************************************/

void addChild(pid_t pid){
  int i = 0;
  while(children[i]!=-1){
    i++;
  }
  children[i]=pid;
}

void checkJobs (){
  int i=0;
  for (i=0;i<50;i++){
    if (kill(children[i],0)){
      children[i]=-1;
    }
  }
}

/**************************************************/
/**************************************************/


int main (void)
{
  char inputBuffer[MAX_LINE];	        // buffer to hold the command entered
  int background;            	        // equals 1 if a command is followed by '&'
  char *args[MAX_LINE/2 + 1]; 	      // command line (of 80) has max of 40 arguments
  pid_t pid; 		                      // pid and childpid
  int i, j, k;  		                  // loop iterators
  int skip;			                      // used to track a shell command run by parent and skip child
  char* cwd;			                    // will hold pwd
  char buff[80];		                  // buffer for pwd
  int execStatus;		                  // check if child execvp worked
  i=j=k=execStatus=skip=0;            // initialize some vars

  for (i=0; i<50; i++){
    children[i]=-1;
  }

  while (1){
    background = 0;
    skip = 0;
    printf("COMMAND->\n");
    setup(inputBuffer, args, &background); 		// get next command


// built in commands
/**************************************************/

//built in redo command
    //NOT WORKING
    /*
    if (strcmp(inputBuffer,"r")==0){
      histCount=histCount-2;
      j=0;
      printf("%d: ",histCount);
      while( hist[histCount%10][j] != '\n' && hist[histCount%10][j] != '\0'){
        printf("%c",hist[histCount%10][j]);
        j++;
      }
      printf("\n");
      strcpy(inputBuffer,hist[histCount%10]);   // set last command as inputBuffer
      //setup(inputBuffer, args, &background);        // redo setup using last command
    }*/

// built in change directory
    if (strcmp(inputBuffer,"cd")==0){
      skip=1;
      printf ("changing directory to %s\n", args[1]);  
      if (chdir (args[1]) == -1) {  
        printf("chdir failed!\n"); 
      } else {  
        printf ("chdir successful\n");    
      }  
    }

//built in print working directory
    if (strcmp(inputBuffer,"pwd")==0){
      skip=1;
      cwd = getcwd(buff,80);
      if (cwd!=NULL){
        printf ("current working directory is %s\n",cwd);  
      }
    }

//built in exit command
    if (strcmp(inputBuffer,"exit")==0){
      printf("exiting shell\n");
      exit(0); 
    }

//built in history command
    if (strcmp(inputBuffer,"history")==0){
      skip=1;
      j=k=0;
      for (i=1;i<10;i++){
        j=histCount-i;          // j counts 10 down from histCount
        if(j>=0){
          printf("%d: ",j);
          while( hist[j%10][k] != '\n' && hist[j%10][k] != '\0'){
            printf("%c",hist[j%10][k]);
            k++;
          }
          printf("\n");
          k=0;
        }
      } 
    }
/*
//built in jobs command
    if (strcmp(inputBuffer,"jobs")==0){
      char str[50]="ps -o pid --ppid ";
      char ppid[7];
      sprintf(ppid,"%d",getpid());
      strcat(str,ppid);
      system(str);
    }
*/
// alt built in jobs command
    if (strcmp(inputBuffer,"jobs")==0){
      for (i=0;i<50;i++){
        if (children[i]!=-1){
          printf("Child # %d\n",children[i]);
        }
      }
    }


// forking child process
/**************************************************/

    if (skip==0){
      pid=fork();					                          //forks a child process

      if (pid<0) { 					                        //check forking
        printf("forking error\n");	
        exit(1);
      }
      else if (pid == 0) { 				                  //start of child process       
	      //printf("child pid = %d\n", getpid());
        //printf("parent pid = %d\n", getppid());
        if (execvp(*args, args)<0){ 			          //check execution
	        printf("execution error\n");	
          exit(1);
        }	
      }
      else {						                            //start of parent process
        if (background == 0){
          printf("waiting for child process # %d\n", pid);
          addChild(pid);
          waitpid(pid);
          //printf("finished waiting for child process # %d\n", pid);
        } else {
	        printf("parent NOT waiting\n");
	      }	
        printf("resuming parent process\n");
      }
    }


/**************************************************/

  }							//close while and end
}
