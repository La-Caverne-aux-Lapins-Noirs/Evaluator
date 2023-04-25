/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<unistd.h>
#include		"technocore.h"

t_technocore_result	evaluate_construction(const char		*argv0,
					      t_bunny_configuration	*general_cnf,
					      t_bunny_configuration	*exe,
					      t_technocore_activity	*act)
{
  const char		*str;

  if (!bunny_configuration_getf(exe, &str, "Build"))
    {
      if (bunny_configuration_getf(exe, &str, "Name"))
	fprintf(stderr, "%s: Cannot retrieve Build node for exercise %s.\n",
		argv0, str);
      else
	fprintf(stderr, "%s: Cannot retrieve Build and Name for construction evaluation.\n",
		argv0);
      return (TC_CRITICAL);
    }

  if (bunny_strcasecmp(str, "Object") == 0)
    return (evaluate_object_build(argv0, general_cnf, exe, act));
  if (bunny_strcasecmp(str, "Full") == 0)
    return (evaluate_full_build(argv0, general_cnf, exe, act));
  if (bunny_strcasecmp(str, "Make") == 0)
    return (evaluate_make_build(argv0, general_cnf, exe, act));
  if (bunny_strcasecmp(str, "Cheat") == 0)
    return (evaluate_cheat(argv0, general_cnf, exe, act));
  return (TC_CRITICAL);
}

