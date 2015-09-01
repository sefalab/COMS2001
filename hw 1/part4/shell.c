//844165 Raesetje Bonjo Sefala 
//hw1



#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h> 

#define INPUT_STRING_SIZE 80

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

int cmd_quit(tok_t arg[]) {
  printf("Bye\n");
  exit(0);
  return 1;
}

int cmd_cd(tok_t args[]){

if(args[0]==NULL)
return 0;


else

chdir(args[0]); 

}


int cmd_help(tok_t arg[]);


/* Command Lookup table */
typedef int cmd_fun_t (tok_t args[]); /* cmd functions take token array and return int */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_quit, "quit", "quit the command shell"},
  {cmd_cd, "cd", "allows you to move to another directory"},

};

int cmd_help(tok_t arg[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

int lookup(char cmd[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
  }
  return -1;
}

void init_shell()
{
  /* Check if we are running interactively */
  shell_terminal = STDIN_FILENO;
  
  /** Note that we cannot take control of the terminal if the shell
   is not interactive */
  shell_is_interactive = isatty(shell_terminal);
  
  if(shell_is_interactive){
    
    /* force into foreground */
    while(tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
      kill( - shell_pgid, SIGTTIN);
    
    shell_pgid = getpid();
    /* Put shell in its own process group */
    if(setpgid(shell_pgid, shell_pgid) < 0){
      perror("Couldn't put the shell in its own process group");
      exit(1);
    }
    
    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
  }
  /** YOUR CODE HERE */
  
}

/**
 * Add a process to our process list
 */
void add_process(process* p)
{
  /** YOUR CODE HERE */
}

/**
 * Creates a process given the inputString from stdin
 */
process* create_process(char* inputString)
{
  pid_t tcpid;
  pid_t pid = getpid(); // get pid of the current process ...
  // printf( "Parent pid: %d\n", pid ); // ... and print it out
  pid_t cpid;
  pid_t mypid;
  
  
  /** YOUR CODE HERE */
  return NULL;
}

char* concat(char *s1, char *s2)
{
  char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
  //in real code you would check for errors in malloc here
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

//providing the path for the function to be executed
void part2(tok_t * t){
    execv(*t,t);
      perror(*t);
      exit(0);
}

//finds the path...actually implements ls
void part3(tok_t *t){
  char *poi=getenv("PATH");
      tok_t * pois = getToks(poi);
      int i;
      for(i = 0;i<MAXTOKS && pois[i];i++){
        //char *( char *pois, *t);
        char *fi=concat(pois[i],"/");
        fi=concat(fi,t[0]);
        if(access(fi,F_OK) != -1){
          execve(fi,t, NULL);
         printf("%s \n",fi);
        }
        //perror(*t);
      }
}

//enables you to read input from or to write output to a file
void part4(tok_t *input,char * filename,char * c){
int newfd;
 
 if(c == ">"){
  //printf("PART 4\n");
  if ((newfd = open(filename, O_CREAT|O_WRONLY | O_APPEND, 0644)) < 0) {
	perror(input);
	exit(1);
  }
 
 dup2(newfd, 1);
 close(newfd);
 }
 if(c == "<"){
  if ((newfd = open(filename, O_RDONLY, 0644)) < 0) {
	perror(input);
	exit(1);
  }
 dup2(newfd,0);
 close(newfd);
 }
  part3(input);
  part2(input);
}
int shell (int argc, char *argv[]) {
  char *s = malloc(INPUT_STRING_SIZE+1);			/* user input string */
  tok_t *t;			/* tokens parsed from input */
  int lineNum = 0;
  int fundex = -1;
  pid_t pid = getpid();		/* get current processes PID */
  pid_t ppid = getppid();	/* get parents PID */
  pid_t cpid, tcpid, cpgid;
  
  init_shell();
  
  printf("%s running as PID %d under %d\n",argv[0],pid,ppid);
  
  lineNum=0;
  char cwd[5000];
  fprintf(stdout, "%d: %s:", lineNum, getcwd(cwd,sizeof(cwd)));
  while ((s = freadln(stdin))){
    t = getToks(s); /* break the line into tokens */
  fundex = lookup(t[0]); /* Is first token a shell literal */
  if(fundex >= 0) cmd_table[fundex].fun(&t[1]);
  else {
    pid = fork(); 
    
    if( pid == 0 ){ // child process
     // printf("%d\n", strlen(strstr(s,">")));
         int i;
         for(i=0;i<MAXTOKS && t[i];i++){
		 if (strcmp( t[i], ">") == 0){
	 	  t[i]=NULL;
		  //printf("hello");
		  part4(t,t[i+1],">");	
                  }
		if (strcmp( t[i], "<") == 0){
	 	  t[i]=NULL;
		  //printf("hello");
		  part4(t,t[i+1],"<");	
                  }
		}
      part3(t);
      part2(t);

    }else if(pid<0){
      perror( "Fork failed" );
      exit( EXIT_FAILURE );    
    }
  }
  lineNum++;
  wait(NULL);
  
  fprintf(stdout, "%d: %s :", lineNum, getcwd(cwd,sizeof(cwd)));
  }
  return 0;
}

