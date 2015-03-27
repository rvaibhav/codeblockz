#include <stdio.h>
#include "dictionary.h"
#include "string.h"

#define MAX_WORDS 10000

char * words[MAX_WORDS];
char * meanings[MAX_WORDS];

static int count = 0;

void lookupword_call(CLIENT *,char *);
void insert_call(CLIENT *,char *);
void deleteword_call(CLIENT *,char *);

int main(int argc, char **argv)
{
	CLIENT *cl;
	dictionary in;
	resultlong *outp;
	if(argc != 2)
	{
		printf("usage: client <hostname>\n");
		exit(1);
	}
	cl = clnt_create(argv[1], DICTIONARY_PROG, DICTIONARY_VERS, "tcp");
	if(cl == NULL){
		clnt_pcreateerror(argv[1]);
       		exit(2);	
	}
	int j;
	int choice;
	while(1)
	{
		printf("1. Insert a word\n");
		printf("2. Lookup a word\n");
		printf("3. Delete a word\n");
		printf("Enter your choice:");
		scanf("%d",&choice);
		printf("\n");
		printf("Choice entered is %d\n",choice);
		if(choice == 1){
			insert_call(cl,argv[1]);
		}else if(choice == 2){
			lookupword_call(cl,argv[1]);	
		}else if(choice == 3){
			deleteword_call(cl,argv[1]);	
		}else{
			printf("Invalid choice. Please try again!\n");		
		}
			
	}
	exit (0) ;
}

void lookupword_call(CLIENT *cl, char * hostname){
	dictionary in;
	lookupword *lookup;
	int j;
	char word[100];
	printf("\nEnter the word to lookup:");
	scanf("%s",word);
	j = strlen(word)-1;
	if(word[j] == '\n') 
      		word[j] = '\0';
	in.word = word;
	in.meaning = "something";
	if ((lookup=lookup_1(&in, cl))==NULL)
			printf("%s", clnt_sperror(cl, hostname));
	char * noword = "NoWord";
	if(strcmp(noword,lookup->word) == 0){
		printf("%s --- Not found in dictionary\n\n",word);	
	}else{
		printf("%s --- %s\n\n",lookup->word,lookup->meaning);
	}
}

void insert_call(CLIENT *cl, char * hostname){
	char word[100];
	char meaning[1000];
	dictionary in;
	resultlong *outp;
	int j;
	printf("\nEnter the word:");
	scanf("%s",word);
	j = strlen(word)-1;
	  if(word[j] == '\n') 
		word[j] = '\0';

	printf("Enter the meaning:");
	scanf("%s",meaning);
	j = strlen(meaning)-1;
	  if(meaning[j] == '\n') 
		meaning[j] = '\0';
	in.word = word;
	in.meaning = meaning;
	if ((outp=insert_1(&in, cl))==NULL)
		printf("%s", clnt_sperror(cl, hostname));

	if (outp->success == 1)
	{	
		printf("Word already exists in the dictionary\n\n");
	}
	else
	{
		printf("Word successfully inserted in the dictionary\n\n");
	}
}
void deleteword_call(CLIENT *cl, char * hostname){
	dictionary in;
	resultlong *outp;
	int j;
	char word[100];
	printf("\nEnter the word to delete:");
	scanf("%s",word);
	j = strlen(word)-1;
	if(word[j] == '\n') 
      		word[j] = '\0';
	in.word = word;
	in.meaning = "something";
	if ((outp=deleteword_1(&in, cl))==NULL)
			printf("%s", clnt_sperror(cl, hostname));
	if (outp->success == 1)
	{	
		printf("%s successfully deleted from dictionary!\n\n",word);
	}
	else
	{
		printf("%s not available in the dictionary!\n\n",word);
	}	
}
