/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2024
**
** TechnoCore
*/

#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_technocore_activity	act;

  assert(dict_open());
  assert(dict_set_language("EN"));
  assert(act.current_report = bunny_new_configuration());

  assert(do_string_diff(&act, "aaa", "Test\\n", "Test\\n", 5) == TC_SUCCESS);
  assert(bunny_configuration_getf(act.current_report, NULL, "Message[0]") == false);
  assert(do_string_diff(&act, "bbb", "AAA", "AAB", -1) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, NULL, "Message[0]") != false);

  bunny_delete_configuration(act.current_report);
  act.current_report = NULL;
  assert(do_string_diff(&act, "bbb", "AAA", "AAB", -1) == TC_CRITICAL);  

  return (EXIT_SUCCESS);
}

