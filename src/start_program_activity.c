/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define				_GNU_SOURCE
#include			<fcntl.h>
#include			<limits.h>
#include			<stdio.h>
#include			<stdlib.h>
#include			<string.h>
#include			<unistd.h>
#include			<signal.h>
#include			<errno.h>
#include			<sys/wait.h>
#include			<errno.h>
#include			"technocore.h"

static char			prog_buffer[16 * 1024 * 1024];
int				__stdin;
int				__stdout;
int				__stderr;

static pid_t			waitpid_no_intr(pid_t				pid,
						int				*status)
{
  pid_t				ret;

  do
    ret = waitpid(pid, status, 0);
  while (ret == -1 && errno == EINTR);
  return (ret);
}

static int			wait_status_to_return_value(int			status)
{
  if (WIFEXITED(status))
    return (WEXITSTATUS(status));
  if (WIFSIGNALED(status))
    return (128 + WTERMSIG(status));
  return (status);
}

// LCOV_EXCL_START
static void			siguser1(int					x)
{
  close(__stdin);
  exit(x);
}
// LCOV_EXCL_STOP

static void			siguser2(int					x)
{
  (void)x;
  close(__stdout);
  close(__stderr);
  return ;
}

static bool			ignore_stream(t_bunny_configuration		*cnf,
					      const char			*field1,
					      const char			*field2)
{
  bool				ignore;

  ignore = false;
  if (bunny_configuration_getf(cnf, &ignore, field1) && ignore)
    return (true);
  ignore = false;
  if (field2 != NULL && bunny_configuration_getf(cnf, &ignore, field2) && ignore)
    return (true);
  return (false);
}

static bool			get_stderr_expected(t_bunny_configuration		*cnf,
					    const char			**output)
{
  if (bunny_configuration_getf(cnf, output, "Error"))
    return (true);
  if (bunny_configuration_getf(cnf, output, "Stderr"))
    return (true);
  return (false);
}

static void			diff_name(const char			*name,
					  const char			*stream,
					  char				*buffer,
					  size_t			siz)
{
  if (strcmp(stream, "stdout") == 0)
    snprintf(buffer, siz, "%s", name);
  else
    snprintf(buffer, siz, "%s %s", name, stream);
}

static t_technocore_result	read_expected_stream
(const char			*argv0,
 const char			*name,
 t_technocore_activity		*act,
 int				fd,
 const char			*stream,
 const char			*output,
 int				maxtm,
 bool				control_messages)
{
  ssize_t			ret;
  size_t			len;
  char				dname[256];

  len = 0;
  ret = 1;
  while (errno != EBADF && ret != 0)
    {
      errno = 0;
      if ((ret = read(fd, &prog_buffer[len], 1)) > 0)
	{
	  if ((len += 1) >= sizeof(prog_buffer) - 1)
	    {
	      add_message
		(&gl_technocore.error_buffer,
		 "%s: Data input is too big on %s for program test %s.\n",
		 argv0, stream, name);
	      return (TC_CRITICAL);
	    }
	  if (prog_buffer[len - 1] == '\n')
	    break ;
	}
      else if (ret == -1 && errno != EAGAIN)
	break ;
      else if (usleep(10) == -1)
	{
	  ret = 0;
	  strcpy(prog_buffer, "TIMEOUT");
	  break ; // Timeout!
	}
    }
  prog_buffer[len] = '\0';

  if (ret == -1 && errno == EBADF)
    {
      if (*output != '\0')
	{
	  diff_name(name, stream, &dname[0], sizeof(dname));
	  return (do_string_diff(act, &dname[0], prog_buffer, "EOF", -1));
	}
      return (TC_SUCCESS);
    }
  if (ret == -1 && errno != EAGAIN)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot read data from %s for program test %s:%s.\n",
	 argv0, stream, name, strerror(errno));
      return (TC_CRITICAL);
    }

  if (control_messages && strcmp(prog_buffer, "TIMEOUT") == 0)
    { // LCOV_EXCL_START
      if (add_exercise_message(act, dict_get_pattern("Timeout"), maxtm) == false)
	{
	  add_message(&gl_technocore.error_buffer, "%s: Cannot mark timeout for program test %s.\n", argv0, name);
	  return (TC_CRITICAL);
	}
      return (TC_FAILURE);
    } // LCOV_EXCL_STOP
  if (control_messages && strcmp(prog_buffer, "DUPFAIL") == 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "%s: Error while duping fds in program test %s.\n", argv0, name);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (control_messages && strcmp(prog_buffer, "SIGNALFAIL") == 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "%s: Error while setting sighandler in program test %s.\n", argv0, name);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (control_messages && strcmp(prog_buffer, "MALLOCFAIL") == 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "%s: Error allocating environment for program test %s.\n", argv0, name);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (control_messages && strcmp(prog_buffer, "NO_EXECUTION") == 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "%s: Program %s was not run for unknown reason.\n", argv0, name);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  diff_name(name, stream, &dname[0], sizeof(dname));
  return (do_string_diff(act, &dname[0], prog_buffer, output, -1));
}

