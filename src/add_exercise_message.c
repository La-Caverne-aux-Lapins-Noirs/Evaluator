/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

bool			add_exercise_message(t_technocore_activity	*act,
					     const char			*md,
					     ...)
{
  char			buffer[2048];
  va_list		lst;
  int			len;

  va_start(lst, md);
  if ((len = vsnprintf(&buffer[0], sizeof(buffer), md, lst)) >= (int)sizeof(buffer))
    return (false);
  va_end(lst);

  if (act->current_report == NULL)
    return (false);

  len = bunny_configuration_casesf(act->current_report, "Message");
  len = len < 0 ? 0 : len;
  return (bunny_configuration_setf(act->current_report, &buffer[0], "Message[%d]", len));
}

