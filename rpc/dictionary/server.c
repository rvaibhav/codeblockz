#include <stdio.h>
#include "dictionary.h"

#define MAX_WORDS 10000
#define MAXLENGTH 10000

char * words[MAX_WORDS];
char * meanings[MAX_WORDS];

static int count = 0;

char *strdu(char *);

resultlong * insert_1_svc(dictionary *in,struct svc_req *rqst)
{
	static resultlong j;
	
	int i;
	
	printf("value of count is: %d\n",count);
	for(i=1;i<=count;i++)
	{	
		if(strcmp(words[i],in->word) == 0)
		{
			printf("\nWord already present in dictionary:%s\n\n",in->word);
			j.success=1;			
			return &j;
		}
	}
	count++;
	char *word = strdu(in->word);
	char *meaning = strdu(in->meaning);
	words[count] = word;
	meanings[count] = meaning;
	for(i=1;i<=count;i++)
	{
		printf("%s --- %s\n",words[i],meanings[i]);
	}
	printf("\nInserting new word:%s with meaning:%s\n",in->word,in->meaning);
	j.success=0;
	return &j;
}

lookupword * lookup_1_svc(dictionary *in, struct svc_req *rqstp)
{
	static lookupword result;
	int i;
	for(i=1;i<=count;i++)
	{
		if(strcmp(words[i],in->word) == 0)
		{
			result.word = words[i];
			result.meaning = meanings[i];
			return &result;
		}
	}
	result.word = "NoWord";
	result.meaning = "No Meaning";
	return &result;
}

resultlong * deleteword_1_svc(dictionary *in, struct svc_req *rqstp)
{
	static resultlong result;

	int i;
	int found,position;
	found = 0;
	
	for(i=1;i<=count;i++)
	{
		if(strcmp(words[i],in->word) == 0)
		{
			found = 1;
			position = i;
		}
	}
	if(found == 1){
		for(i=position;i<count;i++)
		{
			char *shiftWord = strdu(words[i+1]);
			char *shiftMeaning = strdu(meanings[i+1]);
			words[i] =  shiftWord;
			meanings[i] = shiftMeaning;
		}
		count--;
		result.success = 1;
		printf("Successfully deleted %s from dictionary\n",in->word);
	}else{
		result.success = 0;	
	}
	for(i=1;i<=count;i++)
	{
		printf("%s---%s\n",words[i],meanings[i]);
	}
	return &result;
}

char *strdu(char *s)
{
    char *p;
    p = (char *) malloc(strlen(s)+1); /* +1 for ’\0’ */
    if (p != NULL)
       strcpy(p, s);
    return p;
}	
