/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_POSIX_C_SOURCE		2023
#include		<unistd.h>
#include		<setjmp.h>

extern sigjmp_buf	gl_before_test;

void			sighandler(int		sig)
{
  alarm(0);
  siglongjmp(&gl_before_test, sig);
}

