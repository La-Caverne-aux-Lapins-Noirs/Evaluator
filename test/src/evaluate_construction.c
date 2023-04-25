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
  const char		*str;
  void			*d;

  assert(dict_open());
  assert(dict_set_language("EN"));

  memset(&act, 0, sizeof(act));
  assert(local = bunny_new_configuration());
  assert(act.current_report = bunny_new_configuration());
  assert(evaluate_construction("a", local, local, &act) == TC_CRITICAL);
  assert(bunny_configuration_setf(local, "TheName", "Name"));
  assert(evaluate_construction("a", local, local, &act) == TC_CRITICAL);

  // Construction partielle
  assert(global = bunny_open_configuration("./src/res/object.dab", NULL));
  assert(evaluate_construction("a", global, global, &act) == TC_SUCCESS);
  assert(bunny_load_file("fichier.out", &d, NULL) == 8);
  assert(strcmp(d, "fichier\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  bunny_delete_configuration(global);
  bunny_delete_file(d, "fichier.out");
  bunny_delete_node(act.current_report, "Conclusion");

  // Construction complete
  assert(global = bunny_open_configuration("./src/res/full.dab", NULL));
  assert(evaluate_construction("a", global, global, &act) == TC_SUCCESS);
  assert(bunny_load_file("duplicate.out", &d, NULL) == 8);
  assert(strcmp(d, "fichier\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));

  // Construction par Makefile
  assert(global = bunny_open_configuration("./src/res/make.dab", NULL));
  assert(chdir("./src/res/make/makeH") == 0);
  evaluate_construction("a", global, global, &act);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(chdir("./../../../../") == 0);

  // Construction par Makefile
  assert(global = bunny_open_configuration("./src/res/cheat.dab", NULL));
  assert(chdir("./src/res/make/makeH") == 0);
  evaluate_construction("a", global, global, &act);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(chdir("./../../../../") == 0);

  return (EXIT_SUCCESS);
}

