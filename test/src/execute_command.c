/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<stdio.h>
#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_bunny_configuration	*g = bunny_new_configuration();
  t_bunny_configuration	*e = bunny_new_configuration();
  t_technocore_activity	a;

  memset(&a, 0, sizeof(a));
  assert(execute_command("a", g, e, &a) == TC_CRITICAL);

  assert(bunny_configuration_setf(e, "touch lel", "Command"));
  assert(execute_command("a", g, e, &a) == TC_SUCCESS);
  assert(file_exists("lel\n"));

  bunny_delete_node(e, "Command");
  assert(bunny_configuration_setf(e, "touch a", "Command[0]"));
  assert(bunny_configuration_setf(e, "touch b", "Command[1]"));
  assert(execute_command("a", g, e, &a) == TC_SUCCESS);
  assert(file_exists("a"));
  assert(file_exists("b\r"));
  assert(system("rm lel a b") == 0);

  return (EXIT_SUCCESS);
}

