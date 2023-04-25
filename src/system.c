/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<stdio.h>
#include		<stdlib.h>
#include		<stdarg.h>

int			tcsystem(const char		*cmd,
				 ...)
{
  char			buffer[512];
  va_list		lst;

  va_start(lst, cmd);
  vsnprintf(&buffer[0], sizeof(buffer), cmd, lst);
  return (system(&buffer[0]));
}

