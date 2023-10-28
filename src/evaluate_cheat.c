/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

// -1: interdit, 0: ne sait pas, 1: autorisé
static int		is_cheater(const char				*func,
				   t_bunny_configuration		*auth,
				   t_bunny_configuration		*forb)
{
  bool			whitelist;
  bool			blacklist;

  blacklist = (forb != NULL && bunny_configuration_childrenf(forb, ".") != 0);
  whitelist = (auth != NULL && bunny_configuration_childrenf(auth, ".") != 0);

  // Rien n'est autorisé ou interdit => Tout est autorisé, sauf contre indication d'autre conf
  if (!blacklist && !whitelist)
    return (0);

  // Tout est autorisé sauf.
  if (!whitelist && blacklist)
    // Si je trouve, c'est interdit. Sinon on verra avec l'autre conf.
    return (bunny_configuration_getf(forb, NULL, "%s", func) ? -1 : 0);

  // Rien n'est autorisé sauf.
  if (whitelist && !blacklist)
    // Si je trouve pas, c'est interdit. Sinon on verra avec l'autre conf.
    return (!bunny_configuration_getf(auth, NULL, "%s", func) ? -1 : 0);

  // Si ce n'est pas autorisé, c'est interdit
  if (bunny_configuration_getf(auth, NULL, "%s", func) == false)
    return (-1);
  // Si c'est interdit, c'est interdit.
  return (bunny_configuration_getf(forb, NULL, "%s", func) ? -1 : 1);
}

static bool		is_authorized(const char			*func,
				      t_bunny_configuration		*gen,
				      t_bunny_configuration		*exe)
{
  t_bunny_configuration	*auth;
  t_bunny_configuration	*forb;
  int			a = 0;
  int			b = 0;

  // Si il n'y a pas de fonctions autorisé, c'est qu'elles sont toutes interdites.
  // Si il n'y a pas de fonctions interdites, c'est qu'elles sont toutes autorisées.
  if (gen)
    { // LCOV_EXCL_START
      if (!bunny_configuration_getf(gen, &auth, "AuthorizedFunctions"))
	auth = NULL;
      if (!bunny_configuration_getf(gen, &forb, "ForbiddenFunctions"))
	forb = NULL;
      // Global
      a = is_cheater(func, auth, forb);
    } // LCOV_EXCL_STOP
  if (exe)
    {
      if (!bunny_configuration_getf(exe, &auth, "AuthorizedFunctions"))
	auth = NULL;
      if (!bunny_configuration_getf(exe, &forb, "ForbiddenFunctions"))
	forb = NULL;
      // Local (il l'emporte sur le global)
      b = is_cheater(func, auth, forb);
    }

  if (b == 0) // Si le local ne sait pas, le global l'emporte.
    return (a != -1);
  return (b != -1);
}

t_technocore_result	evaluate_cheat(const char			*argv,
				       t_bunny_configuration		*general_cnf,
				       t_bunny_configuration		*exe,
				       t_technocore_activity		*act)
{
  t_technocore_result	res;
  char			buffer[1024 * 16];
  int			siz = sizeof(buffer);
  bool			demangle = false;

  (void)argv;
  res = TC_SUCCESS;
  bunny_configuration_getf(exe, &demangle, "DemangleSymbols");

  if (tcpopen("cheat", "gcc -shared -o libcheat.so -fPIC `find . -name '*.o'`", &buffer[0], &siz, NULL, 0) != 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Cannot list object for the cheating module %s.\n", bunny_configuration_get_address(exe));
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  const char		*tokens[4] = {"\r", "\n", " ", NULL};
  const char * const	*split;
  char			cmd[512];

  snprintf(&cmd[0], sizeof(cmd),
	   "nm -u %s ./libcheat.so | "
	   "grep ' U ' | "
	   "tr -s ' ' | "
	   "cut -d ' ' -f 3 | "
	   "cut -d '(' -f 1 | "
	   "cut -d '@' -f 1",
	   demangle ? "-C" : ""
	   );
  siz = sizeof(buffer);
  if (tcpopen("cheat nm", &cmd[0], &buffer[0], &siz, NULL, 0) != 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot list function in object file for the cheating module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if ((split = bunny_split(&buffer[0], &tokens[0], true)) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Fail to split function list for the cheating module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  for (size_t j = 0; split[j]; ++j)
    {
      const char	*specials[] = {"_GLOBAL_OFFSET_TABLE_"};
      size_t	k;

      for (k = 0; k < NBRCELL(specials) && strcmp(split[j], specials[k]); ++k);
      if (k != NBRCELL(specials))
	continue ; // LCOV_EXCL_LINE

      if (is_authorized(split[j], general_cnf, exe) == false)
	{
	  add_to_current_report(act, "", "ForbiddenFunctions.%s", split[j]);
	  res = TC_FAILURE;
	}
    }
  if (res == TC_FAILURE)
    add_to_current_report(act, dict_get_pattern("ForbiddenFunction"), "Conclusion");
  else
    add_to_current_report(act, dict_get_pattern("NoForbiddenFunction"), "Conclusion");

  bunny_delete_split(split);
  return (res);
}

