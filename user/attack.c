#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  sbrk(PGSIZE * 8);
  char *end = sbrk(PGSIZE);
  char* secret = end + 48;

  write(2, secret, 8);

  exit(0);
}
