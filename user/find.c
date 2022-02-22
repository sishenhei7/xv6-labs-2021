#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
find(char *path, char *name) {
  int fd;
  struct stat st;

  if ((fd = read(&path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type) {
    case T_FILE:
      break;
    case T_DIR:
      break;
  }

  close(fd)
}

int
main(int argc, char *argv[]) {
  if(argc != 3){
    fprintf(2, "Usage: find path name ...\n");
    exit(1);
  }

  find(argv[1], argv[2])
  exit(0);
}
