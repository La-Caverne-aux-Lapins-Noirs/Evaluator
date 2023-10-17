/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define				_GNU_SOURCE
#include			<limits.h>
#include			<unistd.h>
#include			<signal.h>
#include			<errno.h>
#include			<sys/wait.h>
#include			<errno.h>
#include			"technocore.h"

int				__stdin;
int				__stdout;

static void			siguser1(int					x)
{
  close(__stdin);
  exit(x);
}

static void			siguser2(int					x)
{
  close(__stdout);
  return ;
}

static t_technocore_result	interaction(const char				*argv0,
					    const char				*name,
					    t_technocore_activity		*act,
					    t_bunny_configuration		*cnf,
					    int					in,
					    int					out,
					    int					maxtm)
{
  const char			*input;
  const char			*output;
  int				len;

  if (bunny_configuration_getf(cnf, &input, "Input"))
    {
      if ((len = strlen(input)) == 0)
	close(in);
      else if (write(in, input, len) == -1)
	{ // LCOV_EXCL_START
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot write data to program for program test %s:%s.\n",
	     argv0, name, strerror(errno));
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
    }
  if (bunny_configuration_getf(cnf, &output, "Output"))
    {
      ssize_t			ret;

      if ((ret = read(out, bunny_big_buffer, sizeof(bunny_big_buffer) - 1)) == -1)
	{
	  if (errno == EINTR || errno == EBADF)
	    {
	      strcpy(bunny_big_buffer, "TIMEOUT");
	      ret = 7;
	    }
	  else
	    { // LCOV_EXCL_START
	      add_message
		(&gl_technocore.error_buffer,
		 "%s: Cannot read data from program for program test %s:%s.\n",
		 argv0, name, strerror(errno));
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	}
      bunny_big_buffer[ret] = '\0';

      if (ret == 0)
	{
	  if (*output != '\0')
	    return (do_string_diff(act, name, bunny_big_buffer, "EOF", -1)); // LCOV_EXCL_LINE
	  return (TC_SUCCESS);
	}
      if (strcmp(bunny_big_buffer, "TIMEOUT") == 0)
	{ // LCOV_EXCL_START
	  if (add_exercise_message(act, dict_get_pattern("Timeout"), maxtm) == false)
	    {
	      add_message(&gl_technocore.error_buffer, "%s: Cannot mark timeout for program test %s.\n", argv0, name);
	      return (TC_CRITICAL);
	    }
	  return (TC_FAILURE);
	} // LCOV_EXCL_STOP
      if (strcmp(bunny_big_buffer, "DUPFAIL") == 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "%s: Error while duping fds in program test %s.\n", argv0, name);
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if (strcmp(bunny_big_buffer, "SIGNALFAIL") == 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "%s: Error while setting sighandler in program test %s.\n", argv0, name);
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if (strcmp(bunny_big_buffer, "MALLOCFAIL") == 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "%s: Error allocating environment for program test %s.\n", argv0, name);
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if (strcmp(bunny_big_buffer, "NO_EXECUTION") == 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "%s: Program %s was not run for unknown reason.\n", argv0, name);
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP

      return (do_string_diff(act, name, bunny_big_buffer, output, -1));
    }
  return (TC_SUCCESS);
}

t_technocore_result		start_program_activity(const char		*argv0,
						       t_bunny_configuration	*general_cnf,
						       t_bunny_configuration	*exe_cnf,
						       t_technocore_activity	*act)
{
  t_technocore_result		result;
  const char			*name;
  const char			*command;
  const char			*shcall[4] = {"/bin/sh", "-c", NULL, NULL};
  int				inpipe[2];
  int				outpipe[2];
  int				opid = getpid();
  pid_t				pid;
  int				return_value = INT_MAX;
  int				timeout = 2;

  (void)general_cnf;

  if (bunny_configuration_getf(exe_cnf, &name, "Name") == false)
    name = bunny_configuration_get_address(exe_cnf);
  if (bunny_configuration_getf(exe_cnf, &command, "Command") == false)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Missing Command field for program test %s.\n",
	 argv0, name);
      return (TC_CRITICAL);
    }
  shcall[2] = command;
  bunny_configuration_getf(exe_cnf, &timeout, "Timeout");
  bunny_configuration_getf(exe_cnf, &return_value, "ReturnValue");
  if (pipe(inpipe) == -1)
    { // LCOV_EXCL_START
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot create input pipe for program test %s:%s.\n",
	 argv0, name, strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (pipe(outpipe) == -1)
    { // LCOV_EXCL_START
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot create output pipe for program test %s:%s.\n",
	 argv0, name, strerror(errno));
      goto CloseInputPipe;
    } // LCOV_EXCL_STOP

  if ((pid = fork()) == -1)
    { // LCOV_EXCL_START
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot create processsus for program test %s:%s.\n",
	 argv0, name, strerror(errno));
      goto CloseOutputPipe;
    } // LCOV_EXCL_STOP
  else if (pid == 0)
    { // LCOV_EXCL_START
      t_bunny_configuration	*env;

      close(inpipe[1]);
      close(outpipe[0]);
      if (dup2(inpipe[0], STDIN_FILENO) == -1 || dup2(outpipe[1], STDOUT_FILENO) == -1)
	{
	  if (write(outpipe[1], "DUPFAIL", 7)) {}
	  exit(EXIT_FAILURE);
	}
      if (signal(SIGTERM, siguser1) == SIG_ERR)
	{
	  if (write(1, "SIGNALFAIL", 10)) {}
	  exit(EXIT_FAILURE);
	}
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
  close(inpipe[0]);
  close(outpipe[1]);
  pid_t				watchdog;

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
      close(inpipe[1]);
      close(outpipe[0]);
      bunny_usleep(timeout * 1e6);
      int out = kill(pid, SIGTERM) == -1 && errno == ESRCH ? EXIT_SUCCESS : EXIT_FAILURE;
      kill(opid, SIGUSR2);
      exit(out);
    } // LCOV_EXCL_STOP

  // Pour éviter d'etre coincé dans read coté evaluator - solution naze a mon avis
  __stdout = outpipe[0];
  if (signal(SIGUSR2, siguser2) == SIG_ERR)
    goto KillProcess2; // LCOV_EXCL_LINE

  t_bunny_configuration		*cnf;
  int				status;
  int				wstatus;

  for (int i = 0; bunny_configuration_getf(exe_cnf, &cnf, "Interactions[%d]", i); ++i)
    if ((result = interaction(argv0, name, act, cnf, inpipe[1], outpipe[0], timeout)) == TC_CRITICAL)
      goto KillProcess2; // LCOV_EXCL_LINE

  // On ferme les moyens de communication
  close(inpipe[1]);
  close(outpipe[0]);

  // Le watchdog qui empeche que command reste coincé dans read
  kill(watchdog, SIGTERM);
  waitpid(watchdog, &wstatus, 0);
  // Le programme, s'il n'est pas mort
  waitpid(pid, &status, 0);

  // Si on a timeout... (ne fonctionne pas, a priori, vu que interaction
  // est maintenant responsable de cette marque
  if (wstatus == EXIT_FAILURE)
    { // LCOV_EXCL_START
      if (add_exercise_message(act, dict_get_pattern("Timeout"), timeout) == false)
	{
	  add_message(&gl_technocore.error_buffer, "%s: Cannot mark timeout for program test %s.\n", argv0, name);
	  return (TC_CRITICAL);
	}
      return (TC_FAILURE);
    } // LCOV_EXCL_STOP
  
  // Si on a demandé à vérifier la valeur de retour...
  if (return_value != INT_MAX)
    {
      if (return_value != status)
	{
	  if (add_exercise_message(act, dict_get_pattern("BadReturnValue"), status, return_value) == false)
	    { // LCOV_EXCL_START
	      add_message(&gl_technocore.error_buffer, "%s: Cannot mark bad return value for program test %s.\n", argv0, name);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  return (TC_FAILURE);
	}
    }
  // Tout est nickel, on s'en va
  return (result);

  // LCOV_EXCL_START
  // Les cas d'erreurs.
 KillProcess2:
  kill(watchdog, SIGTERM);
 KillProcess:
  kill(pid, SIGTERM);
 CloseOutputPipe:
  close(outpipe[0]);
  close(outpipe[1]);
 CloseInputPipe:
  close(inpipe[0]);
  close(inpipe[1]);
  return (TC_CRITICAL);
  // LCOV_EXCL_STOP
}
