/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

bool			add_all_medals(t_technocore_activity	*act,
				       t_bunny_configuration	*exe)
{
  const char		*med;

  for (int z = 0; bunny_configuration_getf(exe, &med, "Medals[%d]", z); ++z)
    if (!add_exercise_medal(act, med))
      return (false);
  return (true);
}
