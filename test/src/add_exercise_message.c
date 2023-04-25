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
  t_technocore_activity	act;
  t_bunny_configuration	*cnf;
  const char		*str;

  memset(&act, 0, sizeof(act));
  assert(cnf = bunny_new_configuration());
  act.current_report = cnf;
  assert(add_exercise_message(&act, "Message %d", 0));
  assert(add_exercise_message(&act, "Message %d", 1));
  assert(add_exercise_message(&act, "Message %d", 2));
  assert(bunny_configuration_getf(act.current_report, &str, "Message[0]"));
  assert(strcmp(str, "Message 0") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Message[1]"));
  assert(strcmp(str, "Message 1") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Message[2]"));
  assert(strcmp(str, "Message 2") == 0);
  assert(!bunny_configuration_getf(act.current_report, &str, "Message[3]"));
  bunny_delete_configuration(cnf);
  return (EXIT_SUCCESS);
}

