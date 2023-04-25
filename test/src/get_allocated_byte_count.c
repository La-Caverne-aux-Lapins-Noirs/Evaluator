/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  size_t		total;
  int			add;
  int			i;

  return (EXIT_SUCCESS);
  // LCOV_EXCL_START
  if (bunny_malloc == malloc)
    return (EXIT_SUCCESS);
  total = 0;
  rseed();
  for (i = 0; i < 50; ++i)
    {
      add = (rand() % 256 + 1) * 64;
      bunny_malloc(add);
      total += add;
      assert(total == get_allocated_byte_count());
    }
  return (EXIT_SUCCESS);
  // LCOV_EXCL_STOP
}

