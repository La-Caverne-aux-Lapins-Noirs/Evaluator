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
  t_bunny_configuration	*cnf;
  t_bunny_configuration	*empty;
  t_function_evaluation	eval;
  const char		*code =
    "Evaluation = true\n"
    "FullEvaluation = false\n"
    "DefensiveProgramming = true\n"
    "ReportError = false\n"
    "UseErrno = true\n"
    "MaximumPerfRatio = 2\n"
    "MaximumRamRatio = 3\n"
    "Test = true\n"
    "FullTesting = false\n"
    "DefensiveTesting = true\n"
    "ErrorTesting = false\n"
    "ErrnoTesting = true\n"
    ;

  memset(&eval, 0, sizeof(eval));
  assert(cnf = bunny_read_configuration(BC_DABSIC, code, NULL));
  assert(empty = bunny_new_configuration());

  // Defaut values
  get_evaluation_axis(NULL, NULL, &eval);
  assert(eval.mostly_working == true);
  assert(eval.fully_working == true);
  assert(eval.defensive_programming == false);
  assert(eval.signal_error == true);
  assert(eval.use_errno == false);
  assert(fabs(eval.perf_ratio + 1) < 0.1);
  assert(fabs(eval.ram_ratio + 1) < 0.1);
  assert(eval.test_func == true);
  assert(eval.mostly_testing == true);
  assert(eval.fully_testing == true);
  assert(eval.defensive_testing == false);
  assert(eval.error_testing == true);
  assert(eval.errno_testing == false);
  assert(eval.cheater == false);
  assert(eval.missing_main_function == false);
  assert(eval.missing_test_function == false);

  // Global only
  get_evaluation_axis(cnf, empty, &eval);
  assert(eval.mostly_working == true);
  assert(eval.fully_working == false);
  assert(eval.defensive_programming == true);
  assert(eval.signal_error == false);
  assert(eval.use_errno == true);
  assert(fabs(eval.perf_ratio - 2.0) < 0.1);
  assert(fabs(eval.ram_ratio - 3.0) < 0.1);
  assert(eval.test_func == true);
  assert(eval.mostly_testing == true);
  assert(eval.fully_testing == false);
  assert(eval.defensive_testing == true);
  assert(eval.error_testing == false);
  assert(eval.errno_testing == true);
  assert(eval.cheater == false);
  assert(eval.missing_main_function == false);
  assert(eval.missing_test_function == false);

  // Reset
  get_evaluation_axis(NULL, NULL, &eval);
  assert(eval.mostly_working == true);
  assert(eval.fully_working == true);
  assert(eval.defensive_programming == false);
  assert(eval.signal_error == true);
  assert(eval.use_errno == false);
  assert(fabs(eval.perf_ratio + 1) < 0.1);
  assert(fabs(eval.ram_ratio + 1) < 0.1);
  assert(eval.test_func == true);
  assert(eval.mostly_testing == true);
  assert(eval.fully_testing == true);
  assert(eval.defensive_testing == false);
  assert(eval.error_testing == true);
  assert(eval.errno_testing == false);
  assert(eval.cheater == false);
  assert(eval.missing_main_function == false);
  assert(eval.missing_test_function == false);

  // Local only
  get_evaluation_axis(empty, cnf, &eval);
  assert(eval.mostly_working == true);
  assert(eval.fully_working == false);
  assert(eval.defensive_programming == true);
  assert(eval.signal_error == false);
  assert(eval.use_errno == true);
  assert(fabs(eval.perf_ratio - 2.0) < 0.1);
  assert(fabs(eval.ram_ratio - 3.0) < 0.1);
  assert(eval.test_func == true);
  assert(eval.mostly_testing == true);
  assert(eval.fully_testing == false);
  assert(eval.defensive_testing == true);
  assert(eval.error_testing == false);
  assert(eval.errno_testing == true);
  assert(eval.cheater == false);
  assert(eval.missing_main_function == false);
  assert(eval.missing_test_function == false);

  return (EXIT_SUCCESS);
}
