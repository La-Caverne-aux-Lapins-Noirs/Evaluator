/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<stdarg.h>
#include		"technocore.h"

bool			add_message(t_message		*msg,
				    const char		*str,
				    ...)
{
  va_list		lst;
  int			len;
  int			ret;

  if ((len = NBRCELL(msg->message) - msg->length) == 0)
    return (false);
  va_start(lst, str);
  ret = vsnprintf(&msg->message[msg->length], len, str, lst);
  va_end(lst);
  if (ret < 0)
    return (false);
  if (ret >= len)
    return (false);
  msg->length += ret;
  return (true);
}
