/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_POSIX_C_SOURCE		2023
#include		<unistd.h>
#include		<setjmp.h>
#include		<signal.h>

extern sigjmp_buf	gl_before_test;
extern volatile sig_atomic_t gl_before_test_ready;

void			sighandler(int		sig)
{
  if (!gl_before_test_ready)
    _exit(128 + sig);
  alarm(0);
  gl_before_test_ready = 0;
  siglongjmp(gl_before_test, sig);
}