static t_technocore_result	check_remaining_stream
(t_technocore_activity		*act,
 const char			*name,
 int				fd,
 const char			*stream,
 bool				ignore)
{
  ssize_t			ret;
  size_t			len;
  char				dname[256];

  if (ignore)
    return (TC_SUCCESS);
  len = 0;
  while ((ret = read(fd, &prog_buffer[len], sizeof(prog_buffer) - 1 - len)) > 0)
    {
      len += ret;
      if (len >= sizeof(prog_buffer) - 1)
	break ;
    }
  if (ret == -1 && errno != EAGAIN && errno != EBADF)
    return (TC_CRITICAL);
  prog_buffer[len] = '\0';
  if (len == 0)
    return (TC_SUCCESS);
  diff_name(name, stream, &dname[0], sizeof(dname));
  return (do_string_diff(act, &dname[0], prog_buffer, "", -1));
}

static t_technocore_result	interaction(const char				*argv0,
					    const char				*name,
					    t_technocore_activity		*act,
					    t_bunny_configuration		*cnf,
					    int					in,
					    int					out,
					    int					err,
					    int					maxtm)
{
  const char			*input;
  const char			*output;
  t_technocore_result		res;
  ssize_t			ret;

  if (bunny_configuration_getf(cnf, &input, "Input"))
    {
      if ((ret = strlen(input)) == 0)
	close(in);
      else if (write(in, input, ret) == -1)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot write data to program for program test %s:%s.\n",
	     argv0, name, strerror(errno));
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
    }
  if (!ignore_stream(cnf, "IgnoreOutput", "IgnoreStdout") &&
      bunny_configuration_getf(cnf, &output, "Output"))
    if ((res = read_expected_stream
	 (argv0, name, act, out, "stdout", output, maxtm, true)) != TC_SUCCESS)
      return (res);
  if (!ignore_stream(cnf, "IgnoreError", "IgnoreStderr") &&
      get_stderr_expected(cnf, &output))
    if ((res = read_expected_stream
	 (argv0, name, act, err, "stderr", output, maxtm, false)) != TC_SUCCESS)
      return (res);
  return (TC_SUCCESS);
}

static void			close_if_valid(int				*fd)
{
  if (*fd >= 0)
    {
      close(*fd);
      *fd = -1;
    }
}

