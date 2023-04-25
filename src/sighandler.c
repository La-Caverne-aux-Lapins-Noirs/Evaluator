/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include	<unistd.h>
#include	<setjmp.h>

extern jmp_buf	gl_before_test;

void		sighandler(int		sig)
{
  alarm(0);
  longjmp(gl_before_test, sig);
}

