/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<dlfcn.h>
#include		"technocore.h"

t_function		*get_function(const char		*argv0,
				      t_technocore_activity	*act,
				      t_bunny_configuration	*general_conf,
				      t_bunny_configuration	*exercise_conf)
{
  static t_function	func;
  const char		*str;

  delete_function(&func);
  if (bunny_configuration_getf(exercise_conf, &str, "UserLibrary") == false)
    if (bunny_configuration_getf(general_conf, &str, "UserLibrary") == false)
      {
	fprintf(stderr, "%s: Cannot find node UserLibrary in activity file.\n", argv0);
	return (NULL);
      }
  if (strcmp(str, "local") == 0)
    str = NULL;
  if ((func.handler = dlopen(str, RTLD_NOW)) == NULL)
    {
      add_exercise_message(act, "CannotFindLibrary", str);
      return (NULL);
    }
  func.function = NULL;
  return (&func);
}
