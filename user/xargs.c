#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{

	if (argc < 2)
	{
		fprintf(2, "xargs <exec> <args..>\n");
		exit(1);
	}
	char *args[MAXARG + 1];
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	for (int i = 1; i < argc; i++)
	{
		args[i - 1] = argv[i];
	}
	char *p = buf;
	while (read(0, p, sizeof(char)))
	{
		if (*p == '\n')
		{
			*p = '\0';
			args[argc] = 0;
			args[argc - 1] = (char *)buf;
			if (fork() == 0)
			{
				exec(argv[1], args);
				fprintf(2, "exec %s failed\n", argv[0]);
				exit(1);
			}
			else
			{
				p = buf;
				wait(0);
				continue;
			}
		}
		p++;
	}
	exit(0);
}