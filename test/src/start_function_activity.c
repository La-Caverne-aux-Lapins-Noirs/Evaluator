/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_POSIX_C_SOURCE				1
#include		<signal.h>
#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

// Le type de la fonction élève
typedef int		(*t_func)(int				a);
// Le type de la fonction de test élève
typedef int		(*t_test_func)(void);

int			gl_signal;

int			function_to_evaluate(int		x)
{
  return (x);
}

bool			test_function_to_evaluate(void)
{
  if (function_to_evaluate(42) != 42)
    return (false); // LCOV_EXCL_LINE
  return (true);
}

t_technocore_result	test_function(void			*user_handler,
				      t_bunny_configuration	*global,
				      t_bunny_configuration	*local,
				      t_technocore_activity	*act)
{
  t_func_eval_mod	fem;
  t_func		func;
  t_test_func		test;

  (void)act;
  get_func_eval_mod(global, local, &fem, user_handler);
  func = (t_func)fem.user_functions.func;
  test = (t_test_func)fem.user_functions.test;
  assert(func == function_to_evaluate);
  assert(test == test_function_to_evaluate);
  assert(test_function_to_evaluate());
  if (gl_signal > 0)
    {
      if (gl_signal == SIGALRM)
	alarm(0);
      kill(getpid(), gl_signal);
    }
  return (TC_SUCCESS);
}

int			main(void)
{
  t_bunny_configuration	*global;
  t_bunny_configuration	*local;
  t_technocore_activity	act;
  const char		*code =
    "Module = \"local\"\n"
    "UserLibrary = \"local\"\n"
    "[Local\n"
    "  Evaluator = \"test_function\"\n"
    "  FunctionName = \"function_to_evaluate\"\n"
    "  TestName = \"test_function_to_evaluate\"\n"
    "  Timeout = 1\n"
    "]\n"
    ;

  memset(&act, 0, sizeof(act));
  assert(dict_open());
  assert(dict_set_language("EN"));
  assert(act.current_report = bunny_new_configuration());
  assert(global = bunny_read_configuration(BC_DABSIC, code, NULL));
  assert(bunny_configuration_getf(global, &local, "Local"));
  gl_signal = -1;
  assert(start_function_activity("aaa", global, local, &act) == TC_SUCCESS);
  bunny_configuration_setf(act.current_report, NULL, "Message");

  gl_signal = SIGALRM;
  assert(start_function_activity("aaa", global, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &code, "Message"));
  assert(strcmp("Timeout. Maximum delay was 1 seconds.\n", code) == 0);
  bunny_configuration_setf(act.current_report, NULL, "Message");

  gl_signal = SIGSEGV;
  assert(start_function_activity("aaa", global, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &code, "Message"));
  assert(strcmp("Segmentation fault.\n", code) == 0);
  bunny_configuration_setf(act.current_report, NULL, "Message");

  gl_signal = SIGFPE;
  assert(start_function_activity("aaa", global, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &code, "Message"));
  assert(strcmp("Dividing by zero.\n", code) == 0);
  bunny_configuration_setf(act.current_report, NULL, "Message");

  gl_signal = SIGPIPE;
  assert(start_function_activity("aaa", global, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &code, "Message"));
  assert(strcmp("Broken pipe.\n", code) == 0);
  bunny_configuration_setf(act.current_report, NULL, "Message");

  gl_signal = SIGBUS;
  assert(start_function_activity("aaa", global, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &code, "Message"));
  assert(strcmp("Bus error.\n", code) == 0);
  bunny_configuration_setf(act.current_report, NULL, "Message");

  gl_signal = SIGILL;
  assert(start_function_activity("aaa", global, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &code, "Message"));
  assert(strcmp("Illegal instruction.\n", code) == 0);
  bunny_configuration_setf(act.current_report, NULL, "Message");

  return (EXIT_SUCCESS);
}