t_technocore_result		start_program_activity(const char		*argv0,
					       t_bunny_configuration	*general_cnf,
					       t_bunny_configuration	*exe_cnf,
					       t_technocore_activity	*act)
{
  t_technocore_result		result = TC_SUCCESS;
  t_technocore_result		ret;
  const char			*name;
  const char			*command;
  const char			*shcall[4] = {"/bin/sh", "-c", NULL, NULL};
  int				inpipe[2] = {-1, -1};
  int				outpipe[2] = {-1, -1};
  int				errpipe[2] = {-1, -1};
  int				opid = getpid();
  pid_t				pid;
  int				return_value;
  int				timeout;
  pid_t				watchdog = -2;
  void				(*old_sigusr2)(int) = SIG_ERR;
  bool				sigusr2_installed = false;
  t_program_trace		trace;

  (void)general_cnf;
  pid = -1;
  memset(&trace, 0, sizeof(trace));
  if (bunny_configuration_getf(exe_cnf, &name, "Name") == false)
    name = bunny_configuration_get_address(exe_cnf);

  t_bunny_configuration		*temCnf;
  if (bunny_configuration_getf(exe_cnf, &temCnf, "Program") == false)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Missing Command field for program test %s.\n",
	 argv0, name);
      return (TC_CRITICAL);
    }
  // LOOP FOR PROG
  for (int j = 0; bunny_configuration_getf(exe_cnf, &temCnf, "Program[%d]", j); ++j)
    {
      bool			ignore_stdout;
      bool			ignore_stderr;

      if (bunny_configuration_getf(temCnf, &command, "Command") == false)
	{
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Missing Command field for program test %s.\n",
	     argv0, name);
	  return (TC_CRITICAL);
	}
      shcall[2] = command;
      memset(&trace, 0, sizeof(trace));

      return_value = INT_MAX;
      timeout = 2;
      pid = -1;
      sigusr2_installed = false;
      watchdog = -2;
      ignore_stdout = ignore_stream(temCnf, "IgnoreOutput", "IgnoreStdout");
      ignore_stderr = ignore_stream(temCnf, "IgnoreError", "IgnoreStderr");
      bunny_configuration_getf(temCnf, &timeout, "Timeout");
      bunny_configuration_getf(temCnf, &return_value, "ReturnValue");
      if ((ret = prepare_program_trace(argv0, name, temCnf, &trace)) != TC_SUCCESS)
	{
	  cleanup_program_trace(&trace);
	  return (ret);
	}
      // Pipe normal pour l'entrée du programme enfant, on a besoin qu'il attende
      if (pipe(inpipe) == -1)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot create input pipe for program test %s:%s.\n",
	     argv0, name, strerror(errno));
	  cleanup_program_trace(&trace);
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      // Pipes non bloquants pour les sorties du programme enfant.
      if (pipe2(outpipe, O_NONBLOCK) == -1)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot create stdout pipe for program test %s:%s.\n",
	     argv0, name, strerror(errno));
	  goto CloseInputPipe;
	} // LCOV_EXCL_STOP
      if (pipe2(errpipe, O_NONBLOCK) == -1)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot create stderr pipe for program test %s:%s.\n",
	     argv0, name, strerror(errno));
	  goto CloseOutputPipe;
	} // LCOV_EXCL_STOP

      if ((pid = fork()) == -1)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot create processsus for program test %s:%s.\n",
	     argv0, name, strerror(errno));
	  goto CloseErrorPipe;
	} // LCOV_EXCL_STOP
      else if (pid == 0)
	{ // LCOV_EXCL_START
	  t_bunny_configuration	*env;

	  close(inpipe[1]);
	  close(outpipe[0]);
	  close(errpipe[0]);
	  if (dup2(inpipe[0], STDIN_FILENO) == -1 ||
	      dup2(outpipe[1], STDOUT_FILENO) == -1 ||
	      dup2(errpipe[1], STDERR_FILENO) == -1)
	    {
	      if (write(outpipe[1], "DUPFAIL", 7)) {}
	      exit(EXIT_FAILURE);
	    }
	  if (signal(SIGTERM, siguser1) == SIG_ERR)
	    {
	      if (write(1, "SIGNALFAIL", 10)) {}
	      exit(EXIT_FAILURE);
	    }
	  apply_program_trace_environment(&trace);
	  if (bunny_configuration_getf(exe_cnf, &env, "Environ") == false)
	    execvp(shcall[0], (char**)shcall);
	  else
	    {
	      const char		**environ;
	      int			nbr_env;

	      nbr_env = bunny_configuration_casesf(env, ".") + 1;
	      if ((environ = malloc(nbr_env * sizeof(*environ))) == NULL)
		{
		  if (write(1, "MALLOCFAIL", 11)) {}
		  exit(EXIT_FAILURE);
		}
	      int			i;

	      for (i = 0; i < nbr_env; ++i)
		bunny_configuration_getf(env, &environ[i], "[%d]", i);
	      environ[i] = NULL;
	      execvpe(shcall[0], (char**)shcall, (char**)environ);
	    }
	  if (write(1, "NO_EXECUTION", 11)) {}
	  exit(EXIT_FAILURE);
	} // LCOV_EXCL_STOP
      close_if_valid(&inpipe[0]);
      close_if_valid(&outpipe[1]);
      close_if_valid(&errpipe[1]);

      if (timeout > 0)
	{
	  // On crée un processus qui va descendre le programme si il met plus de TIMEOUT a finir.
	  if ((watchdog = fork()) == -1)
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer,
		 "%s: Cannot create processsus for program test %s:%s.\n",
		 argv0, name, strerror(errno));
	      goto KillProcess;
	    } // LCOV_EXCL_STOP
	  else if (watchdog == 0)
	    { // LCOV_EXCL_START
	      // Ces pipes n'étaient pas pour nous...
	      close_if_valid(&inpipe[1]);
	      close_if_valid(&outpipe[0]);
	      close_if_valid(&errpipe[0]);
	      bunny_usleep(timeout * 1e6);
	      int out = kill(pid, SIGTERM) == -1 && errno == ESRCH ? EXIT_SUCCESS : EXIT_FAILURE;
	      kill(opid, SIGUSR2);
	      exit(out);
	    } // LCOV_EXCL_STOP
	}

      // Pour éviter d'etre coincé dans read coté evaluator - solution naze a mon avis
      __stdout = outpipe[0];
      __stderr = errpipe[0];
      if ((old_sigusr2 = signal(SIGUSR2, siguser2)) == SIG_ERR)
	goto KillProcess2; // LCOV_EXCL_LINE
      sigusr2_installed = true;

      t_bunny_configuration		*cnf;
      int				status;
      int				wstatus = EXIT_SUCCESS;
      int				program_return_value;

      for (int i = 0; bunny_configuration_getf(temCnf, &cnf, "Interactions[%d]", i); ++i)
	{
	  ret = interaction(argv0, name, act, cnf, inpipe[1], __stdout, __stderr, timeout);
	  if (ret == TC_CRITICAL)
	    goto KillProcess2; // LCOV_EXCL_LINE
	  if (ret == TC_FAILURE)
	    result = TC_FAILURE;
	}

      // On ferme l'entrée du programme. Les sorties restent ouvertes côté parent
      // jusqu'à la fin du processus, afin de pouvoir vérifier les sorties restantes.
      close_if_valid(&inpipe[1]);

      // Le watchdog qui empeche que command reste coincé dans read
      if (timeout > 0)
	{
	  kill(watchdog, SIGTERM);
	  if (waitpid_no_intr(watchdog, &wstatus) == -1)
	    goto KillProcess2;
	}
      if (sigusr2_installed)
	{
	  signal(SIGUSR2, old_sigusr2);
	  sigusr2_installed = false;
	}
      // Le programme, s'il n'est pas mort
      if (waitpid_no_intr(pid, &status) == -1)
	goto KillProcess;
      program_return_value = wait_status_to_return_value(status);

      if (result == TC_SUCCESS)
	{
	  if ((ret = check_remaining_stream
	       (act, name, __stdout, "stdout", ignore_stdout)) != TC_SUCCESS)
	    result = ret;
	  if (result == TC_SUCCESS &&
	      (ret = check_remaining_stream
	       (act, name, __stderr, "stderr", ignore_stderr)) != TC_SUCCESS)
	    result = ret;
	  if (result == TC_CRITICAL)
	    goto KillProcess;
	}
      close_if_valid(&outpipe[0]);
      close_if_valid(&errpipe[0]);

      // Si on a timeout... (ne fonctionne pas, a priori, vu que interaction
      // est maintenant responsable de cette marque
      if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == EXIT_FAILURE)
	{ // LCOV_EXCL_START
	  if (add_exercise_message(act, dict_get_pattern("Timeout"), timeout) == false)
	    {
	      add_message(&gl_technocore.error_buffer, "%s: Cannot mark timeout for program test %s.\n", argv0, name);
	      cleanup_program_trace(&trace);
	      return (TC_CRITICAL);
	    }
	  cleanup_program_trace(&trace);
	  return (TC_FAILURE);
	} // LCOV_EXCL_STOP

      // Si on a demandé à vérifier la valeur de retour...
      if (return_value != INT_MAX)
	{
	  if (return_value != program_return_value)
	    {
	      if (add_exercise_message(act, dict_get_pattern("BadReturnValue"), program_return_value, return_value) == false)
		{ // LCOV_EXCL_START
		  add_message(&gl_technocore.error_buffer, "%s: Cannot mark bad return value for program test %s.\n", argv0, name);
		  cleanup_program_trace(&trace);
		  return (TC_CRITICAL);
		} // LCOV_EXCL_STOP
	      cleanup_program_trace(&trace);
	      return (TC_FAILURE);
	    }
	}
      if (result == TC_SUCCESS && trace.enabled)
	{
	  result = check_program_trace(act, name, &trace);
	  if (result != TC_SUCCESS)
	    {
	      cleanup_program_trace(&trace);
	      return (result);
	    }
	}
      cleanup_program_trace(&trace);
    }
  // Tout est nickel, on s'en va
  return (result);

  // LCOV_EXCL_START
  // Les cas d'erreurs.
 KillProcess2:
  if (sigusr2_installed)
    {
      signal(SIGUSR2, old_sigusr2);
      sigusr2_installed = false;
    }
  if (watchdog > 0)
    kill(watchdog, SIGTERM);
 KillProcess:
  if (pid > 0)
    kill(pid, SIGTERM);
 CloseErrorPipe:
  close_if_valid(&errpipe[0]);
  close_if_valid(&errpipe[1]);
 CloseOutputPipe:
  close_if_valid(&outpipe[0]);
  close_if_valid(&outpipe[1]);
 CloseInputPipe:
  close_if_valid(&inpipe[0]);
  close_if_valid(&inpipe[1]);
  cleanup_program_trace(&trace);
  return (TC_CRITICAL);
  // LCOV_EXCL_STOP
}
