#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
primes(int fd) {
  int num, p[2], prime, buf, pid;

  while(read(fd, num, 1)) {
    if (!prime) {
      prime = num
      printf(prime)
    } else if (num % prime != 0) {
      write(p[1], i, 1)

      if (!pid && !(pid = fork())) {
        primes(p[0])
      }
    }
  }
}

int
main(int argc, char *argv[]) {
  int i, p[2], prime, buf;

  if (fork()) {
    for (i = 2; i <= 35; i += 1) {
      write(p[1], i, 1)
    }
    wait(void 0)
    exit(0)
  } else {
    primes(p[0])
  }
}


