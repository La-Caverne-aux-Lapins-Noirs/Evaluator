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
  t_bunny_configuration	*cnf;

  assert(dict_open());
  assert(dict_set_language("EN"));
  memset(&act, 0, sizeof(act));
  assert(act.current_report = bunny_new_configuration());

  assert(chdir("./test/norm/") == 0);
  assert(cnf = bunny_open_configuration("./cln.dab", NULL));

}

