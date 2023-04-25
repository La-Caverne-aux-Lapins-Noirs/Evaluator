/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_technocore_activity	act;
  const char		*str;

  memset(&act, 0, sizeof(act));
  assert(act.current_report = bunny_new_configuration());
  assert(add_to_current_report(&act, "val", "p%s", "lel"));
  assert(bunny_configuration_getf(act.current_report, &str, "plel"));
  assert(strcmp(str, "val") == 0);

  assert(chdir("./src/report/") == 0);
  act.artefacts[0] = "./out";
  act.nbr_artefacts = 1;
  assert(system("echo 'lel' > out") == 0);
  assert(act.report = bunny_new_configuration());
  assert(bunny_configuration_setf(act.report, 42, "[].Group.Field"));
  assert(build_report(&act));
  assert(system("tar xvfz report.tar.gz") == 0);
  return (EXIT_SUCCESS);
}

