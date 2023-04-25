/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

bool			add_exercise_medal(t_technocore_activity	*act,
					   const char			*md)
{
  if (bunny_configuration_getf(act->current_report, NULL, "Medals") == false)
    return (bunny_configuration_setf(act->current_report, md, "Medals[0]"));
  ssize_t		len;

  len = bunny_configuration_casesf(act->current_report, "Medals");
  return (bunny_configuration_setf(act->current_report, md, "Medals[%zd]", len));
}

