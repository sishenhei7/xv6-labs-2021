#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

//MAXARG

int
main(int argc, char *argv[]) {
  if(argc < 2){
    fprintf(2, "Usage: xargs <command> ...\n");
    exit(1);
  }

  char ch, buf[2048], *args[MAXARG];

  for (int m = 1; m < argc; m++) {
    args[m - 1] = argv[m];
  }

  char *p = buf;
  int i = argc - 1;
  int n = 0;

  while (read(0, &ch, 1) > 0) {
    if (ch == ' ' || ch == '\n') {
      args[i++] = p;
      buf[n++] = 0;
      p = buf + n;
    } else {
      buf[n++] = ch;
    }

    if (ch == '\n') {
      if (fork()) {
        wait(0);
        i = argc - 1;
      } else {
        exec(args[0], args);
        exit(0);
      }
    }
  }
  exit(0);
}
