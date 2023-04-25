/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<assert.h>
#include		"technocore.h"

void			module_main3(void)
{
  if (write(1, "", 0)) {}
}

void			module_main2(void)
{
  if (write(1, "", 0)) {}
}

int			main(void)
{
  module_main3();
  module_main2();
  t_function		*func;
  t_bunny_configuration	*global;
  t_bunny_configuration	*local;
  const char		*code =
    "Module = \"local\"\n"
    "[Local\n"
    "  Evaluator = \"module_main3\"\n"
    "  Module = \"local\"\n"
    "]\n"
    ;

  assert(global = bunny_read_configuration(BC_DABSIC, code, NULL));
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(func = get_evaluator("aaa", global, local));
  assert(func->handler);
  assert(func->function == module_main3);

  assert(bunny_configuration_setf(local, "module_main2", "Evaluator"));
  assert(func = get_evaluator("aaa", global, local));
  assert(func->handler);
  assert(func->function == module_main2);

  assert(bunny_configuration_setf(local, NULL, "Evaluator"));
  assert((func = get_evaluator("aaa", global, local)) == NULL);

  assert(bunny_configuration_setf(local, "local", "Module"));
  assert(bunny_configuration_setf(local, "not_here", "Evaluator"));
  assert((func = get_evaluator("aaa", global, local)) == NULL);

  assert(bunny_configuration_setf(global, NULL, "Module"));
  assert((func = get_evaluator("aaa", global, local)) == NULL);

  assert(bunny_configuration_setf(global, NULL, "Local"));
  assert(bunny_configuration_setf(global, "bad.so", "Module"));
  assert((func = get_evaluator("aaa", global, local)) == NULL);

  assert(bunny_configuration_setf(global, NULL, "Module"));
  assert((func = get_evaluator("aaa", global, local)) == NULL);

  return (EXIT_SUCCESS);
}

