#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  int parent_fd[2], child_fd[2];
  char parent_buf[1], child_buf[1];

  if(argc > 1){
    fprintf(2, "Usage: sleep ...\n");
    exit(1);
  }

  pipe(parent_fd);
  pipe(child_fd);

  if (fork() == 0) {
    close(parent_fd[1]);
    close(child_fd[0]);
    if (read(parent_fd[0], child_buf, 1) == 1) {
      fprintf(1, "%d: received ping\n", getpid());
      write(child_fd[1], "q", 1);
      close(child_fd[1]);
    }
  } else {
    close(parent_fd[0]);
    close(child_fd[1]);
    write(parent_fd[1], "p", 1);
    if (read(child_fd[0], parent_buf, 1) == 1) {
      fprintf(1, "%d: received pong\n", getpid());
      close(parent_fd[1]);
    }
  }

  exit(0);
}
