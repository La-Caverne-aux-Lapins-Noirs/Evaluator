/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_POSIX_C_SOURCE		200809L
#include		<setjmp.h>
#include		<signal.h>
#include		<unistd.h>
#include		<assert.h>
#include		"technocore.h"

extern sigjmp_buf	gl_before_test;
extern volatile sig_atomic_t gl_before_test_ready;

int			main(void)
{
  int			ret;

  alarm(1);
  if ((ret = sigsetjmp(gl_before_test, 1)) == 0)
    {
      gl_before_test_ready = 1;
      sighandler(1337);
    }
  else
    assert(ret == 1337);
  gl_before_test_ready = 0;
  assert(alarm(0) == 0);
  return (EXIT_SUCCESS);
}

