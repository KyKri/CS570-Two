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

#define SPACE ' '
#define NEWLINE '\n'
#define LSSR '<'
#define GRTR '>'
#define PIPE '|'
#define DLLR '$'
#define AMP '&'
#define BACK '\\'

void parse();
void prompt();

int c; /*Number of characters from input line*/
char s[STORAGE]; /*Used to store input line*/

/*Main prompts for input, handles EOF, handles creating new
processes, handles redirection and kills children.
Exits with code 0 if no errors.*/
int main(){
	for(;;){
		prompt();
		parse();
		if( c == EOF )
			break;
		if( c == 0 )
			continue;
	}
	printf("p2 terminated\n");
	exit(0);
}

/*Parse cycles through the input received from stdin by calling
getword.
Parse sets flags when metacharacters are encountered.*/
void parse(){
	c = getword(s);
}

/*Issues the prompt "p2: "*/
void prompt(){
	(void)printf("p2: ");
}
