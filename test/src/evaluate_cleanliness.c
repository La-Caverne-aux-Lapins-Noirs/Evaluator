/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<sys/stat.h>
#include		<stdio.h>
#include		<errno.h>
#include		<string.h>
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

  if (chdir("./test/src/res") != 0)
    if (chdir("./src/res/") != 0)
      assert(chdir("./res/") == 0);
  assert((cnf = bunny_open_configuration("./clean1.dab", NULL)));
  int			ret;

  if (system("rm -rf ./tmp")) {}
  assert((ret = mkdir("./tmp", 0775)) == 0 || printf("mkdir: %s", strerror(errno)) != 0);
  assert(chdir("./tmp") == 0);
  assert(system("echo 'indesirable' > './product'") == 0);
  assert(system("echo 'indesirable' > './blob.o'") == 0);

  assert(evaluate_cleanliness("", cnf, cnf, &act) == TC_FAILURE);
  const char		*str;

  assert(bunny_configuration_getf(act.current_report, &str, "UndesirableFound[0]"));
  assert(strcmp(str, "product") == 0);

  assert(bunny_configuration_getf(act.current_report, &str, "UndesirableFound[1]"));
  assert(strcmp(str, "./blob.o") == 0);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The repository is dirty (2 dirty element(s) for a 1 margin).\n") == 0);
  assert(chdir("../") == 0);
  if (system("rm -rf ./tmp")) {}
  bunny_delete_configuration(cnf);
  bunny_delete_configuration(act.current_report);

  /////////////////////////////////////////////////////////////////

  assert(act.current_report = bunny_new_configuration());
  assert((cnf = bunny_open_configuration("./clean2.dab", NULL)));
  assert((ret = mkdir("./tmp", 0775)) == 0 || printf("mkdir: %s", strerror(errno)) != 0);
  assert(chdir("./tmp") == 0);
  assert(system("echo 'indesirable' > './product'") == 0);
  assert(system("echo 'indesirable' > './product2'") == 0);
  assert(evaluate_cleanliness("", cnf, cnf, &act) == TC_SUCCESS);

  assert(bunny_configuration_getf(act.current_report, &str, "UndesirableFound[0]"));
  assert(strcmp(str, "product") == 0);

  assert(bunny_configuration_getf(act.current_report, &str, "UndesirableFound[1]"));
  assert(strcmp(str, "product2") == 0);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The repository is clean enough (2 dirty element(s) for a 2 margin).\n") == 0);
  assert(chdir("../") == 0);
  if (system("rm -rf ./tmp")) {}
  bunny_delete_configuration(cnf);
  bunny_delete_configuration(act.current_report);

  /////////////////////////////////////////////////////////////////

  assert(act.current_report = bunny_new_configuration());
  assert((cnf = bunny_open_configuration("./clean1.dab", NULL)));
  assert((ret = mkdir("./tmp", 0775)) == 0 || printf("mkdir: %s", strerror(errno)) != 0);
  assert(chdir("./tmp") == 0);
  assert(evaluate_cleanliness("", cnf, cnf, &act) == TC_SUCCESS);

  assert(!bunny_configuration_getf(act.current_report, &str, "UndesirableFound[0]"));
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The repository is clean.\n") == 0);
  assert(chdir("../") == 0);
  if (system("rm -rf ./tmp")) {}
  bunny_delete_configuration(cnf);
  bunny_delete_configuration(act.current_report);

  return (EXIT_SUCCESS);
}

