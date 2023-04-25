/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_bunny_configuration	*global;
  t_bunny_configuration	*local;
  t_technocore_activity	act;
  const char		*code =
    "\n"
    "[Local\n"
    "  Name = \"AAA\"\n"
    "]"
    ;

  assert(dict_open());
  assert(dict_set_language("EN"));

  memset(&act, 0, sizeof(act));
  assert(global = bunny_read_configuration(BC_DABSIC, code, NULL));
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_builtin_activity("aaa", global, local, &act) == TC_CRITICAL);

  assert(local = bunny_open_configuration("./src/res/fail_module.dab", NULL));
  assert(start_builtin_activity("aaa", global, local, &act) == TC_FAILURE);

  assert(local = bunny_open_configuration("./src/res/success_module.dab", NULL));
  assert(start_builtin_activity("aaa", global, local, &act) == TC_SUCCESS);

  assert(local = bunny_open_configuration("./src/res/critical_module.dab", NULL));
  assert(start_builtin_activity("aaa", global, local, &act) == TC_CRITICAL);
  const char		*str = NULL;

  assert(local = bunny_open_configuration("./src/res/cleanliness_module.dab", NULL));
  assert(act.current_report = bunny_new_configuration());
  start_builtin_activity("aaa", global, local, &act);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  bunny_delete_node(act.current_report, "Conclusion");

  assert(local = bunny_open_configuration("./src/res/construction_module.dab", NULL));
  assert(act.current_report = bunny_new_configuration());
  assert(start_builtin_activity("aaa", global, local, &act) == TC_CRITICAL);

  assert(local = bunny_open_configuration("./src/res/command_module.dab", NULL));
  assert(act.current_report = bunny_new_configuration());
  assert(start_builtin_activity("aaa", global, local, &act) == TC_SUCCESS);
  assert(!bunny_configuration_getf(act.current_report, &str, "Conclusion"));

  assert(local = bunny_open_configuration("./src/res/bad_module.dab", NULL));
  assert(act.current_report = bunny_new_configuration());
  assert(start_builtin_activity("aaa", global, local, &act) == TC_CRITICAL);
  assert(!bunny_configuration_getf(act.current_report, &str, "Conclusion"));

  return (EXIT_SUCCESS);
}

