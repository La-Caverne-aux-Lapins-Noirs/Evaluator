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
  double		d;
  const char		*code =
    "GivenMedals = \"A\", \"B\", \"C\"\n"
    ;

  memset(&act, 0, sizeof(act));
  assert(cnf = bunny_read_configuration(BC_DABSIC, code, NULL));
  act.current_report = cnf;
  assert(add_exercise_all_medals(&act, cnf, "GivenMedals"));

  assert(bunny_configuration_getf(cnf, &code, "Medals[0]"));
  assert(strcmp(code, "A") == 0);
  assert(bunny_configuration_getf(cnf, &code, "Medals[1]"));
  assert(strcmp(code, "B") == 0);
  assert(bunny_configuration_getf(cnf, &code, "Medals[2]"));
  assert(strcmp(code, "C") == 0);
  assert(!bunny_configuration_getf(cnf, &code, "Medals[3]"));

  assert(set_exercise_grade(&act, 1337));
  assert(bunny_configuration_getf(act.current_report, &d, "Grade"));
  assert(fabs(d - 1337) < 0.01);

  bunny_delete_configuration(cnf);
  return (EXIT_SUCCESS);
}


