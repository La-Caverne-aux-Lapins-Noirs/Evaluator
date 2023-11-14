/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include			"technocore.h"

// LCOV_EXCL_START
static t_technocore_result	critical(const char				*argv0,
					 const char				*msg,
					 t_bunny_configuration			*exe)
{
  const char			*module;

  if (bunny_configuration_getf(exe, &module, "Name"))
    fprintf(stderr, "%s: %s %s.\n",
	    argv0, msg, module);
  else
    fprintf(stderr, "%s: %s %s.\n",
	    argv0, msg, bunny_configuration_get_address(exe));
  return (TC_CRITICAL);
}
// LCOV_EXCL_STOP

t_technocore_result		start_builtin_activity(const char		*argv0,
						       t_bunny_configuration	*general_cnf,
						       t_bunny_configuration	*exe,
						       t_technocore_activity	*act)
{
  const char			*module;

  if (!bunny_configuration_getf(exe, &module, "Module"))
    return (critical(argv0, "Cannot retrieve Module node for exercise", exe));
  if (strcmp(module, "Failure") == 0)
    return (TC_FAILURE);
  if (strcmp(module, "Success") == 0)
    return (TC_SUCCESS);
  if (strcmp(module, "Critical") == 0)
    return (TC_CRITICAL);

  if (strcmp(module, "Cleanliness") == 0)
    return (evaluate_cleanliness(argv0, general_cnf, exe, act));
  if (strcmp(module, "Construction") == 0)
    return (evaluate_construction(argv0, general_cnf, exe, act));
  if (strcmp(module, "Command") == 0)
    return (execute_command(argv0, general_cnf, exe, act));
  if (strcmp(module, "Move") == 0)
    return (execute_move(argv0, general_cnf, exe, act));
  if (strcmp(module, "Norm") == 0)
    return (evaluate_file_c_norm(argv0, general_cnf, exe, act));
  if (strcmp(module, "Unset") == 0)
    {
      t_bunny_configuration	*vcnf;

      if (!bunny_configuration_getf(exe, NULL, "Variables"))
	bunny_delete_node(general_cnf, "Variables");
      if (bunny_configuration_getf(general_cnf, &vcnf, "Variables"))
	for (int v = 0; bunny_configuration_getf(exe, &module, "Variables[%d]", v); ++v)
	  bunny_delete_node(vcnf, module);
      return (TC_SUCCESS);
    }

  return (critical(argv0, "Invalid Module specified for exercise", exe));
}


