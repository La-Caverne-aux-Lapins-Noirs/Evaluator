/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<unistd.h>
#include		"technocore.h"

t_technocore_result	execute_move(const char				*argv,
				     t_bunny_configuration		*general_cnf,
				     t_bunny_configuration		*exe,
				     t_technocore_activity		*act)
{
  const char		*cmd;

  (void)argv;
  (void)general_cnf;
  (void)act;
  if (bunny_configuration_getf(exe, &cmd, "Target") == false) 
    { // LCOV_EXCL_START
      // Ce module sert à la correction quand elle a besoin d'effectuer une tache.
      // Si Command n'est pas la, c'est qu'il y a une erreur!
      add_message(&gl_technocore.error_buffer,
		  "Missing command field for builtin command.\n"
		  );
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  // On le fait toujours, ne serait ce que pour avoir l'info
  if (!getcwd(act->pathstack[act->nbr_path], sizeof(act->pathstack[act->nbr_path])))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "%s: Cannot get current working directory for Move command. %s.\n",
		  *argv, strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  // On effectue le déplacement
  if (strcmp(cmd, "-") == 0)
    {
      if (act->nbr_path == 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer,
		      "No path to go back to in Move command.\n"
		      );
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      cmd = act->pathstack[--act->nbr_path];
    }
  else
    act->nbr_path += 1;
  if (chdir(cmd) != 0)
    goto Bad; // LCOV_EXCL_LINE
  if (!add_to_current_report(act, cmd, "Moving"))
    return (TC_CRITICAL);
  return (TC_SUCCESS);

 Bad:
  if (errno == ENOENT || errno == ENOTDIR)
    {
      if (add_exercise_message(act, dict_get_pattern("CannotMoveToDir"),
			       cmd, act->pathstack[act->nbr_path - 1],
			       errno))
	return (TC_FAILURE);
      // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "%s: Cannot notify directory change to %s from %s "
		  "failure because of %s.\n",
		  *argv, cmd, strerror(errno));
      return (TC_CRITICAL);
    }
  add_message(&gl_technocore.error_buffer,
	      "%s: Cannot move to directory %s. %s.\n",
	      *argv, cmd, strerror(errno));
  return (TC_CRITICAL);
  // LCOV_EXCL_STOP
}
