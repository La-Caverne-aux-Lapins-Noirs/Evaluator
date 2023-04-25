/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<stdio.h>
#include		<errno.h>
#include		<unistd.h>
#include		"technocore.h"

t_technocore_result	evaluate_object_build(const char		*argv,
					      t_bunny_configuration	*general_cnf,
					      t_bunny_configuration	*exe,
					      t_technocore_activity	*act)
{
  char			build_command[1024];
  const char		*build_cmd;
  const char		*search_command;
  bool			bol;

  (void)argv;
  (void)general_cnf;
  //////////////////////////////////////////////////////////////
  // Commpilateur de fichier .c (ou autre) séparément, soit vers des .o soit
  // vers des programmes
  if (!bunny_configuration_getf(exe, &search_command, "SearchCommand"))
    search_command = "find . -name \"*.c\"";
  if (!bunny_configuration_getf(exe, &build_cmd, "BuildCommand"))
    {
      if (!bunny_configuration_getf(exe, &bol, "Link"))
	bol = false;
      if (bol)
	build_cmd = "gcc %s -o %1$s.out -I./ -I./include/ -I/usr/local/include/";
      else
	build_cmd = "gcc -L/usr/local/lib/ -c %s -I./ -I./include -I/usr/local/include/";
    }
  // On redirige les erreurs vers la sortie standard
  snprintf(&build_command[0], sizeof(build_command), "%s 2>&1", build_cmd);
  char			tmp[2048];
  char			*buffer;
  FILE			*search_pipe;
  int			warning;
  int			error;
  int			cnt;
  size_t		l;

  l = 8128;
  warning = 0;
  error = 0;
  if ((buffer = malloc(l)) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot allocate memory for popen for object build module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  // On va commencer par récupérer tous les fichiers
  // Les types de commande attendues sont du style
  // echo src/*.c
  // find . -name "*.c"
  if ((search_pipe = popen(search_command, "r")) == NULL)
    { // LCOV_EXCL_START
      free(buffer);
      add_message(&gl_technocore.error_buffer,
		  "Fail to execute '%s' for object build module (%s).\n",
		  search_command, strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  errno = 0;
  while ((cnt = getline(&buffer, &l, search_pipe)) >= 0)
    {
      // On a des fichiers sur la ligne, normalement. Au moins un.
      const char * const * splitted;
      const char *toks[4] = {" ", "\n", "\r", NULL};

      if ((splitted = bunny_split(&buffer[0], toks, true)) == NULL)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer,
		      "Fail to retrieve .o ofiles for object build module.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      // On parcoure les fichiers trouvé sur la ligne
      for (size_t x = 0; splitted[x]; ++x)
	{
	  FILE		*build_pipe;
	  size_t	red;
	  int		ret;

	  snprintf(&tmp[0], sizeof(tmp), build_command, splitted[x]);
	  if ((build_pipe = popen(&tmp[0], "r")) == NULL)
	    { // LCOV_EXCL_START
	      add_message(&gl_technocore.error_buffer,
			  "Fail to command construction of %s.\n", splitted[x]);
	      bunny_delete_split(splitted);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  red = fread(&tmp[0], 1, sizeof(tmp), build_pipe);
	  if (ferror(build_pipe))
	    { // LCOV_EXCL_START
	      add_message(&gl_technocore.error_buffer,
			  "Fail to read from command construction of %s.\n", splitted[x]);
	      bunny_delete_split(splitted);
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  ret = pclose(build_pipe);
	  // On a lu quelque chose (des erreurs...) ou ca a échoué
	  if (red != 0 || ret != 0)
	    {
	      // On enregistre le fichier que l'on essayait de compiler.
	      if (add_to_current_report
		  (act, splitted[x], "FailedObject[%d].File", warning) == false)
		return (TC_CRITICAL);
	      const char	*str;

	      if (ret != 0)
		{
		  str = "Error";
		  error += 1;
		}
	      else
		str = "Warning";
	      if (add_to_current_report
		  (act, str, "FailedObject[%d].Status", warning) == false)
		return (TC_CRITICAL);
	      if (add_to_current_report
		  (act, &tmp[0], "FailedObject[%d].Message", warning) == false)
		return (TC_CRITICAL);
	      warning += 1;
	    }
	}
      bunny_delete_split(splitted);
      errno = 0;
    }
  free(buffer);
  pclose(search_pipe);
  if (errno != 0) // eof ?
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Error while get lines for find object modules: %s.\n",
		  strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  t_technocore_result	res;

  res = TC_SUCCESS;
  if (warning == 0 && error == 0)
    {
      snprintf(&tmp[0], sizeof(tmp), dict_get_pattern("ObjectBuildOk"));
      bunny_delete_node(act->current_report, "FailedObject");
    }
  else
    {
      int	nbr;

      if (!bunny_configuration_getf(exe, &nbr, "Tolerance"))
	nbr = 0;
      if (error != 0)
	{
	  res = TC_FAILURE;
	  snprintf(&tmp[0], sizeof(tmp),
		   dict_get_pattern("ObjectBuildFail"),
		   error, warning - error, nbr);
	}
      else if (warning > nbr)
	{
	  res = TC_FAILURE;
	  snprintf(&tmp[0], sizeof(tmp),
		   dict_get_pattern("ObjectBuildBad"),
		   warning, nbr);
	}
      else
	snprintf(&tmp[0], sizeof(tmp),
		 dict_get_pattern("ObjectBuildGoodEnough"),
		 warning, nbr);
    }
  if (!add_to_current_report(act, tmp, "Conclusion"))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Fail to write conclusion for object build module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  return (res);
}

