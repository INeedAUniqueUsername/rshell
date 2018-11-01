#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main()
{
   pid_t pid;
   char *const parmList[] = 
   {(char*)"-l",(char*) "-a", NULL};

   if ((pid = fork()) == -1) {
	perror("fork() error");
	}
   else if (pid == 0) {
	execvp("ls", parmList);
        printf("Return not expected. Must be an execvp() error.\\n");
   }
}
