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

  if (chdir("./test/src/res/") != 0)
    if (chdir("./src/res/") != 0)
      assert(chdir("./res/") == 0);
  
  assert(cnf = bunny_open_configuration("./cln.dab", NULL));
  
  assert(chdir("norm/success") == 0);
  assert(evaluate_file_c_norm("norm", NULL, cnf, &act) == TC_SUCCESS);

  assert(chdir("../") == 0);
  assert(chdir("failure") == 0);
  assert(evaluate_file_c_norm("norm", NULL, cnf, &act) == TC_FAILURE);

  return (0);
}

