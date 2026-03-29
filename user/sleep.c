#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
	int ms;

	if (argc != 2)
	{
		fprintf(2, "Usage: sleep [seconds]\n");
		exit(1);
	}
	ms = atoi(argv[1]);
	if (ms == 0)
	{
		fprintf(2, "Invaild param [seconds] should be a number\n");
		exit(1);
	}
	pause(ms * 10);
	exit(0);
}