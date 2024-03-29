/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include			"technocore.h"

bool				it_was_mandatory(t_technocore_activity		*act,
						 const char			*c,
						 const char			*s)
{
  if (add_exercise_message(act, "ItWasMandatory") == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot write 'it was mandatory' for %s of function %s.\n", c, s);
      return (false);
    }
  return (true);
}

bool				add_medal(t_technocore_activity			*act,
					  t_bunny_configuration			*exe,
					  const char				*field)
{
  if (!exe || add_exercise_all_medals(act, exe, field) == false)
    {
      add_message(&gl_technocore.error_buffer, "Cannot add medals from field %s.\n", field);
      return (false);
    }
  return (true);
}

static t_technocore_result	build_func_report(t_technocore_activity		*act,
						  t_bunny_configuration		*exe,
						  t_func_eval_mod		*fem)
{
  t_function_evaluation		*crit = &fem->criterias;
  t_function_evaluation		*res = &fem->result;
  bool				strict = false;
  bool				success = true;

  if (res->func == false)
    {
      if (add_exercise_message
	  (act, dict_get_pattern("MissingFunction"), fem->user_functions.func_name) == false)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer, "Cannot write function report for missing function %s.\n",
	     fem->user_functions.func_name);
	  return (TC_CRITICAL);
	} // LCOV_EXL_STOP
      return (TC_FAILURE);
    }

  if (crit->mostly_working)
    {
      if (res->mostly_working == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("BrokenFunction"), fem->user_functions.func_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for broken function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "EvaluationMandatory");
	  if (strict)
	    {
	      if (it_was_mandatory(act, "evaluation", fem->user_functions.func_name) == false)
		return (TC_CRITICAL);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "EvaluationMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->fully_working)
    {
      if (res->fully_working == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("AlmostFunction"), fem->user_functions.func_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for almost function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "FullEvaluationMandatory");
	  if (strict)
	    {
	      it_was_mandatory(act, "full evaluation", fem->user_functions.func_name);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "FullEvaluationMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->defensive_programming)
    {
      if (res->defensive_programming == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("VulnerableFunction"), fem->user_functions.func_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for vulnerable function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_START
	  strict = false;
	  bunny_configuration_getf(exe, &strict, "DefensiveProgrammingMandatory");
	  if (strict)
	    {
	      it_was_mandatory(act, "defensive programming", fem->user_functions.func_name);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "DefensiveProgrammingMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->signal_error)
    {
      if (res->signal_error == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("NoErrorHandling"), fem->user_functions.func_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for no error handling function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "ReportErrorMandatory");
	  if (strict)
	    {
	      it_was_mandatory(act, "error handling", fem->user_functions.func_name);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "ReportErrorMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->use_errno)
    {
      if (res->use_errno == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("NoErrnoUsage"), fem->user_functions.func_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for bad errno function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = false;
	  bunny_configuration_getf(exe, &strict, "UseErrnoMandatory");
	  if (strict)
	    {
	      it_was_mandatory(act, "errno usage", fem->user_functions.func_name);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "UseErrnoMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->perf_ratio > 0)
    {
      if (res->perf_ratio > crit->perf_ratio)
	{
	  if (add_exercise_message(act, dict_get_pattern("BadPerformences"),
				   fem->user_functions.func_name,
				   (int)(res->perf_ratio * 100),
				   (int)(crit->perf_ratio * 100)) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for slow function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = false;
	  bunny_configuration_getf(exe, &strict, "MaximumPerfRatioMandatory");
	  if (strict)
	    {
	      it_was_mandatory(act, "performences", fem->user_functions.func_name);
	      success = false;
	    }
	}
      else
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("YourPerformencesAre"),
	       fem->user_functions.func_name,
	       (int)(res->perf_ratio * 100),
	       (int)(crit->perf_ratio * 100)
	       ) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function performence report for function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  if (add_medal(act, exe, "MaximumPerfRatioMedals") == false)
	    return (TC_CRITICAL);
	  if (res->perf_ratio < crit->perf_ratio / 2.0)
	    {
	      if (add_medal(act, exe, "SuperPerfRatioMedals") == false)
		return (TC_CRITICAL);
	      if (res->perf_ratio < crit->perf_ratio / 5.0)
		{
		  if (add_medal(act, exe, "IncrediblePerfRatioMedals") == false)
		    return (TC_CRITICAL);
		}
	    }
	}
    }

  if (crit->ram_ratio > 0)
    {
      if (res->ram_ratio > crit->ram_ratio || res->ram_ratio < 0)
	{
	  if (add_exercise_message(act, dict_get_pattern("TooGreedy"),
				   fem->user_functions.func_name,
				   (int)(res->ram_ratio * 100),
				   (int)(crit->ram_ratio * 100)) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for greedy function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = false;
	  bunny_configuration_getf(exe, &strict, "MaximumRamRatioMandatory");
	  if (strict)
	    {
	      it_was_mandatory(act, "ram", fem->user_functions.func_name);
	      success = false;
	    }
	}
      else
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("YourRamIs"),
	       fem->user_functions.func_name,
	       (int)(res->ram_ratio * 100),
	       (int)(crit->ram_ratio * 100)
	       ) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function ram report for function %s.\n",
		 fem->user_functions.func_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  if (add_medal(act, exe, "MaximumRamRatioMedals") == false)
	    return (TC_CRITICAL);
	}
    }

  return (success ? TC_SUCCESS : TC_FAILURE);
}

static t_technocore_result	build_test_report(t_technocore_activity		*act,
						  t_bunny_configuration		*exe,
						  t_func_eval_mod		*fem)
{
  t_function_evaluation		*crit = &fem->criterias;
  t_function_evaluation		*res = &fem->result;
  bool				strict = false;
  bool				success = true;

  if (res->test_func == false)
    {
      if (add_exercise_message
	  (act, dict_get_pattern("MissingFunction"), fem->user_functions.test_name) == false)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer, "Cannot write function report for function %s.\n",
	     fem->user_functions.test_name);
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      return (TC_FAILURE);
    }

  if (crit->mostly_testing)
    {
      if (res->mostly_testing == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("BrokenTestFunction"), fem->user_functions.test_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for broken test function %s.\n",
		 fem->user_functions.test_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "TestMandatory");
	  if (strict)
	    {
	      if (it_was_mandatory(act, "test", fem->user_functions.func_name) == false)
		return (TC_CRITICAL);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "TestMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->fully_testing)
    {
      if (res->fully_testing == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("IncompleteTestFunction"), fem->user_functions.test_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for incomplete test function %s.\n",
		 fem->user_functions.test_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "FullTestingMandatory");
	  if (strict)
	    {
	      if (it_was_mandatory(act, "full test", fem->user_functions.func_name) == false)
		return (TC_CRITICAL);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "FullTestingMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->defensive_testing)
    {
      if (res->defensive_testing == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("VulnerableTestFunction"), fem->user_functions.test_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for vulnerable test function %s.\n",
		 fem->user_functions.test_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "DefensiveTestingMandatory");
	  if (strict)
	    {
	      if (it_was_mandatory(act, "defensive test", fem->user_functions.func_name) == false)
		return (TC_CRITICAL);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "DefensiveTestingMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->error_testing)
    {
      if (res->error_testing == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("NoErrorTesting"), fem->user_functions.test_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for no error test function %s.\n",
		 fem->user_functions.test_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "ErrorTestingMandatory");
	  if (strict)
	    {
	      if (it_was_mandatory(act, "error test", fem->user_functions.func_name) == false)
		return (TC_CRITICAL);
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "ErrorTestingMedals") == false)
	return (TC_CRITICAL);
    }

  if (crit->errno_testing)
    {
      if (res->errno_testing == false)
	{
	  if (add_exercise_message
	      (act, dict_get_pattern("NoErrnoTesting"), fem->user_functions.test_name) == false)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer, "Cannot write function report for no errno test function %s.\n",
		 fem->user_functions.test_name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  strict = true;
	  bunny_configuration_getf(exe, &strict, "ErrnoTestingMandatory");
	  if (strict)
	    {
	      if (it_was_mandatory(act, "errno test", fem->user_functions.func_name) == false)
		return (TC_CRITICAL); // LCOV_EXCL_LINE
	      success = false;
	    }
	}
      else if (add_medal(act, exe, "ErrnoTestingMedals") == false)
	return (TC_CRITICAL);
    }

  if (success)
    if (add_medal(act, exe, "SuccessMedals") == false)
      return (TC_CRITICAL);
  
  return (success ? TC_SUCCESS : TC_FAILURE);
}

t_technocore_result		build_function_report(t_technocore_activity	*act,
						      t_bunny_configuration	*exe,
						      t_func_eval_mod		*fem)
{
  t_technocore_result		res1 = TC_SUCCESS;
  t_technocore_result		res2 = TC_SUCCESS;

  if (message_len(&fem->module_messages) != 0)
    {
      if (add_exercise_message(act, get_message(&fem->module_messages)) == false)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Cannot flush modules logs to report.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      purge_message(&fem->module_messages);
    }

  if (fem->criterias.func)
    if ((res1 = build_func_report(act, exe, fem)) == TC_CRITICAL)
      return (TC_CRITICAL);

  if (fem->criterias.test_func)
    if ((res2 = build_test_report(act, exe, fem)) == TC_CRITICAL)
      return (TC_CRITICAL);

  if (res1 == TC_FAILURE || res2 == TC_FAILURE)
    return (TC_FAILURE);

  // Tout est réussi, alors on va pouvoir débloquer les fonctions
  // On les ajoute bien sur dans le rapport, mais aussi
  // dans la liste des fonctions autorisées, au cas ou les prochains exercices
  // s'en servent aussi.
  const char			*str;
  int				cnt;

  for (cnt = 0; bunny_configuration_getf(exe, &str, "UnlockedFunctions[%d]", cnt); ++cnt)
    if (add_exercise_function(act, exe, str) == false)
      { // LCOV_EXCL_START
	add_message
	  (&gl_technocore.error_buffer, "Cannot add functio %s to the list of authorized ones.\n",
	   str);
	return (TC_CRITICAL);
      } // LCOV_EXCL_STOP
  return (TC_SUCCESS);
}

