
#include <stdio.h>
#include <string.h>

int my_function(const char *);

int main(void)
{
  int x = my_function("lel");

  x += strlen("lol");
  printf("%d\n", x);
  return (x);
}

