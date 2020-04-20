#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "gst.h"
#include "rbt.h"
#include "string.h"
#include "scanner.h"

int ProcessOptions(int,char **);
void Fatal(char *,...);
void processCommands(void);
void makeTree(void);
char *getStr(FILE *fp);
char *cleanString(char *string);
char *cleanWithSpaces(char *string);

FILE *GSTcommands = NULL;
FILE *GSTcorpus = NULL;
GST *gstTree = NULL;
FILE *RBTcommands = NULL;
FILE *RBTcorpus = NULL;
RBT *rbtTree = NULL;

int main(int argc, char **argv) {
	if (argc == 1) Fatal("%d arguments!\n",argc-1);

	ProcessOptions(argc,argv);
	// printf("Starting makeTree\n");
	makeTree();
	// printf("Starting processCommands\n");
	processCommands();

	if(rbtTree != NULL){
		freeRBT(rbtTree);
	}
	if(gstTree != NULL){
		freeGST(gstTree);
	}

	return 0;
}

void
Fatal(char *fmt, ...)
    {
    va_list ap;

    fprintf(stderr,"An error occured: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    exit(-1);
    }

/* only -oXXXX  or -o XXXX options */

int
ProcessOptions(int argc, char **argv)
    {
    int start,argIndex;
    int argsUsed;
    char *arg;

    argIndex = 1;

    while (argIndex < argc && *argv[argIndex] == '-')
        {
        /* check if stdin, represented by "-" is an argument */
        /* if so, the end of options has been reached */
        if (argv[argIndex][1] == '\0') return argIndex;

        argsUsed = 0;
        start = argIndex;


        /* advance argIndex to point to the first argument to the option */
        if (argv[start][2] == '\0')
            {
            arg = argv[start+1];
            ++argIndex;
            }
        else
            {
            /* first arg is connected to option, so don't advance */
            arg = argv[start]+2;
            }

        switch (argv[start][1])
            {
            case 'v':
                printf("Written by Andrew Duerr\n");
                break;
            case 'g':	;
								// printf("Making green tree.\n");
								gstTree = newGST(compareSTRING);
								// printf("Corpus is %s, Command is %s\n",arg,argv[argIndex+1]);
								GSTcorpus = fopen(arg,"r");
								GSTcommands = fopen(argv[argIndex+1],"r");
                argsUsed = 2;
                break;
            case 'r':	;
								// printf("Making red black tree.\n");
								rbtTree = newRBT(compareSTRING);
								// printf("Corpus is %s, Command is %s\n",arg,argv[argIndex+1]);
								RBTcorpus = fopen(arg,"r");
								RBTcommands = fopen(argv[argIndex+1],"r");
								argsUsed = 2;
                break;
            default:
                fprintf(stderr,"option %s not understood\n",argv[start]);
                exit(-1);
            }

        argIndex += argsUsed;
        }

    return argIndex;
    }

		void processCommands(void){
			char *string = NULL;
			char *temp = NULL;
			char x;
			STRING *s = NULL;
			if(rbtTree != NULL){
				// printf("PC: inside rbtTree case\n");
				string = getStr(RBTcommands);
				x = string[0];
				while(!feof(RBTcommands)){
					switch(x){
						case 's':
							if(sizeRBT(rbtTree) > 0){
								displayRBT(rbtTree,stdout);
							}
							else{
								fprintf(stdout,"EMPTY\n");
							}
							break;
						case 'r':
							statisticsRBT(rbtTree,stdout);
							break;
						case 'i':
							temp = getStr(RBTcommands);
							if(strlen(temp) > 0){
								insertRBT(rbtTree,newSTRING(temp));
							}
							break;
						case 'd':
							temp = getStr(RBTcommands);
							s = newSTRING(temp);
							if(strlen(temp) > 0){
								if(locateRBT(rbtTree,s) == NULL){
									fprintf(stdout,"Value %s not found.\n",temp);
								}
								else{
									deleteRBT(rbtTree,s);
									freeSTRING(s);
								}
							}
							break;
						case 'f':
							temp = getStr(RBTcommands);
							s = newSTRING(temp);
							if(strlen(temp) > 0){
								if(locateRBT(rbtTree,s) == NULL){
									fprintf(stdout,"Value %s not found.\n",temp);
								}
								else{
									fprintf(stdout,"Frequency of %s: %d\n",temp,freqRBT(rbtTree,s));
								}
							}
							freeSTRING(s);
							break;
						default :
							fprintf(stderr,"Bad command: [%c]\n",x);
							break;
					}
					string = getStr(RBTcommands);
					x = string[0];
				}
			}
			if(gstTree != NULL){
				// printf("PC: Inside gstTree case\n");
				string = getStr(GSTcommands);
				temp = NULL;
				x = string[0];
				s = NULL;
				while(!feof(GSTcommands)){
					switch(x){
						case 's':
							// printf("Command is -s\n");
							if(sizeGST(gstTree) > 0){
								displayGST(gstTree,stdout);
							}
							else{
								fprintf(stdout,"EMPTY\n");
							}
							break;
						case 'r':
							// printf("Command is -r\n");
							statisticsGST(gstTree,stdout);
							break;
						case 'i':
							// printf("Command is -i\n");
							temp = getStr(GSTcommands);
							if(strlen(temp) > 0){
								insertGST(gstTree,newSTRING(temp));
							}
							break;
						case 'd':
							// printf("Command is -d\n");
							temp = getStr(GSTcommands);
							s = newSTRING(temp);
							if(strlen(temp) > 0){
								if(locateGST(gstTree,s) == NULL){
									fprintf(stdout,"Value %s not found.\n",temp);
								}
								else{
									deleteGST(gstTree,s);
									freeSTRING(s);
								}
							}
							break;
						case 'f':
							// printf("Command is -f\n");
							temp = getStr(GSTcommands);
							s = newSTRING(temp);
							if(strlen(temp) > 0){
								if(locateGST(gstTree,s) == NULL){
									fprintf(stdout,"Value %s not found.\n",temp);
								}
								else{
									fprintf(stdout,"Frequency of %s: %d\n",temp,freqGST(gstTree,s));
								}
							}
							freeSTRING(s);
							break;
						default :
							fprintf(stderr,"Bad command: [%c]\n",x);
							break;
					}
					string = getStr(GSTcommands);
					x = string[0];
				}
			}
		}

void makeTree(void){
	char *temp = NULL;
	if(rbtTree != NULL){
		// printf("making red black tree...\n");
		setRBTdisplay(rbtTree,displaySTRING);
		setRBTfree(rbtTree,freeSTRING);
		temp = getStr(RBTcorpus);
		while(!feof(RBTcorpus)){
			if(strlen(temp) > 0){
				insertRBT(rbtTree,newSTRING(temp));
			}
			temp = getStr(RBTcorpus);
		}
	}
	if(gstTree != NULL){
		// printf("making green tree...\n");
		setGSTdisplay(gstTree,displaySTRING);
		setGSTfree(gstTree,freeSTRING);
		temp = getStr(GSTcorpus);
		while(!feof(GSTcorpus)){
			if(strlen(temp) > 0){
				insertGST(gstTree,newSTRING(temp));
			}
			temp = getStr(GSTcorpus);
		}
	}
}

char *getStr(FILE *fp){
	// printf("Calling getStr\n");
	char x = readChar(fp);
	ungetc(x,fp);
	if(x == EOF) {
		return "";
	}
	else if(x == '"'){
		return cleanWithSpaces(readString(fp));
	}
	else{
		return cleanString(readToken(fp));
	}
}

char *cleanString(char *string){
	// printf("Calling cleanString\n");
	char *new = malloc(sizeof(strlen(string))+1);
	char ch;
	int length = strlen(string);
	int index = 0;
	for(int i = 0; i < length; i++){
		ch = string[i];
		if(isalpha(ch)) {
			if(isupper(ch)) {
				ch = tolower(ch);
			}
			new[index] = ch;
			index++;
		}
	}
	new[index] = '\0';
	char *final = realloc(new,index+1);
	return final;
}

char *cleanWithSpaces(char *string){
	// printf("Calling cleanWithSpaces\n");
	char *new = malloc(sizeof(strlen(string)));
	char ch;
	int length = strlen(string);
	int index = 0;
	for(int i = 0; i < length; i++){
		ch = string[i];
		if(ch == ' ' && index != 0){
			if(new[index-1] != ' '){
				new[index] = ch;
				index++;
			}
		}
		else if(islower(ch)){
			new[index] = ch;
			index ++;
		}
		else if(isupper(ch)){
			new[index] = tolower(ch);
			index++;
		}
	}
	char m = new[index-1];
	while(m == ' '){
		index -= 1;
		m = new[index-1];
	}
	new[index] = '\0';
	char *final = realloc(new,index+1);
	if(strcmp(final," ") == 0){
		final = "";
	}
	return final;
}
