/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

bool			it_was_mandatory(t_technocore_activity		*act,
					 const char			*c,
					 const char			*s);
bool			add_medal(t_technocore_activity			*act,
				  t_bunny_configuration			*exe,
				  const char				*field);

int			main(void)
{
  t_technocore_activity	act;
  t_bunny_configuration	*cnf;
  t_func_eval_mod	fem;
  const char		*code =
    "EvaluationMandatory = true\n"
    "EvaluationMedals = \"A\"\n"
    "FullEvaluationMandatory = true\n"
    "FullEvaluationMedals = \"B\"\n"
    "DefensiveProgrammingMandatory = true\n"
    "DefensiveProgrammingMedals = \"C\"\n"
    "ReportErrorMandatory = true\n"
    "ReportErrorMedals = \"D\"\n"
    "UseErrnoMandatory = true\n"
    "UseErrnoMedals = \"E\"\n"
    "MaximumPerfRatioMandatory = true\n"
    "MaximumPerfRatioMedals = \"F\"\n"
    "MaximumRamRatioMandatory = true\n"
    "MaximumRamRatioMedals = \"G\"\n"

    "TestMandatory = true\n"
    "TestMedals = \"a\"\n"
    "FullTestingMandatory = true\n"
    "FullTestingMedals = \"b\"\n"
    "DefensiveTestingMandatory = true\n"
    "DefensiveTestingMedals = \"c\"\n"
    "ErrorTestingMandatory = true\n"
    "ErrorTestingMedals = \"d\"\n"
    "ErrnoTestingMandatory = true\n"
    "ErrnoTestingMedals = \"e\"\n"

    "LastMedals = \"A\"\n"
    ;
  const char		*str;

  assert(dict_open());
  assert(dict_set_language("EN"));
  memset(&act, 0, sizeof(act));
  assert(cnf = bunny_read_configuration(BC_DABSIC, code, NULL));
  memset(&fem, 0, sizeof(fem));
  fem.user_functions.func_name = "aaa";
  act.report = act.current_report = cnf;

  // Avec aucune évaluation. Seulement la purge des messages.
  fem.criterias.func = false;
  fem.criterias.test_func = false;
  assert(build_function_report(&act, cnf, &fem) == TC_SUCCESS);

  // Absence de la fonction a corriger
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.func = true;
  fem.result.func = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "No aaa function found.\n") == 0);
  fem.result.func = true;

  // La fonction ne marche pas du tout
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.mostly_working = true;
  fem.result.mostly_working = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function aaa does not work at all.\n") == 0);
  fem.result.mostly_working = true;

  // La fonction ne gere pas tous les cas
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.mostly_working = false;
  fem.criterias.fully_working = true;
  fem.result.fully_working = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function aaa does not work in specific cases.\n") == 0);
  fem.result.fully_working = true;

  // La fonction n'est pas écrite en prog defensive
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.fully_working = false;
  fem.criterias.defensive_programming = true;
  fem.result.defensive_programming = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function aaa may crash with incorrects parameters.\n") == 0);
  fem.result.defensive_programming = true;

  // La fonction n'emet pas de code erreur
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.defensive_programming = false;
  fem.criterias.signal_error = true;
  fem.result.signal_error = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function aaa does not handle errors.\n") == 0);
  fem.result.signal_error = true;

  // La fonction n'utilise pas errno ou le fait mal
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.signal_error = false;
  fem.criterias.use_errno = true;
  fem.result.use_errno = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function aaa does not use errno well.\n") == 0);
  fem.result.use_errno = true;

  // La fonction est lente
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.use_errno = false;
  fem.criterias.perf_ratio = 1;
  fem.result.perf_ratio = 2;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strncmp(str, "Function aaa is too slow.", 25) == 0);
  fem.result.perf_ratio = 1;

  // La fonction consomme trop de RAM
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.perf_ratio = -1;
  fem.criterias.ram_ratio = 1;
  fem.result.ram_ratio = 2;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strncmp(str, "Function aaa RAM consumption is problematic.", 44) == 0);
  fem.result.ram_ratio = 1;

  fem.criterias = fem.result;
  assert(build_function_report(&act, cnf, &fem) == TC_SUCCESS);
  assert(bunny_configuration_getf(cnf, &str, "Medals[0]"));
  assert(strcmp(str, "A") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[1]"));
  assert(strcmp(str, "B") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[2]"));
  assert(strcmp(str, "C") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[3]"));
  assert(strcmp(str, "D") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[4]"));
  assert(strcmp(str, "E") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[5]"));
  assert(strcmp(str, "F") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[6]"));
  assert(strcmp(str, "G") == 0);
  assert(bunny_configuration_setf(cnf, NULL, "Medals"));

  // TEST

  memset(&fem, 0, sizeof(fem));
  fem.user_functions.test_name = "bbb";

  // Absence de la fonction a corriger
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.test_func = true;
  fem.result.test_func = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "No bbb function found.\n") == 0);
  fem.result.test_func = true;

  // La fonction ne teste pas du tout
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.mostly_testing = true;
  fem.result.mostly_testing = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function bbb does not test well.\n") == 0);
  fem.result.mostly_testing = true;

  // La fonction ne gere pas tous les cas
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.mostly_testing = false;
  fem.criterias.fully_testing = true;
  fem.result.fully_testing = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function bbb does not cover all cases.\n") == 0);
  fem.result.fully_testing = true;

  // La fonction n'est pas écrite en prog defensive
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.fully_testing = false;
  fem.criterias.defensive_testing = true;
  fem.result.defensive_testing = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function bbb does not test defensives cases.\n") == 0);
  fem.result.defensive_testing = true;

  // La fonction n'emet pas de code erreur
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.defensive_testing = false;
  fem.criterias.error_testing = true;
  fem.result.error_testing = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function bbb does not try error cases.\n") == 0);
  fem.result.error_testing = true;

  // La fonction n'utilise pas errno ou le fait mal
  assert(bunny_configuration_setf(cnf, NULL, "Message"));
  fem.criterias.error_testing = false;
  fem.criterias.errno_testing = true;
  fem.result.errno_testing = false;
  assert(build_function_report(&act, cnf, &fem) == TC_FAILURE);
  assert(bunny_configuration_getf(cnf, &str, "Message[0]"));
  assert(strcmp(str, "Function bbb does not check errno values.\n") == 0);
  fem.result.errno_testing = true;

  fem.criterias = fem.result;
  assert(build_function_report(&act, cnf, &fem) == TC_SUCCESS);
  assert(bunny_configuration_getf(cnf, &str, "Medals[0]"));
  assert(strcmp(str, "a") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[1]"));
  assert(strcmp(str, "b") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[2]"));
  assert(strcmp(str, "c") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[3]"));
  assert(strcmp(str, "d") == 0);
  assert(bunny_configuration_getf(cnf, &str, "Medals[4]"));
  assert(strcmp(str, "e") == 0);

  act.current_report = NULL;
  assert(!it_was_mandatory(&act, "A", "B"));
  assert(strcmp(gl_technocore.error_buffer.message,
		"Cannot write 'it was mandatory' for A of function B.\n")
	 == 0
	 );
  act.current_report = cnf;
  gl_technocore.error_buffer.length = 0;
  assert(!add_medal(&act, NULL, "X"));
  assert(strcmp(gl_technocore.error_buffer.message,
		"Cannot add medals from field X.\n")
	 == 0
	 );

  bunny_delete_configuration(act.current_report);
  assert(act.current_report = bunny_new_configuration());
  add_message(&fem.module_messages, "Test%d", 42);
  fem.criterias.func = NULL;
  fem.criterias.test_func = NULL;
  assert(build_function_report(&act, cnf, &fem) == TC_SUCCESS);
  assert(fem.module_messages.length == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Message[0]"));
  assert(strcmp(str, "Test42") == 0);
  return (EXIT_SUCCESS);
}
