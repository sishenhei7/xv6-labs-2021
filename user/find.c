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

  if (st.type == T_FILE) {
    fprintf(2, "find: path isn't a dir!\n");
    close(fd);
    return;
  }

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(stat(path, &st) < 0){
      printf("ls: cannot stat %s\n", path);
      continue;
    }

    switch(st.type){
      case T_FILE:
        if (strcmp(file_name, de.name)) {
          printf("&s\n", de.name);
        }
        break;
      case T_DIR:
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        memmove(p, de.name, DIRSIZ);
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
