/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define				_GNU_SOURCE
#include			<fcntl.h>
#include			<limits.h>
#include			<unistd.h>
#include			<signal.h>
#include			<errno.h>
#include			<sys/wait.h>
#include			<errno.h>
#include			"technocore.h"

static char			prog_buffer[16 * 1024 * 1024];
int				__stdin;
int				__stdout;

// LCOV_EXCL_START
static void			siguser1(int					x)
{
  close(__stdin);
  exit(x);
}
// LCOV_EXCL_STOP

static void			siguser2(int					x)
{
  (void)x;
  close(__stdout);
  return ;
}
