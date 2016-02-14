#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
//#include <sys/types.h>
typedef struct History{
    int num;
    char args[20][80];
    int bg;
    int status;
    pid_t pid;
    struct History * prev;
    struct History * next;
} Hist;

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

    //args[i++] = '\0';
    args[i]=NULL;
    return i;
}

void freecmd (char *args[]){
	for(int i=0; i<20; i++){
		if(args[i] != NULL){
			args[i] = NULL;
		}
	}
}

/*void checkup(Hist *node){
	//do {
		int status;
		pid_t pid = node->pid;
		pid_t result = waitpid (pid, &status, WNOHANG); //wont actually wait
		printf("pid:%d,status:%d\n", pid, result);
		if (result == 0){ //still running
			continue;
		} else if (result == -1) { // error
			printf("error checking child status\n");
		} else { // child exited
			node->status = status;
		}
	//} while (node->prev != NULL);
}*/

void update(){
	printf("received signal\n");
}

void printhistory(Hist *node){
	//print out the last 10 cmds
	Hist *cur = node->prev;
	if (cur == NULL){
		printf("no previous command history found");
	} else {
		int i = 0;
		while (cur != NULL && i<10){
			printf("%d: ", cur->num);
			int j = 0;
			while (strcmp(cur->args[j], "")!=0){
				printf("%s ", cur->args[j]);
				j++;
			}
			printf("\n");
			cur = cur->prev;
			i++;
		}
	}
}

Hist* findhist(Hist *node, int num){
	Hist *cur = node->prev;
	if (cur == NULL){
		printf("no previous command history found\n");
	} else if (cur->num < num || cur->num-10 > num){
		printf("command not found / out of range\n");
	} else {
		int i = 0;
		while (cur != NULL && i<10){
			if (cur->num == num){
				return cur;
			}
			cur = cur->prev;
			i++;
		}
		printf("command not found / out of range\n");
	}
	return NULL;
}

void pwd(){
	int PATH_MAX = 200; // arbitrary char limit
	char* cwd;
	char buff[PATH_MAX + 1];
	cwd = getcwd( buff, PATH_MAX + 1 );
	if( cwd != NULL ) {
		printf( "Current working directory is %s.\n", cwd );
	}
}

void printjobs(Hist *node){
	Hist *cur = node->prev;
	if (cur == NULL){
		printf("no previous jobs found");
	} else {
		while (cur != NULL){
			if (cur->bg==1){
				//print job
				printf("%d: ", cur->num);
				int j = 0;
				while (strcmp(cur->args[j], "")!=0){
					printf("%s ", cur->args[j]);
					j++;
				}
				// check if it already finished
				int status;
				pid_t rpid = waitpid(cur->pid, &status, WNOHANG); //wnohang does not actually wait
				if (rpid == -1) {
					perror("error checking job");
					cur->bg = 0;
				} else if (rpid == 0) {
					printf("still running");
				} else if (rpid == cur->pid) {
					printf("finished, status: %d", status);
					cur->status = status; // update status
					cur->bg = 0; // remove from background list
				}
				printf("\n");
			}
			cur = cur->prev;
		}
		printf("Done printing background jobs\n");
	}
}

void fg(Hist *node, int num){
	Hist *cur = node->prev;
	if (cur == NULL){
		printf("no previous jobs found");
	} else {
		while (cur != NULL){
			if (cur->num==num){ //found the job
				if (cur->bg==0){
					printf("This job was already foreground\n");
					return;
				} else if (cur->status==0){
					printf("This job has already finished\n");
					cur->bg = 0;
					return;
				} else {
					//print job
					printf("Bringing %d: ", cur->num);
					int j = 0;
					while (strcmp(cur->args[j], "")!=0){
						printf("%s ", cur->args[j]);
						j++;
					}
					printf("to foreground...");

					pid_t pid = cur->pid;
					int status;
					waitpid(pid, &status, 0);
					cur->status = status;
					cur->bg = 0;

					printf(" finished with status %d", status);
					return;
				}
			}
			cur = cur->prev;
		}
		printf ("Could not find specified job\n");
		return;
	}
}

