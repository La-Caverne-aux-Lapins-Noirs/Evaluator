/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<errno.h>
#include		<unistd.h>
#include		"technocore.h"

static
t_technocore_result	retrieve_results(t_technocore_activity		*act,
					 const char			*str,
					 char				*buffer,
					 size_t				l,
					 int				*dirty)
{
  FILE			*pip;
  int			cnt;

  if ((pip = popen(str, "r")) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Fail to execute '%s' for object build module (%s).\n",
		  str, strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  errno = 0;
  while ((cnt = getline(&buffer, &l, pip)) >= 0)
    {
      char		*s = strchr(buffer, '\n');

      if (s)
	*s = '\0';
      if (file_exists(buffer))
	{
	  if (add_to_current_report
	      (act, buffer, "UndesirableFound[%d]", *dirty) == false)
	    {
	      pclose(pip);
	      return (TC_CRITICAL);
	    }
	  *dirty += 1;
	}
      errno = 0;
    }
  pclose(pip);
  if (errno != 0) // eof ?
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Error while get lines for find cleanliness modules: %s.\n",
		  strerror(errno));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  return (TC_SUCCESS);
}

t_technocore_result	evaluate_cleanliness(const char			*argv,
					     t_bunny_configuration	*general_cnf,
					     t_bunny_configuration	*exe,
					     t_technocore_activity	*act)
{
  t_bunny_configuration	*cnf;
  const char		*str;
  int			dirty;

  (void)argv;
  (void)general_cnf;
  ////////////////////////////
  // Compteur d'element sales - On commence par les produits arbitraires
  dirty = 0;
  if (bunny_configuration_getf(exe, &cnf, "Products"))
    {
      if (bunny_configuration_getf(cnf, &str, "."))
	{
	  if (file_exists(str))
	    {
	      if (add_to_current_report(act, str, "UndesirableFound[%d]", dirty) == false)
		return (TC_CRITICAL);
	      dirty += 1;
	    }
	}
      else
	for (int i = 0; bunny_configuration_getf(cnf, &str, "[%d]", i); ++i)
	  if (file_exists(str))
	    {
	      if (add_to_current_report(act, str, "UndesirableFound[%d]", dirty) == false)
		return (TC_CRITICAL);
	      dirty += 1;
	    }
    }

  // On continue avec tous les fichiers indésirables...
  char			*buffer;
  size_t		l;

  l = 1024; // On malloc car getline peut realloc
  if ((buffer = malloc(l)) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Cannot allocate memory for popen for cleanliness module.\n");
      return (TC_CRITICAL);
      // LCOV_EXCL_STOP
    }
  str = "find . -name '*.o' -or -name '*~' -or -name '#*' -or -name '*.gcno' -or -name '*.gch'";
  if (retrieve_results(act, str, buffer, l, &dirty) == TC_CRITICAL)
    {
      free(buffer);
      return (TC_CRITICAL);
    }
  str = "find . -type f -exec sh -c \"nm {} 2> /dev/null | grep 'T main$' > /dev/null\" ; -print | grep -v \".*\\.o\"";
  if (retrieve_results(act, str, buffer, l, &dirty) == TC_CRITICAL)
    {
      free(buffer);
      return (TC_CRITICAL);
    }
  free(buffer);

  // On efface tout les indésirables - au cas ou il s'agisse d'une tentative
  // de casser le systeme ou de tricher (avec un .o forgé, par exemple)
  for (int i = 0; i < dirty; ++i)
    {
      bunny_configuration_getf(act->current_report, &str, "UnderisableFound[%d]", i);
      unlink(str);
    }

  /////////////////////////////////////////////
  // On en tire les conclusions qu'on souhaite
  t_technocore_result	res;
  char			tmp[512];
  const char		*medal;

  res = TC_SUCCESS;
  if (dirty == 0) // On efface le noeud indésirable vu qu'il est vide.
    {
      snprintf(&tmp[0], sizeof(tmp), dict_get_pattern("RepositoryClean"));
      bunny_delete_node(act->current_report, "UndesirableFound");
      medal = "clean";
    }
  else
    {
      int	nbr;

      if (!bunny_configuration_getf(exe, &nbr, "Tolerance"))
	nbr = 0; // LCOV_EXCL_LINE
      if (dirty > nbr)
	{
	  res = TC_FAILURE;
	  snprintf(&tmp[0], sizeof(tmp),
		   dict_get_pattern("RepositoryDirty"),
		   dirty, nbr);
	  medal = "dirty";
	}
      else
	{
	  snprintf(&tmp[0], sizeof(tmp),
		   dict_get_pattern("RepositoryCleanEnough"),
		   dirty, nbr);
	  medal = "unclean";
	}
    }
  if (!add_to_current_report(act, tmp, "Conclusion"))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Fail to write conclusion for cleanliness module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (!bunny_configuration_getf(exe, NULL, "NoMedals") &&
      !add_exercise_medal(act, medal))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Cannot add cleanliness medal.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  return (res);
}
