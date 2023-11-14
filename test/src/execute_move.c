/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
** EFRITS SAS 2022-2023
** Pentacle Technologie 2008-2023
**
** TechnoCore
*/

#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_technocore_activity	act;
  char			oripos[1024];
  char			tmppos[1024];

  memset(&act, 0, sizeof(act));
  if (chdir("./test/src/") != 0)
    if (chdir("./src/") == 0)
      {}

  // Start position
  assert(getcwd(&oripos[0], sizeof(oripos)) != 0);
  
  {
    t_bunny_configuration *cnf;
    const char		*code =
      "Target = \"res\""
      ;

    assert(cnf = bunny_read_configuration(BC_DABSIC, code, NULL));
    assert(execute_move("", cnf, cnf, &act) == TC_SUCCESS);
    assert(getcwd(&tmppos[0], sizeof(tmppos)) != 0);
    assert(strcmp(tmppos, oripos) != 0);
  }

  {
    t_bunny_configuration	*cnf;
    const char		*code =
      "Target = \"-\""
      ;

    assert(cnf = bunny_read_configuration(BC_DABSIC, code, NULL));
    assert(execute_move("", cnf, cnf, &act) == TC_SUCCESS);
    assert(getcwd(&tmppos[0], sizeof(tmppos)) != 0);
    assert(strcmp(tmppos, oripos) == 0);
  }

  return (0);
}