void redirect(char *args[], int cnt){
	for (int k=0; k<cnt; k++){
		if (strcasecmp(args[k],">")==0){
			close(1);
			int PATH_MAX = 200;
			char* path;
			char buff[PATH_MAX + 1];
			path = getcwd( buff, PATH_MAX + 1 );
			strcat(buff, "/");
			strcat(buff, args[k+1]);
			if (open(path, O_WRONLY) == -1) {
				perror(path);
				_exit(1);
			}
			args[k]='\0';
			args[k++]='\0';
		}
	}
}

int main(){
    char *args[20];
    int bg;
    int num=1;

	Hist * node = NULL;
	node = malloc(sizeof(Hist));
	node->prev = NULL;
	//pid_t ppid = getpid();

    while(1){
    	bg = 0;
		int cnt = getcmd("\n>> ", args, &bg);

		// save cmd to history
		node->num = num;
		int i=0;
		while(args[i]!=NULL){
			strcpy(node->args[i], args[i]);
			i++;

		}
		node->bg = bg;
		//printf("%d: %s, %d",node->num, node->args[0], node->bg);

		// built in commands
		int builtin=0; // built in flag, prevents child forking

		// print history
		if(strcasecmp(args[0],"history")==0){
			builtin = 1;
			printhistory(node);
			freecmd(args);
			continue;
		}

		// run history
		int ret;
		int runhistflag = 0;
		ret = atoi(args[0]); // convert string to integer
		if (ret != 0){ // check if integer
			Hist * recall = findhist(node, ret); // find history node associated with that number
			if (recall!=NULL){
				//printf("%d: %s\n", recall->num, recall->args[0]);
				if (recall->status!=0){
					printf("this was a bad command");
					freecmd(args);
					continue;
				}
				int r=0;
				while (strcmp(recall->args[r], "")!=0){
					args[r] = recall->args[r]; // overwrite current args with old ones
					r++;
				}
				bg = recall->bg;
				runhistflag = 1; 
			} else {
				freecmd(args);
				continue;
			}
		}

		// change directory
		if(strcasecmp(args[0],"cd")==0 && cnt==2){
			builtin = 1;
			chdir(args[1]);
			// already saved the args to the node, need to refresh manually
			strcpy(node->args[1], "");
			freecmd(args);
			continue;
		}

		// print working directory
		if(strcasecmp(args[0],"pwd")==0 && cnt==1){
			builtin = 1;
			pwd();
			freecmd(args);
			continue;
		}
		
		// exit shell
		if(strcasecmp(args[0],"exit")==0 && cnt == 1){
			// also kill all children
			kill(0, SIGTERM); // kill all P in same P group as sender
			exit(0);
		}

		// print background jobs
		if(strcasecmp(args[0],"jobs")==0 && cnt == 1){
			builtin = 1;
			printjobs(node);
			freecmd(args);
			continue;
		}

		// bring to foreground
		if(strcasecmp(args[0],"fg")==0 && cnt==2){
			builtin = 1;
			fg(node, atoi(args[1]));
			strcpy(node->args[1], "");
			freecmd(args);
			continue;
		}

		// non-built in commands
		if (!builtin){
			// fork the job
			pid_t ppid = getpid();
			pid_t pid = fork();
			if (pid < 0){
				perror("fork failed");
				exit(1);
			}else if (pid == 0){ // child
				redirect(args, cnt); // check for redirects
				execvp(args[0],args);
				perror("error in execvp");
				//kill(ppid, SIGINT);
				// do something to update history of bad command
				_exit(EXIT_FAILURE);
			}else{ // parent
				node->pid = pid;
				int status;
				if(bg == 0){ //not bg
					waitpid(pid, &status, 0);
					node->status = status;
				}
			}
		}

		// catch error signal from child... didnt work as planned
		//signal(SIGINT, update);

		if (!runhistflag){ // dont save reruns
			// create next node and link up
			Hist * next = malloc(sizeof(Hist));
			next->prev = node;
			node->next = next;
			node = next;

			num++;
		}
		freecmd(args); // reset commands
		
	}
}
