/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

extern const char	*fieldname_first_char;

t_technocore_result	start_activity(const char			*argv0,
				       t_bunny_configuration		*cnf)
{
  // On récupère le langage de l'utilisateur
  const char		*language;

  language = "EN";
  bunny_configuration_getf(cnf, &language, "Language");
  if (dict_set_language(language) == false)
    { // LCOV_EXCL_START
      fprintf(stderr, "%s: Cannot load language.\n", argv0);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  // On nettoie l'allocateur mémoire et on place la limite de RAM. Par défaut, c'est 2 mega.
  int			i;

  i = 1024 * 1024 * 2;
  bunny_allocator_reset();
  bunny_configuration_getf(cnf, &i, "MaximumRam");
  bunny_set_maximum_ram(i);

  // Pour chaque exercice.
  char			buffer[64];
  t_technocore_activity	tech;
  t_bunny_configuration	*act;
  t_bunny_configuration	*rot;
  t_technocore_result	res;
  const char		*str;
  int			failedcnt;
  int			excnt;

  failedcnt = 0;
  res = TC_CRITICAL;
  rot = bunny_configuration_get_root(cnf);
  memset(&tech, 0, sizeof(tech));
  if ((tech.report = bunny_new_configuration()) == NULL)
    { // LCOV_EXCL_START
      fprintf(stderr, "%s: Cannot create a new configuration for report.\n", argv0);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  excnt = 0;
  for (i = 0; bunny_configuration_getf(cnf, &act, "Exercises[%d]", i); ++i)
    {
      if (bunny_configuration_getf(cnf, &str, "."))
	continue ; // LCOV_EXCL_LINE Si le noeud est une chaine de caractère, c'est une directive pour docbuilder...

      // Condition pour executer le bloc
      if (bunny_configuration_getf(act, &str, "ConditionalVar"))
	{
	  ssize_t	p = 0;
	  int		cond;
	  
	  if (bunny_read_char(str, &p, fieldname_first_char) == false || str[p] != '\0')
	    {
	      fprintf(stderr, "%s: Invalid ConditionalVar field %s.\n", argv0, str);
	      return (TC_CRITICAL);
	    }
	  if (!bunny_configuration_getf(rot, &cond, "Variables.%s", str) || !cond)
	    continue ;
	}
      
      // On récupère le nom de l'activité et on prépare un noeud pour le rapport
      if (bunny_configuration_getf(act, &str, "Name") == false)
	{ // LCOV_EXCL_START
	  snprintf(&buffer[0], sizeof(buffer), dict_get_pattern("Exercise"), i);
	  str = &buffer[0];
	} // LCOV_EXCL_STOP

      if (!bunny_configuration_getf(act, NULL, "NoReport"))
	{
	  // On écrit le nom afin de générer le noeud
	  if (bunny_configuration_setf(tech.report, str, "Exercises[%d].Name", excnt) == false)
	    { // LCOV_EXCL_START
	      fprintf(stderr, "%s: Cannot create report node for activity %s.\n", argv0, str);
	      goto DeleteTA;
	    } // LCOV_EXCL_STOP
	  // On récupère le noeud
	  bunny_configuration_getf(tech.report, &tech.current_report, "Exercises[%d]", excnt);
	  excnt += 1;
	}
      else
	tech.current_report = NULL;

      // Le type par défaut d'exercce, c'est "Function".
      if (bunny_configuration_getf(act, &str, "Type") == false)
	str = "Function";

      // On corrige une fonction, il faut donc charger une bibliothèque dynamique
      if (bunny_strcasecmp(str, "Function") == 0)
	res = start_function_activity(argv0, cnf, act, &tech);
      // On corrige un programme.
      else if (bunny_strcasecmp(str, "Program") == 0)
	res = start_program_activity(argv0, cnf, act, &tech);
      else if (bunny_strcasecmp(str, "Builtin") == 0)
	res = start_builtin_activity(argv0, cnf, act, &tech);
      else
	{  // LCOV_EXCL_START
	  fprintf(stderr,
		  "%s: Invalid type %s specified in configuration for %s.\n",
		  argv0, str, bunny_configuration_get_address(act));
	  goto DeleteTA;
	} // LCOV_EXCL_STOP

      // En cas d'erreur interne, on arrête tout.
      if (res == TC_CRITICAL)
	goto DeleteTA;  // LCOV_EXCL_LINE

      if (tech.current_report)
	if (!bunny_configuration_getf(tech.current_report, NULL, "Status"))
	  bunny_configuration_setf(tech.current_report, res == TC_FAILURE ? "Failure" : "Success", "Status");
      
      // En cas d'erreur de l'élève, on regarde ce que dit la configuration
      if (res == TC_FAILURE)
	{
	  bool		stop_on_failure = false;
	  int		max_failure = -1;

	  failedcnt += 1;
	  // Si l'exercice est bloquant, on arrête la.
	  if (bunny_configuration_getf(act, &stop_on_failure, "StopOnFailure") == false)
	    bunny_configuration_getf(cnf, &stop_on_failure, "StopOnFailure");
	  // On peut aussi tolérer une quantité d'erreur determinée
	  bunny_configuration_getf(cnf, &max_failure, "MaximumFailure");
	  if (max_failure > -1 && failedcnt > max_failure)
	    stop_on_failure = true;
	  if (stop_on_failure)
	    goto DeleteTA;
	}

      // Succès, on regarde si une action à effectuer
      if (res == TC_SUCCESS && bunny_configuration_getf(act, &str, "SetVar"))
	{
	  ssize_t	p = 0;
	  
	  if (bunny_read_char(str, &p, fieldname_first_char) == false || str[p] != '\0')
	    {
	      fprintf(stderr, "%s: Invalid SetVar field %s.\n", argv0, str);
	      return (TC_CRITICAL);
	    }
	  bunny_configuration_setf(rot, 1, "Variables.%s", str);
	}
    }

  // On quitte tout dossier qu'on aurait rejoins, soit parcequ'on
  // est pas remonté normalement soit parcequ'on a quitté trop tot
  // a cause d'un echec d'évaluation (critical comme failure)
  while (tech.nbr_path > 0)
    if (chdir(tech.pathstack[--tech.nbr_path]) != 0)
      {
	add_message
	  (&gl_technocore.error_buffer,
	   "%s: Cannot rewind path to %s at the end of evaluation. %s.\n",
	   argv0,
	   tech.pathstack[tech.nbr_path],
	   strerror(errno)
	   );
	return (TC_CRITICAL);
      }
  
  // On construit le bilan de l'évaluation.
  if (build_report(&tech) == false)
    goto DeleteTA;

  // Tout s'est bien passé.
  clear_technocore_activity(&tech);
  return (TC_SUCCESS);

 DeleteTA:
  clear_technocore_activity(&tech);
  return (res);
}

