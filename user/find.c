#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"

#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int find_file(const char *path, const char *target,
              char **exec_argv, int exec_argc)
{
  char buf[512];
  char *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return -1;
  }
  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return -1;
  }
  if (st.type != T_DIR)
  {
    close(fd);
    return 0;
  }
  strcpy(buf, path);
  p = buf + strlen(buf);
  if (*(p - 1) != '/')
  {
    *p++ = '/';
  }
  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;
    if (strlen(path) + 1 + strlen(de.name) + 1 > sizeof(buf))
    {
      fprintf(2, "find: path too long\n");
      continue;
    }
    strcpy(p, de.name);
    if (strcmp(de.name, target) == 0)
    {
      if (exec_argv == 0)
      {
        printf("%s\n", buf);
      }
      else
      {
        int pid = fork();
        if (pid == 0)
        {
          // child

          char *args[MAXARG];
          int i;

          for (i = 0; i < exec_argc; i++)
          {
            args[i] = exec_argv[i];
          }

          args[i++] = buf;
          args[i] = 0;

          exec(args[0], args);
          fprintf(2, "exec failed\n");
          exit(1);
        }
        else
        {
          wait(0);
        }
      }
      continue;
    }

    if (stat(buf, &st) < 0)
    {
      fprintf(2, "find: stat %s failed \n", buf);
      continue;
    }
    if (st.type == T_DIR)
    {
      find_file(buf, target, exec_argv, exec_argc);
    }
  }
  close(fd);
  return 0;
}

// Returns 0 if the given path is a directory, > 0 if it is not a directory,
// and -1 in case of an error (e.g., cannot open or stat the path).
int is_dir(const char *path)
{

  struct stat st;
  if (stat(path, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    return -1;
  }
  // Return 0 if it's a directory, otherwise > 0
  return st.type == T_DIR ? 0 : 1;
}

int main(int argc, char *argv[])
{
  int exec_idx = -1;

  if (argc < 3)
  {
    fprintf(2, "Usage: find path filename [-exec cmd ...]\n");
    exit(1);
  }
  for (int i = 3; i < argc; i++)
  {
    if (strcmp(argv[i], "-exec") == 0)
    {
      exec_idx = i;
      break;
    }
  }

  // Check if the path is a directory
  if (is_dir(argv[1]) != 0)
  {
    fprintf(2, "%s is not a directory or cannot be accessed\n", argv[1]);
    exit(1);
  }

  // Start searching for the target file
  if (exec_idx == -1)
  {
    find_file(argv[1], argv[2], 0, 0);
  }
  else
  {
    find_file(argv[1], argv[2],
              &argv[exec_idx + 1],
              argc - exec_idx - 1);
  }

  exit(0);
}