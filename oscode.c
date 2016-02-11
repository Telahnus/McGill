#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int getcmd(char *prompt, char *args[], int *background){
    int length, i = 0;
    char *token, *loc;
    char *line;
    size_t linecap = 0;

    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);

    if (length <= 0) {
        exit(-1);
    }

    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else
        *background = 0;

    while ((token = strsep(&line, " \t\n")) != NULL) {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
            args[i++] = token;
    }

    args[i++] = '\0';
    return i;
}

void freecmd (char *args[], int count){
	for(int i=0; i<count; i++){
		if(args[i] != NULL){
			args[i] = NULL;
		}
	}
}

int main(){
    char *args[20];
    int bg;

    while(1){
    	bg = 0;
		int cnt = getcmd("\n>> ", args, &bg);
		
		pid_t pid = fork();
		if (pid < 0){
			perror("fork failed");
			exit(1);
		}else if (pid == 0){ // child
			execvp(args[0],args);
			perror("error in execvp");
			_exit(EXIT_FAILURE);
		}else{ // parent
			int status;
			if(bg == 1){
				waitpid(pid, &status, 0);
			}
			freecmd(args, cnt); // reset commands
		}
	}
}
