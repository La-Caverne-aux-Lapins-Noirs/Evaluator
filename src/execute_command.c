/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

#define			COMMAND_BUFFER_SIZE	(16 * 1024)

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
  char			buffer[COMMAND_BUFFER_SIZE];
  char			command[COMMAND_BUFFER_SIZE];
  int			siz;

  if (bunny_configuration_getf(cnf, &cmd, ".") == false)
    for (int i = 0; bunny_configuration_getf(cnf, &cmd, "[%d]", i); ++i)
      {
	snprintf(&command[0], sizeof(command), "%s 2>&1", cmd);
	siz = sizeof(buffer);
	if (tcpopen("command module", &command[0], &buffer[0], &siz, NULL, 0) != 0)
	  {
	    add_to_current_report(act, cmd, "FailedCommand");
	    add_to_current_report(act, &buffer[0], "Message");
	    return (TC_FAILURE);
	  }
      }
  else
    {
      snprintf(&command[0], sizeof(command), "%s 2>&1", cmd);
      siz = sizeof(buffer);
      if (tcpopen("command module", &command[0], &buffer[0], &siz, NULL, 0) != 0)
	{
	  add_to_current_report(act, cmd, "FailedCommand");
	  add_to_current_report(act, &buffer[0], "Message");
	  return (TC_FAILURE);
	}
    }
  return (TC_SUCCESS);
}
