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
#include			"technocore.h"

static void			siguser(int					x)
{
  if (x == SIGUSR1)
    if (write(1, "TIMEOUT", 7)) {}
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

  if (bunny_configuration_getf(cnf, &input, "Input"))
    {
      if (write(in, input, strlen(input)) == -1)
	{
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot write data to program for program test %s:%s.",
	     argv0, name, strerror(errno));
	  return (TC_CRITICAL);
	}
    }
  if (bunny_configuration_getf(cnf, &output, "Output"))
    {
      ssize_t			ret;

      if ((ret = read(out, bunny_big_buffer, sizeof(bunny_big_buffer) - 1)) == -1)
	{
	  add_message
	    (&gl_technocore.error_buffer,
	     "%s: Cannot read data from program for program test %s:%s.",
	     argv0, name, strerror(errno));
	  return (TC_CRITICAL);
	}
      bunny_big_buffer[ret] = '\0';

      if (strcmp(bunny_big_buffer, "TIMEOUT") == 0)
	{
	  if (add_exercise_message(act, dict_get_pattern("Timeout"), maxtm) == false)
	    {
	      add_message(&gl_technocore.error_buffer, "%s: Cannot mark timeout for program test %s.", argv0, name);
	      return (TC_CRITICAL);
	    }
	  return (TC_FAILURE);
	}
      if (strcmp(bunny_big_buffer, "DUPFAIL") == 0)
	{
	  add_message(&gl_technocore.error_buffer, "%s: Error while duping fds in program test %s.", argv0, name);
	  return (TC_CRITICAL);
	}
      if (strcmp(bunny_big_buffer, "SIGNALFAIL") == 0)
	{
	  add_message(&gl_technocore.error_buffer, "%s: Error while setting sighandler in program test %s.", argv0, name);
	  return (TC_CRITICAL);
	}
      if (strcmp(bunny_big_buffer, "MALLOCFAIL") == 0)
	{
	  add_message(&gl_technocore.error_buffer, "%s: Error allocating environment for program test %s.", argv0, name);
	  return (TC_CRITICAL);
	}
      if (strcmp(bunny_big_buffer, "NO_EXECUTION") == 0)
	{
	  add_message(&gl_technocore.error_buffer, "%s: Program %s was not run for unknown reason.", argv0, name);
	  return (TC_CRITICAL);
	}

      return (do_string_diff(act, name, bunny_big_buffer, output, -1));
    }
  return (TC_SUCCESS);
}

t_technocore_result		start_program_activity(const char		*argv0,
						       t_bunny_configuration	*general_cnf,
						       t_bunny_configuration	*exe_cnf,
						       t_technocore_activity	*act)
{
  const char			*name;
  const char			*command;
  const char * const		*split;
  const char			*tokens[] = {" ", "\t", "\n"};
  int				inpipe[2];
  int				outpipe[2];
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
	 "%s: Missing Command field for program test %s.",
	 argv0, name);
      return (TC_CRITICAL);
    }
  bunny_configuration_getf(exe_cnf, &timeout, "Timeout");
  bunny_configuration_getf(exe_cnf, &return_value, "ReturnValue");
  if ((split = bunny_split(command, tokens, true)) == NULL)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot general execv format by splitting command for program test %s.",
	 argv0, name);
      return (TC_CRITICAL);
    }
  if (pipe(inpipe) == -1)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot create input pipe for program test %s:%s.",
	 argv0, name, strerror(errno));
      goto DeleteSplit;
    }
  if (pipe(outpipe) == -1)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot create output pipe for program test %s:%s.",
	 argv0, name, strerror(errno));
      goto CloseInputPipe;
    }

  if ((pid = fork()) == -1)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot create processsus for program test %s:%s.",
	 argv0, name, strerror(errno));
      goto CloseOutputPipe;
    }
  else if (pid == 0)
    {
      t_bunny_configuration	*env;

      close(inpipe[1]);
      close(outpipe[0]);
      if (dup2(inpipe[0], STDIN_FILENO) == -1 || dup2(outpipe[1], STDOUT_FILENO) == -1)
	{
	  if (write(outpipe[1], "DUPFAIL", 7)) {}
	  exit(EXIT_FAILURE);
	}
      if (signal(SIGUSR1, siguser) == NULL)
	{
	  if (write(1, "SIGNALFAIL", 10)) {}
	  exit(EXIT_FAILURE);
	}
      if (bunny_configuration_getf(exe_cnf, &env, "Environ") == false)
	execvp(split[0], (char**)split);
      else
	{
	  const char		**environ;
	  int			nbr_env;

	  nbr_env = bunny_configuration_casesf(env, ".") + 1;
	  if ((environ = bunny_malloc(nbr_env * sizeof(*environ))) == NULL)
	    {
	      if (write(1, "MALLOCFAIL", 11)) {}
	      exit(EXIT_FAILURE);
	    }
	  int			i;

	  for (i = 0; i < nbr_env; ++i)
	    bunny_configuration_getf(env, &environ[i], "[%d]", i);
	  environ[i] = NULL;
	  execvpe(split[0], (char**)split, (char**)environ);
	}
      if (write(1, "NO_EXECUTION", 11)) {}
      exit(EXIT_FAILURE);
    }
  pid_t				watchdog;

  // On crée un processus qui va descendre le programme si il met plus de TIMEOUT a finir.
  if ((watchdog = fork()) == -1)
    {
      add_message
	(&gl_technocore.error_buffer,
	 "%s: Cannot create processsus for program test %s:%s.",
	 argv0, name, strerror(errno));
      goto CloseOutputPipe;
    }
  else if (watchdog == 0)
    {
      close(inpipe[0]);
      close(outpipe[1]);
      bunny_usleep(timeout * 1e6);
      kill(pid, SIGTERM);
      exit(EXIT_SUCCESS);
    }


  t_bunny_configuration		*cnf;
  int				status;

  close(inpipe[0]);
  close(outpipe[1]);
  for (int i = 0; bunny_configuration_getf(exe_cnf, &cnf, "Interactions[%d]", i); ++i)
    if (interaction(argv0, name, act, cnf, inpipe[1], outpipe[0], timeout) == TC_CRITICAL)
      goto KillProcess;

  if (return_value != INT_MAX)
    {
      waitpid(pid, &status, 0);
      if (return_value != status)
	{
	  if (add_exercise_message(act, dict_get_pattern("BadReturnValue"), status, return_value) == false)
	    {
	      add_message(&gl_technocore.error_buffer, "%s: Cannot mark bad return value for program test %s.", argv0, name);
	      return (TC_CRITICAL);
	    }
	  return (TC_FAILURE);
	}
    }

  close(inpipe[1]);
  close(outpipe[0]);
  bunny_delete_split(split);
  return (TC_SUCCESS);

 KillProcess:
  kill(pid, SIGTERM);
 CloseOutputPipe:
  close(outpipe[0]);
  close(outpipe[1]);
 CloseInputPipe:
  close(inpipe[0]);
  close(inpipe[1]);
 DeleteSplit:
  bunny_delete_split(split);
  return (TC_CRITICAL);
}
