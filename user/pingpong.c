#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  int parent_fd[2], child_fd[2];
  char buf[1];

  if(argc > 1){
    fprintf(2, "Usage: pingpong ...\n");
    exit(1);
  }

  pipe(parent_fd);
  pipe(child_fd);

  if (fork()) {
    write(parent_fd[1], "p", 1);
    read(child_fd[0], buf, 1);
    fprintf(1, "%d: received pong\n", getpid());
  } else {
    read(parent_fd[0], buf, 1);
    fprintf(1, "%d: received ping\n", getpid());
    write(child_fd[1], "q", 1);
  }

  exit(0);
}
