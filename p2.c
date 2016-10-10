/*P2
Programmer: Kyle Krick
Due: 10/12/16
CS570
Fall 2016
John Carroll*/

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "p2.h"
#include "/home/ma/cs570/CHK.h"

#define SPACE ' '
#define NEWLINE '\n'
#define LSSR '<'
#define GRTR '>'
#define PIPE '|'
#define DLLR '$'
#define AMP '&'
#define BACK '\\'
#define MAXARGS 20

void prompt();
void parse();
void sighandler();

int c; /*Number chars per word*/
int numwords; /*Number of words from input line*/
char s[STORAGE]; /*Used to store each word from input*/
char *firstword; /*Points to first word read from input*/
char *lastword; /*Points to last word read from input*/
int background; /*set if & is lastword, used to background process*/

/*Storage * Maxitem because each word is a max size of storage and
each line has a max word count of maxtitem therefore, the biggest
possible line is (storage * maxitem) (+ 1 for null terminator)*/
char line[(STORAGE * MAXITEM) + 1]; /*Stores line read from input*/
char *lineptr = &line[0]; /*Used to cycle through each word*/
char *word[MAXITEM]; /*Used to mark the start of each word per line*/

char *inptr; /*points to an input redirect received in input line*/
char *infile; /*points to filename for input*/
char *outptr; /*points to an output redirect received in input line*/
char *outfile; /*Points to filename for output*/
char *pipeptr; /*points to a pipe received in input line*/

char *newargv[MAXARGS]; /*used to send args to children*/
int newargc; /*counts number of args sent to children*/

/*Main prompts for input, handles EOF, handles creating new
processes, handles redirection and kills children.
Exits with code 0 if no errors.*/
int main(){
	/*dont want to kill ourself! send SIGTERM to a handler*/
	signal(SIGTERM, sighandler);

	for(;;){
		prompt();
		parse();
		if( /* *firstword == EOF */ c == EOF )
			break;
		if( numwords == 0 )
			continue;
		else{
			if( firstword == NULL ){
				(void) printf("NULL Command not found.\n");
				continue;
			}/*handle the cd command*/
			else if( (strcmp(firstword, "cd")) == 0 ){
				/*Make sure cd has only 1 arg*/
				if( newargc > 2 ){
					(void) printf("cd received too many arguments\n");
					continue;
				}/*If cd has no arguments, set its path to $HOME*/
				else if (newargv[1] == NULL){
					/*make sure HOME is defined*/
					if ( (getenv("HOME")) == NULL ){
						(void) printf("HOME variable not defined.\n");
					}else{
						chdir(getenv("HOME"));
					}
				}else if( (chdir(newargv[1])) == -1 ){
					(void) printf("No such file or directory.\n");
				}
			}
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
			int kidpid;
			if( (kidpid = fork()) == -1 ){
				perror("Unable to fork.\n");
				exit (1);
			}else if( kidpid == 0 ){
				if( (execvp(newargv[0], newargv)) == -1 ){
					(void) printf("Command not found.\n");
					exit(2);
				}
			}else{
				/*background handler*/
				if ( (strcmp(lastword, "&")) == 0/*background*/ ){
					(void) printf("%s [%d]\n", newargv[0], kidpid);
					continue;
				}else{
					for(;;){
						int pid;
						CHK(pid = wait(NULL));
						if (pid == kidpid){
							break;
						}
					}
				}
			}
		}
	}
	killpg(getpid(), SIGTERM);
	(void) printf("p2 terminated.\n");
	exit(0);
}

/*Issues the prompt "p2: " prompting user for input*/
void prompt(){
	(void)printf("p2: ");
}

/*Parse cycles through the input received from stdin by calling
getword.
Parse sets flags when metacharacters are encountered.*/
void parse(){
	numwords = 0;
	newargc = 0;
	firstword = NULL;
	lastword = NULL;
	inptr = infile = outfile = outptr = pipeptr = NULL;
	background = 0;
	*lineptr = &line;
	/*this loop adds words to the line buffer until c is EOF
	or 0 (meaning getword hit s newline)*/
	for(;;){
		c = getword(s);
		if( c == EOF )
			break;
		else if( c == 0 )
			break;
		else{
			if( numwords+1 == MAXITEM ){
				(void) printf("Too many args.\n");
				break;
			}
			*(word + numwords) = lineptr;
			int i;
			for( i=0; i<c; i++){
				*lineptr++ = s[i];
				*lineptr = '\0';
			}
			*lineptr++ = '\0';
			numwords++;
		}
	}

	/*this loop cycles through each word in the line and sets
	appropriate flags
	it also does error checking*/
	int i;
	for ( i=0; i < numwords; i++){
		if ( (strcmp( word[i], "<")) == 0 ){
			if ( inptr != NULL ){
				printf ("Error: Ambiguous input\n");
				break;
			}else{
				inptr = word[i];
				lastword = word[i];
				if( i+1 < MAXITEM ){
					infile = word[++i];
					lastword = word[i];
				}
			}
		}else if ( (strcmp( *(word + i), ">")) == 0 ){
			if ( outptr != NULL ){
				printf ("Error: too many output files\n");
				break;
			}else{
				outptr = word[i];
				lastword = word[i];
				if( i+1 < MAXITEM ){
					outfile = word[++i];
					lastword = word[i];
				}
			}
		/*}else if ( (strcmp( *(word + i), "$")) == 0 ){
			;*/
		/*}else if ( (strcmp( *(word + i), "&")) == 0 ){
			;*/
		}else if ( (strcmp( *(word + i), "|")) == 0 ){
			if ( pipeptr != NULL ){
				printf ("Error: Too many pipes!\n");
				break;
			}else{
				pipeptr = word[i];
				lastword = word[i];
			}
		}else{
			if( firstword == NULL ){
				firstword = word[i];
			}
			newargv[newargc++] = word[i];
			newargv[newargc] = '\0';
			lastword = word[i];
		}
	}

	/*if ( (strcmp(newargv[newargc-1], "&")) == 0 ){
		background = 1;
		newargv[newargc-1] = NULL;
	}*/
}

/*catches the SIGTERM signal to avoid killing p2*/
void sighandler(){
	;
}
