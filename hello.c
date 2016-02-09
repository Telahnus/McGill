#include<stdio.h>

int main(int argc, const char* argv[]){
  
  int i,x;


  if (argc==2){
    x = strtol(argv[1],NULL,10);
    for(i=0; i<x; i++){
      printf("counting: %d\n",i);
    }
  } else {
    printf("Hello world!\n");
  }

  return 0;

}
