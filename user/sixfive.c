#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
char token[64];
const char *pattern = " -\r\t\n./,";

int main(int argc, char *argv[])
{
    int fd, n;
    int tlen = 0; // 当前 token 长度

    if (argc < 2)
    {
        fprintf(2, "Usage: sixfive [file...]\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++)
    {
        fd = open(argv[i], O_RDONLY);
        if (fd < 0)
        {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }

        while ((n = read(fd, buf, sizeof(buf))) > 0)
        {
            for (int i = 0; i < n; i++)
            {
                char c = buf[i];
                if (c >= '0' && c <= '9')
                {
                    // 累加数字字符
                    if (tlen < sizeof(token) - 1)
                    {
                        token[tlen] = c;
                        tlen++;
                    }
                }
                else if (strchr(pattern, c))
                {
                    // 碰到分隔符，处理 token
                    if (tlen > 0)
                    {
                        token[tlen] = '\0';
                        int num = atoi(token);
                        if (num % 5 == 0 || num % 6 == 0)
                            printf("%d\n", num);
                        tlen = 0;
                    }
                }
                else
                {
                    // 非数字且非分隔符（例如字母）
                    // 一样终止当前 token
                    if (tlen > 0)
                    {
                        token[tlen] = '\0';
                        int num = atoi(token);
                        if (num % 5 == 0 || num % 6 == 0)
                            printf("%d\n", num);
                        tlen = 0;
                    }
                }
            }
        }
        // 文件结束后若还有未处理的数字
        if (tlen > 0)
        {
            token[tlen] = '\0';
            int num = atoi(token);
            if (num % 5 == 0 || num % 6 == 0)
                printf("%d\n", num);
        }

        close(fd);
    }

    exit(0);
}