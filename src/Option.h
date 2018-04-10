#ifndef __OPTION__
#define __OPTION__ 1
#include <cstdlib>
bool getopt(int argc, char** argv, char* key, bool isRequired = false);
bool getopt(int argc, char** argv, char* key, int* value, bool isRequired = false);
bool getopt(int argc, char** argv, char* key, double* value, bool isRequired = false);
bool getopt(int argc, char** argv, char* key, char* value, bool isRequired = false);

void option_warning(char* key);


#endif //__OPTION__