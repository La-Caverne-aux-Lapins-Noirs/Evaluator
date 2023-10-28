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
  const char		*x = "aaa";
  t_technocore_activity	act;
  t_bunny_configuration	*cnf;

  assert(dict_open());
  assert(dict_set_language("EN"));
  memset(&act, 0, sizeof(act));
  assert(act.current_report = bunny_new_configuration());

  if (chdir("./test/src/res") != 0)
    if (chdir("./src/res/") != 0)
      assert(chdir("./res/") == 0);

  assert(cnf = bunny_open_configuration("./mcq.dab", NULL));
  assert(evaluate_mcq(x, cnf, cnf, &act) == TC_SUCCESS);
  const char		*rep;

  assert(bunny_configuration_getf(act.current_report, &rep, "Message[0]"));
  assert(strcmp(rep, "Question 'BBB': incomplete answer.\n") == 0);

  assert(bunny_configuration_getf(act.current_report, &rep, "Message[1]"));
  assert(strcmp(rep, "Question 'CCC': invalid answer.\n") == 0);

  assert(bunny_configuration_getf(act.current_report, &rep, "Message[2]"));
  assert(strcmp(rep, "Last question was mandatory.\n") == 0);
  
  assert(bunny_configuration_getf(act.current_report, &rep, "Message[3]"));
  assert(strcmp(rep, "Success. 4 points acquired. Minimum is 3. Maximum is 7.\n") == 0);

  bunny_delete_configuration(act.current_report);
  assert(act.current_report = bunny_new_configuration());
  assert(bunny_configuration_setf(cnf, "wrong.csv", "AnswersFile"));
  assert(evaluate_mcq(x, cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &rep, "Message[0]"));
  assert(strcmp(rep, "Question 'AAA': invalid answer.\n") == 0);

  assert(bunny_configuration_getf(act.current_report, &rep, "Message[1]"));
  assert(strcmp(rep, "Question 'BBB': invalid answer.\n") == 0);

  assert(bunny_configuration_getf(act.current_report, &rep, "Message[2]"));
  assert(strcmp(rep, "Question 'CCC': invalid answer.\n") == 0);

  assert(bunny_configuration_getf(act.current_report, &rep, "Message[3]"));
  assert(strcmp(rep, "Last question was mandatory.\n") == 0);
  
  assert(bunny_configuration_getf(act.current_report, &rep, "Message[4]"));
  assert(strcmp(rep, "Failure. -3 points acquired. Minimum is 3. Maximum is 7.\n") == 0);

  return (0);
}
