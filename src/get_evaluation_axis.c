/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

void			get_evaluation_axis(t_bunny_configuration	*global_cnf,
					    t_bunny_configuration	*local_cnf,
					    t_function_evaluation	*eval)
{
  // Valeur par defaut des elements configurable
  eval->mostly_working = true;
  eval->fully_working = true;
  eval->defensive_programming = false;
  eval->signal_error = true;
  eval->use_errno = false;
  eval->perf_ratio = -1;
  eval->ram_ratio = -1;

  eval->test_func = true;
  eval->mostly_testing = true;
  eval->fully_testing = true;
  eval->defensive_testing = false;
  eval->error_testing = true;
  eval->errno_testing = false;

  // Initialisation d'element servant a l'ecriture du rapport et non a la configuration de l'exercice
  eval->cheater = false;
  eval->missing_main_function = false;
  eval->missing_test_function = false;

  if (!global_cnf || !local_cnf)
    return ;

  // On récupère le global puis le local.
  // Quand l'un des deux est absent, rien n'est établi
  // Si les deux sont présent, le dernier l'emporte
  bunny_configuration_getf(global_cnf, &eval->mostly_working, "Evaluation");
  bunny_configuration_getf(local_cnf, &eval->mostly_working, "Evaluation");

  bunny_configuration_getf(global_cnf, &eval->fully_working, "FullEvaluation");
  bunny_configuration_getf(local_cnf, &eval->fully_working, "FullEvaluation");

  bunny_configuration_getf(global_cnf, &eval->defensive_programming, "DefensiveProgramming");
  bunny_configuration_getf(local_cnf, &eval->defensive_programming, "DefensiveProgramming");

  bunny_configuration_getf(global_cnf, &eval->signal_error, "ReportError");
  bunny_configuration_getf(local_cnf, &eval->signal_error, "ReportError");

  bunny_configuration_getf(global_cnf, &eval->use_errno, "UseErrno");
  bunny_configuration_getf(local_cnf, &eval->use_errno, "UseErrno");

  if (eval->signal_error == false)
    eval->use_errno = false;
  
  bunny_configuration_getf(global_cnf, &eval->perf_ratio, "MaximumPerfRatio");
  bunny_configuration_getf(local_cnf, &eval->perf_ratio, "MaximumPerfRatio");

  bunny_configuration_getf(global_cnf, &eval->ram_ratio, "MaximumRamRatio");
  bunny_configuration_getf(local_cnf, &eval->ram_ratio, "MaximumRamRatio");

  eval->func
    = eval->mostly_working
    || eval->fully_working
    || eval->defensive_programming
    || eval->signal_error
    || eval->use_errno
    || eval->perf_ratio
    || eval->ram_ratio
    ;

  bunny_configuration_getf(global_cnf, &eval->test_func, "Test");
  bunny_configuration_getf(local_cnf, &eval->test_func, "Test");

  bunny_configuration_getf(global_cnf, &eval->fully_testing, "FullTesting");
  bunny_configuration_getf(local_cnf, &eval->fully_testing, "FullTesting");

  bunny_configuration_getf(global_cnf, &eval->defensive_testing, "DefensiveTesting");
  bunny_configuration_getf(local_cnf, &eval->defensive_testing, "DefensiveTesting");

  bunny_configuration_getf(global_cnf, &eval->error_testing, "ErrorTesting");
  bunny_configuration_getf(local_cnf, &eval->error_testing, "ErrorTesting");

  bunny_configuration_getf(global_cnf, &eval->errno_testing, "ErrnoTesting");
  bunny_configuration_getf(local_cnf, &eval->errno_testing, "ErrnoTesting");

  if (eval->error_testing == false)
    eval->errno_testing = false;
}
