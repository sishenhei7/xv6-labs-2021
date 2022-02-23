#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, const char *file_name) {
  int fd;
  char buf[512], *p;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    fprintf(2, "find: %s isn't a dir!\n", path);
    close(fd);
    return;
  }

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
			continue;

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }

    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    memmove(p, de.name, DIRSIZ);

    if(stat(buf, &st) < 0){
      printf("find: cannot stat %s\n", path);
      continue;
    }

    switch(st.type){
      case T_FILE:
        if (!strcmp(file_name, de.name)) {
          printf("%s\n", buf);
        }
        break;
      case T_DIR:
        find(buf, file_name);
        break;
    }
  }

  close(fd);
}

int
main(int argc, char *argv[]) {
  if(argc != 3){
    fprintf(2, "Usage: find path file_name ...\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}
