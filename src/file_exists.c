/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include	<string.h>
#include	<stdbool.h>
#include	<unistd.h>

bool		file_exists(const char				*str)
{
  char		buffer[512];
  char		*s;

  strcpy(&buffer[0], str);
  if ((s = strchr(&buffer[0], '\r')) != NULL)
    *s = '\0';
  if ((s = strchr(&buffer[0], '\n')) != NULL)
    *s = '\0';
  s = &buffer[0];
  return (access(s, F_OK | R_OK) == 0);
}
