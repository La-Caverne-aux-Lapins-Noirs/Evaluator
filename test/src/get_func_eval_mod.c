/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<dlfcn.h>
#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_bunny_configuration	*global;
  t_bunny_configuration	*local;
  t_func_eval_mod	fem;
  void			*handler;

  memset(&fem, 0, sizeof(fem));
  assert(handler = dlopen(NULL, RTLD_NOW));
  assert(global = bunny_new_configuration());
  assert(local = bunny_new_configuration());
  get_func_eval_mod(global, local, &fem, handler);
  return (EXIT_SUCCESS);
}

