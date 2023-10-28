/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include			<signal.h>
#include			<setjmp.h>
#include			"technocore.h"

jmp_buf				gl_before_test;

static t_technocore_result	prepare_jump(const char				*argv0,
					     void				*user_handler,
					     t_technocore_main			tech_func,
					     t_bunny_configuration		*general_cnf,
					     t_bunny_configuration		*exe_cnf,
					     t_technocore_activity		*act)
{
  t_technocore_result		res;
  int				del;
  int				err;

  del = 2;
  bunny_configuration_getf(general_cnf, &del, "Timeout");
  bunny_configuration_getf(exe_cnf, &del, "Timeout");

  alarm(abs(del));
  if ((err = setjmp(gl_before_test)) == 0)
    res = tech_func(user_handler, general_cnf, exe_cnf, act);
  else
    {
      bool			suc = true;

      res = TC_FAILURE;
      if (err == SIGALRM)
	suc = add_exercise_message(act, dict_get_pattern("Timeout"), del);
      else if (err == SIGSEGV)
	suc = add_exercise_message(act, dict_get_pattern("Segfault"));
      else if (err == SIGFPE)
	suc = add_exercise_message(act, dict_get_pattern("Floating"));
      else if (err == SIGPIPE)
	suc = add_exercise_message(act, dict_get_pattern("Pipe"));
      else if (err == SIGBUS)
	suc = add_exercise_message(act, dict_get_pattern("Bus"));
      else if (err == SIGILL)
	suc = add_exercise_message(act, dict_get_pattern("Illegal"));
      else
	suc = add_exercise_message(act, dict_get_pattern("Signal"), err); // LCOV_EXCL_LINE
      if (suc == false)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer,
		      "%s: Cannot add signal message to evaluation.\n", argv0);
	  res = TC_CRITICAL;
	} // LCOV_EXCL_STOP
    }
  alarm(0);
  return (res);
}


static t_technocore_result	prepare_io(const char				*argv0,
					   void					*user_handler,
					   t_technocore_main			tech_func,
					   t_bunny_configuration		*general_cnf,
					   t_bunny_configuration		*exe_cnf,
					   t_technocore_activity		*act)
{
  t_technocore_result		res;
  int				std_out;
  int				std_err;

  purge_message(&gl_technocore.error_buffer);
  std_out = dup(STDOUT_FILENO);
  close(STDOUT_FILENO);
  if (pipe(gl_technocore.stdout_pipe) == -1)
    { // LCOV_EXCL_START
      dup2(std_out, STDOUT_FILENO);
      fprintf(stderr, "%s: Cannot create pipe for stdout. %s.\n", argv0, strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  std_err = dup(STDERR_FILENO);
  close(STDERR_FILENO);
  if (pipe(gl_technocore.stderr_pipe) == -1)
    { // LCOV_EXCL_START
      dup2(std_out, STDOUT_FILENO);
      dup2(std_err, STDERR_FILENO);
      fprintf(stderr, "%s: Cannot create pipe for stderr. %s.\n", argv0, strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  res = prepare_jump(argv0, user_handler, tech_func, general_cnf, exe_cnf, act);

  close(gl_technocore.stdout_pipe[0]);
  close(gl_technocore.stdout_pipe[1]);
  close(gl_technocore.stderr_pipe[0]);
  close(gl_technocore.stderr_pipe[1]);
  dup2(std_out, STDOUT_FILENO);
  dup2(std_err, STDERR_FILENO);
  if (message_len(&gl_technocore.error_buffer) != 0)
    { // LCOV_EXCL_START
      fprintf(stderr, "%s", get_message(&gl_technocore.error_buffer));
      purge_message(&gl_technocore.error_buffer);
    } // LCOV_EXCL_STOP
  return (res);
}

static t_technocore_result	prepare_sighandlers(const char			*argv0,
						    void			*user_handler,
						    t_technocore_main		tech_func,
						    t_bunny_configuration	*general_cnf,
						    t_bunny_configuration	*exe_cnf,
						    t_technocore_activity	*act)
{
  static const int		sigs[] =
    {
     SIGALRM,
     SIGSEGV,
     SIGFPE,
     SIGPIPE,
     SIGBUS,
     SIGILL
    };
  t_technocore_result		res;
  __sighandler_t		old_handlers[NBRCELL(sigs)];

  for (int i = 0; i < (int)NBRCELL(old_handlers); ++i)
    if ((old_handlers[i] = signal(sigs[i], sighandler)) == SIG_ERR)
      { // LCOV_EXCL_START
	fprintf(stderr, "%s: Cannot set handler for signal %d.\n", argv0, sigs[i]);
	for (i -= 1; i >= 0; --i)
	  signal(sigs[i], old_handlers[i]);
	return (TC_FAILURE);
      } // LCOV_EXCL_STOP

  res = prepare_io(argv0, user_handler, tech_func, general_cnf, exe_cnf, act);

  for (int i = 0; i < (int)NBRCELL(old_handlers); ++i)
    signal(sigs[i], old_handlers[i]);
  return (res);
}

t_technocore_result		start_function_activity(const char		*argv0,
							t_bunny_configuration	*general_cnf,
							t_bunny_configuration	*exe_cnf,
							t_technocore_activity	*act)
{
  t_function			*user;
  t_function			*tech;

  if ((tech = get_evaluator(argv0, general_cnf, exe_cnf)) == NULL)
    return (TC_CRITICAL);
  if ((user = get_function(argv0, act, general_cnf, exe_cnf)) == NULL)
    { // LCOV_EXCL_START
      delete_function(tech);
      return (TC_FAILURE);
    } // LCOV_EXCL_STOP
  t_technocore_result		res;

  res = prepare_sighandlers
    (argv0, user->handler, (t_technocore_main)tech->function, general_cnf, exe_cnf, act);
  delete_function(tech);
  delete_function(user);
  return (res);
}
