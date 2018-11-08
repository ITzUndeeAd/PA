#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[])
{	
	/* Silencia os warnings */
	(void)argc;
	(void)argv;

        int a,b,c,d;
        a = b = c = d = -1;
        a = fork();
        if (a == 0)
            b = fork();
        c = fork();
        if (b == 0 && a > 0)
            d = fork();

        printf("Processo %d (%d) ", getpid(), getppid());
        printf("a = %d b = %d c = %d d = %d\n",a, b, c, d);

	return 0;
}
