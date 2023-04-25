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
    "]"
    ;

  memset(&act, 0, sizeof(act));
  assert(global = bunny_read_configuration(BC_DABSIC, code, NULL));
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_program_activity("aaa", global, local, &act) == TC_FAILURE);
  return (EXIT_SUCCESS);
}

