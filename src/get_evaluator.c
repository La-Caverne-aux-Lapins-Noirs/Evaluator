/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<dlfcn.h>
#include		"technocore.h"

t_function		*get_evaluator(const char		*argv0,
				       t_bunny_configuration	*general_conf,
				       t_bunny_configuration	*exercise_conf)
{
  static t_function	func;
  const char		*str;
  void			*handler;

  delete_function(&func);
  if (bunny_configuration_getf(exercise_conf, &str, "Module") == false)
    if (bunny_configuration_getf(general_conf, &str, "Module") == false)
      {
	fprintf(stderr, "%s: Cannot find node Module in activity file.\n", argv0);
	return (NULL);
      }
  if (strcmp("local", str) == 0)
    str = NULL;
  if ((handler = dlopen(str, RTLD_NOW)) == NULL)
    {
      char		buffer[512];

      snprintf(&buffer[0], sizeof(buffer), "/usr/lib/technocore/%s", str);
      if ((handler = dlopen(str, RTLD_NOW)) == NULL)
	{
	  fprintf(stderr, "%s: Cannot open Module %s (%s).\n", argv0, str, dlerror());
	  return (NULL);
	}
    }

  void			*function;

  if (bunny_configuration_getf(exercise_conf, &str, "Evaluator") == false)
    {
      if ((function = dlsym(handler, "module_main")) == NULL)
	{
	  dlclose(handler);
	  fprintf(stderr, "%s: Cannot retrieve function module_main from evaluator.\n", argv0);
	  return (NULL);
	}
    }
  else if ((function = dlsym(handler, str)) == NULL)
    {
      dlclose(handler);
      fprintf(stderr, "%s: Cannot retrieve function %s from evaluator.\n", argv0, str);
      return (NULL);
    }
  func.handler = handler;
  func.function = function;
  return (&func);
}
