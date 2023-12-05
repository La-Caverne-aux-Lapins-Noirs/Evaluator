/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<dlfcn.h>
#include		"technocore.h"

void			get_user_functions(void				*handler,
					   t_function_evaluation	*eval_cnf,
					   t_function_evaluation	*eval,
					   t_bunny_configuration	*local_cnf,
					   t_user_functions		*ufunc)
{
  memset(ufunc, 0, sizeof(*ufunc));

  // Y a t il une fonction a tester?
  if (eval_cnf->func)
    {
      eval->missing_main_function = true;
      if (!bunny_configuration_getf(local_cnf, &ufunc->func_name, "FunctionName"))
	eval->missing_main_function = false;
      else if ((ufunc->func = dlsym(handler, ufunc->func_name)) != NULL)
	eval->missing_main_function = false;
    }

  // Y a t il un test a tester?
  if (eval_cnf->test_func)
    {
      eval->missing_test_function = true;
      if (!bunny_configuration_getf(local_cnf, &ufunc->test_name, "TestFunctionName"))
	eval->missing_test_function = false;
      else if ((ufunc->test = dlsym(handler, ufunc->test_name)) != NULL)
	eval->missing_test_function = false;
    }
}
