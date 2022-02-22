#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  if(argc != 3){
    fprintf(2, "Usage: find path name ...\n");
    exit(1);
  }

  fprintf(1, "Usage: find path name ...\n");
  exit(0);
}
