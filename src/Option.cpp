#include "Option.h"
#include <cstring>
#include <cstdio>
using namespace std;


bool getopt(int argc, char** argv, char* key, bool isRequired)
{
	for(int i = 1 ; i < argc ; i++)
	{
		if(strcmp(argv[i],key) == 0 )	return true;
	}
	if(isRequired)option_warning(key);
	return false;

}
bool getopt(int argc, char** argv, char* key, int* value, bool isRequired)
{
	for(int i = 1 ; i < argc -1; i++)
	{
		if(strcmp(argv[i],key) == 0 )
		{
			*value =atoi(argv[i+1]);
			return true;
		}
	}
	
	if(isRequired)option_warning(key);
	return false;
}
bool getopt(int argc, char** argv, char* key, double* value, bool isRequired)
{
	for(int i = 1 ; i < argc -1; i++)
	{
		if(strcmp(argv[i],key) == 0 )
		{
			*value =atof(argv[i+1]);
			return true;
		}
	}
	
	if(isRequired)option_warning(key);
	return false;
}
bool getopt(int argc, char** argv, char* key, char* value, bool isRequired)
{
	for(int i = 1 ; i < argc -1; i++)
	{
		if(strcmp(argv[i],key) == 0 )
		{
			strcpy(value,argv[i+1]);
			return true;
		}
	}
	
	if(isRequired)option_warning(key);
	return false;
}

void option_warning(char *key)
{
	fprintf(stderr,"Warning : parameter %s is not found.\n",key);
}
