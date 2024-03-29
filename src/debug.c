/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include	<stdio.h>
#include	"technocore.h"

#warning A mettre en static une fois le debug terminé.
char		tcdebug_buffer[sizeof(tcdebug_buffer)];
int		tcdebug_len = 0;

void		tcdebug(const char		*str,
			...)
{
  va_list	lst;
  size_t	len;

  va_start(lst, str);
  len = vsnprintf
    (&tcdebug_buffer[tcdebug_len],
     sizeof(tcdebug_buffer) - tcdebug_len,
     str, lst
     );
  if (len + tcdebug_len >= sizeof(tcdebug_buffer))
    return ;
  tcdebug_len += len;
}
