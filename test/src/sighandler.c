/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<setjmp.h>
#include		<assert.h>
#include		"technocore.h"

extern jmp_buf		gl_before_test;

int			main(void)
{
  int			ret;

  alarm(1);
  if ((ret = setjmp(gl_before_test)) == 0)
    sighandler(1337);
  else
    assert(ret == 1337);
  assert(alarm(0) == 0);
  return (EXIT_SUCCESS);
}

