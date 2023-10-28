/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

t_technocore_result	execute_command(const char			*argv,
					t_bunny_configuration		*general_cnf,
					t_bunny_configuration		*exe,
					t_technocore_activity		*act)
{
  t_bunny_configuration	*cnf;
  const char		*cmd;

  (void)argv;
  (void)general_cnf;
  (void)act;
  if (bunny_configuration_getf(exe, &cnf, "Command") == false)
    { // LCOV_EXCL_START
      // Ce module sert à la correction quand elle a besoin d'effectuer une tache.
      // Si Command n'est pas la, c'est qu'il y a une erreur!
      add_message(&gl_technocore.error_buffer,
		  "Missing command field for builtin command.\n"
		  );
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (bunny_configuration_getf(cnf, &cmd, ".") == false)
    for (int i = 0; bunny_configuration_getf(cnf, &cmd, "[%d]", i); ++i)
      {
	if (system(cmd) != 0)
	  return (TC_FAILURE); // LCOV_EXCL_LINE
      }
  else if (system(cmd) != 0)
    return (TC_FAILURE); // LCOV_EXCL_LINE
  return (TC_SUCCESS);
}
