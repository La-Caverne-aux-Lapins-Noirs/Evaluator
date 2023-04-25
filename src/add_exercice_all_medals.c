/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

bool			add_exercise_all_medals(t_technocore_activity	*act,
						t_bunny_configuration	*exe,
						const char		*field)
{
  const char		*s;
  int			i;

  if ((i = bunny_configuration_casesf(exe, "%s", field)) == 0)
    {
      if (bunny_configuration_getf(exe, &s, "%s", field) == false)
	return (false);
      return (add_exercise_medal(act, s));
    }

  for (i = 0; bunny_configuration_getf(exe, &s, "%s[%d]", field, i); ++i)
    if (add_exercise_medal(act, s) == false)
      return (false);
  return (true);
}

