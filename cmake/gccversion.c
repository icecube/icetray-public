#include <stdio.h>
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

int main(int argc, char** argv)
{
  printf("%d", GCC_VERSION);
  return 0;
}

