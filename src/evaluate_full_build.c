/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<stdio.h>
#include		<errno.h>
#include		<unistd.h>
#include		"technocore.h"

t_technocore_result	evaluate_full_build(const char			*argv,
					    t_bunny_configuration	*gen,
					    t_bunny_configuration	*exe,
					    t_technocore_activity	*act)
{
  char			build_command[1024];
  const char		*build_cmd;

  (void)argv;
  (void)gen;
  ///////////////////////////////////////////////////////////////
  // Compilation de tous les fichiers .o en un unique programme
  // Ce module devrait etre appellé après objet_build
  // Si l'objet a construire est une bibliothèque, cela doit se régler avec la commande
  // de construction.
  if (!bunny_configuration_getf(exe, &build_cmd, "BuildCommand"))
    build_cmd = "gcc -L/usr/local/lib/ `find . -name '*.o'` -o program"; // LCOV_EXCL_LINE
  snprintf(&build_command[0], sizeof(build_command), "%s 2>&1", build_cmd);

  FILE			*compile_pipe;
  char			buffer[8128];
  size_t		red;
  int			ret;

  if ((compile_pipe = popen(build_command, "r")) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Fail to execute '%s' for object build module (%s).\n",
		  build_command, strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  red = fread(&buffer, 1, sizeof(buffer), compile_pipe);
  if (ferror(compile_pipe))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Fail to read from command construction '%s'.\n", build_command);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  ret = pclose(compile_pipe);
  const char		*str;

  if (red != 0 || ret != 0)
    {
      if (ret != 0)
	str = "Error";
      else
	str = "Warning"; // LCOV_EXCL_LINE
      if (add_to_current_report(act, str, "Status") == false)
	return (TC_CRITICAL);
      if (add_to_current_report(act, buffer, "Message") == false)
	return (TC_CRITICAL);
    }
  t_technocore_result	res;

  res = TC_SUCCESS;
  if (red == 0 && ret == 0) // Tout va bien
    str = dict_get_pattern("ConstructionFullyAchieved");
  else
    {
      bool		tolerance;

      if (!bunny_configuration_getf(exe, &tolerance, "Tolerance"))
	tolerance = false;
      if (ret != 0) // Il y a des erreurs, c'est mort
	{
	  res = TC_FAILURE;
	  str = dict_get_pattern("ConstructionFailure");
	} // LCOV_EXCL_START
      else if (tolerance) // Il y a des warnings mais on tolere
	str = dict_get_pattern("ConstructionComplete");
      else // Il y a des warnings et ce n'est pas permit
	{
	  res = TC_FAILURE;
	  str = dict_get_pattern("ConstructionStopped");
	} // LCOV_EXCL_STOP
    }

  if (!add_to_current_report(act, str, "Conclusion"))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Fail to write conclusion for full build module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  return (res);
}
