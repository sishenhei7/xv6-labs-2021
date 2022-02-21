#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
primes(int fd) {
  int prime;

  if (read(fd, &prime, sizeof(prime))) {
    printf("prime: %d\n", prime);

    int p[2];
    pipe(p);

    if (fork()) {
      close(p[0]);

      int i;
      while(read(fd, &i, sizeof(i))) {
        if (i % prime != 0) {
          write(p[1], &i, sizeof(i));
        }
      }

      close(p[1]);
      wait(0);
    } else {
      close(p[1]);
      primes(p[0]);
    }
  }
}

int
main(int argc, char *argv[]) {
  int p[2];

  pipe(p);

  if (fork()) {
    close(p[0]);

    int i;
    for (i = 2; i <= 35; i += 1) {
      write(p[1], &i, sizeof(i));
    }

    close(p[1]);
    wait(0);
  } else {
    close(p[1]);
    primes(p[0]);
  }

  exit(0);
}


