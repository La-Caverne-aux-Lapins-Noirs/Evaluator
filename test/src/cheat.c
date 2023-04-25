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
  const char		*str;
  t_technocore_activity	act;
  t_bunny_configuration	*local;

  assert(dict_open());
  assert(dict_set_language("EN"));

  memset(&act, 0, sizeof(act));
  assert(local = bunny_new_configuration());
  assert(act.current_report = bunny_new_configuration());
  assert(bunny_configuration_setf(local, true, "DemangleSymbols"));

  // Seule strlen est autorisé
  assert(chdir("./src/res/cheat/cheat0") == 0);
  assert(system("gcc -c *.c* -fPIC") == 0);
  assert(bunny_configuration_setf(local, "", "AuthorizedFunctions.strlen"));
  assert(evaluate_cheat("a", NULL, local, &act) == TC_SUCCESS);
  bunny_delete_node(local, "AuthorizedFunctions");
  assert(chdir("../../../../") == 0);

  assert(chdir("./src/res/cheat/cheat0_2") == 0);
  assert(system("gcc -c *.c* -fPIC") == 0);
  assert(bunny_configuration_setf(local, "", "AuthorizedFunctions.strlen"));
  assert(evaluate_cheat("a", NULL, local, &act) == TC_FAILURE);
  assert(bunny_configuration_childrenf(act.current_report, "ForbiddenFunctions") == 1);
  assert(bunny_configuration_getf(act.current_report, &str, "ForbiddenFunctions.printf"));
  bunny_delete_node(act.current_report, "ForbiddenFunctions");
  bunny_delete_node(local, "AuthorizedFunctions");
  bunny_delete_node(local, "ForbiddenFunctions");
  assert(chdir("../../../../") == 0);

  // Seule strlen est interdite
  assert(chdir("./src/res/cheat/cheat1") == 0);
  assert(system("gcc -c *.c* -fPIC") == 0);
  assert(bunny_configuration_setf(local, "", "ForbiddenFunctions.strlen"));
  assert(evaluate_cheat("a", NULL, local, &act) == TC_SUCCESS);
  bunny_delete_node(local, "ForbiddenFunctions");
  assert(chdir("../../../../") == 0);

  assert(chdir("./src/res/cheat/cheat1_2") == 0);
  assert(system("gcc -c *.c* -fPIC") == 0);
  assert(bunny_configuration_setf(local, "", "ForbiddenFunctions.strlen"));
  assert(evaluate_cheat("a", NULL, local, &act) == TC_FAILURE);
  assert(bunny_configuration_childrenf(act.current_report, "ForbiddenFunctions") == 1);
  assert(bunny_configuration_getf(act.current_report, &str, "ForbiddenFunctions.strlen"));
  bunny_delete_node(act.current_report, "ForbiddenFunctions");
  bunny_delete_node(local, "AuthorizedFunctions");
  bunny_delete_node(local, "ForbiddenFunctions");
  assert(chdir("../../../../") == 0);

  // Tout est autorisé
  assert(chdir("./src/res/cheat/cheat2") == 0);
  assert(system("gcc -c *.c* -fPIC") == 0);
  assert(evaluate_cheat("a", NULL, local, &act) == TC_SUCCESS);
  bunny_delete_node(local, "ForbiddenFunctions");
  assert(chdir("../../../../") == 0);

  // Si ce n'est pas autorisé, c'est interdit. Si c'est interdit, c'est interdit
  assert(chdir("./src/res/cheat/cheat3") == 0);
  assert(system("gcc -c *.c* -fPIC") == 0);
  assert(bunny_configuration_setf(local, "", "AuthorizedFunctions.strlen"));
  assert(bunny_configuration_setf(local, "", "AuthorizedFunctions.printf"));
  assert(bunny_configuration_setf(local, "", "AuthorizedFunctions.puts"));
  assert(bunny_configuration_setf(local, "", "AuthorizedFunctions.strdup"));
  assert(bunny_configuration_setf(local, "", "ForbiddenFunctions.strdup"));
  assert(evaluate_cheat("a", NULL, local, &act) == TC_FAILURE);
  assert(bunny_configuration_childrenf(act.current_report, "ForbiddenFunctions") == 1);
  assert(bunny_configuration_getf(act.current_report, &str, "ForbiddenFunctions.strdup"));
  bunny_delete_node(local, "AuthorizedFunctions");
  bunny_delete_node(local, "ForbiddenFunctions");

  assert(chdir("../") == 0);
  assert(system("find . -name '*.*o' -delete") == 0);

  return (EXIT_SUCCESS);
}

