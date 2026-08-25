#include "user.h"

void main(void)
{
  uint64 vamemfree = vamemavail();
  printf("Available virtual address memory: %ld\n", vamemfree);

  uint64 memfree = memavail();
  printf("Available physical address memory: %ld\n", memfree);
}
