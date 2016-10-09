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

/*Storage * Maxitem because each word is a max size of storage and
each line has a max word count of maxtitem therefore, the biggest
possible line is (storage * maxitem) (+ 1 for null terminator)*/
char line[(STORAGE * MAXITEM) + 1]; /*Stores line read from input*/
char *lineptr = &line[0]; /*Used to cycle through each word*/
char *word[MAXITEM]; /*Used to mark the start of each word per line*/

char *inptr; /*points to an input redirect received in input line*/
char *outptr; /*points to an output redirect received in input line*/
char *pipeptr; /*points to a pipe received in input line*/

char *newargv[MAXARGS]; /*used to send args to children*/

/*Main prompts for input, handles EOF, handles creating new
processes, handles redirection and kills children.
Exits with code 0 if no errors.*/
int main(){
	/*dont want to kill ourself! send SIGTERM to a handler*/
	signal(SIGTERM, SIG_IGN);

	for(;;){
		prompt();
		parse();
		if( c == EOF )
			break;
		if( numwords == 0 )
			continue;
		else{
			if( firstword == NULL ){
				printf("Command not found\n");
			}else if( (strcmp(firstword, "cd")) == 0 ){
				;
			}else{
				int kidpid;
				if( (kidpid = fork()) == -1 ){
					perror("Unable to fork");
					exit (1);
				}else if( kidpid == 0 ){
					newargv[0] = firstword;
					newargv[MAXARGS + 1] = NULL;
					CHK(execvp(firstword, newargv));
				}else{
					/*background handler*/
					;
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
	firstword = NULL;
	lastword = NULL;
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
			inptr = word[i];
			lastword = word[i];
		}else if ( (strcmp( *(word + i), ">")) == 0 ){
			outptr = word[i];
			lastword = word[i];
		}else if ( (strcmp( *(word + i), "$")) == 0 ){
			;
		}else if ( (strcmp( *(word + i), "&")) == 0 ){
			;
		}else if ( (strcmp( *(word + i), "|")) == 0 ){
			pipeptr = word[i];
			lastword = word[i];
		}else{
			if ( firstword == NULL ){
				if ( lastword == NULL ){
					firstword = word[i];
				}
				else if ( (strcmp( lastword, "<") == 0) ||
				 	(strcmp( lastword, ">") == 0))
					;
				else{
					firstword = word[i];
				}
			}
			lastword = word[i];
		}
	}
}

/*catches the SIGTERM signal to avoid killing p2*/
void sighandler(){
	;
}
