/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<fnmatch.h>
#include		<stdio.h>
#include		<errno.h>
#include		<unistd.h>
#include		"technocore.h"

/////////////////////////////
// Les règles imposées sont:
// all, clean, fclean, re, check et install.
////////////////////////////
// Les variables imposées sont:
// INSTALL_BIN_DIR si c'est un programme, par défaut /usr/local/bin
// INSTALL_LIB_DIR si c'est une bibliothèque, par défaut /usr/local/lib
// INSTALL_INC_DIR si c'est une bibliothèque, par défaut /usr/local/include
// DEBUG, valant par défaut 'n'.
//////////////////////////
// Le comportement imposé est:
// Un fichier avec un contenu non modifié ne doit pas être recompilé
// Le produit final de compilation ne doit pas être relinké
// Le dossier de test est "./tests"

static t_technocore_result report(t_technocore_activity		*act,
				  t_bunny_configuration		*exe,
				  t_technocore_result		res,
				  const char			*sub,
				  const char			*med)
{
  bool ret = true;

  ret = add_to_current_report(act, dict_get_pattern(sub), "Conclusion") && ret;

  if (!bunny_configuration_getf(exe, NULL, "NoMedals"))
    ret = add_exercise_medal(act, med) && ret;
  if (ret)
    return (res);
  // LCOV_EXCL_START
  add_message(&gl_technocore.error_buffer, "Fail to write conclusion or medal for make module (%s).\n", sub);
  return (TC_CRITICAL);
  // LCOV_EXCL_STOP
}

static t_technocore_result try_make(t_technocore_activity	*act,
				    t_bunny_configuration	*exe)
{
  if (system("make") == 0)
    return (TC_SUCCESS);
  return (report(act, exe, TC_FAILURE, "MakeFailed", "not_build"));
}

t_technocore_result	evaluate_make_build(const char			*argv,
					    t_bunny_configuration	*gen,
					    t_bunny_configuration	*exe,
					    t_technocore_activity	*act)
{
  t_technocore_result	res;
  const char		*tokens[3] = {"\r", "\n", NULL};
  const char		*product_name;
  char			command[1024];
  const char		*str;
  bool			check;
  int			log;
  size_t		i;

  // L'état de départ est: le dossier est propre, cleanliness a été passé.
  log = 0;
  (void)gen;
  (void)argv;

  ///////////////////////////////////////////////////////
  // On commence par vérifier l'existence d'un makefile.
  if (!file_exists("Makefile"))
    return (report(act, exe, TC_FAILURE, "MissingMakefile", "no_makefile"));
  add_to_current_report(act, dict_get_pattern("MakefilePresent"), "Steps[%d]", log++);

  /////////////////////////////////////////
  // On tente maintenant une construction.
  char			buffer[1024];
  int			x = sizeof(buffer);

  if ((res = try_make(act, exe)) != TC_SUCCESS)
    return (res);
  // On vérifie si le programme demandé a bien été produit et qu'il y a bien des fichiers .o
  if (!bunny_configuration_getf(exe, &product_name, "ProductName"))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Missing ProductName field for make module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (!file_exists(product_name))
    return (report(act, exe, TC_FAILURE, "ProductWasNotBuilt", "not_build"));
  if (tcpopen("make module", "find . -name \"*.o\"", &buffer[0], &x, NULL, 0) != 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Fail to list object file for make module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  // Est ce qu'il y a au moins quelques fichiers de remonté?
  if (strstr(&buffer[0], ".o") == NULL) // Non
    return (report(act, exe, TC_FAILURE, "NoObjectWereProduced", "not_build"));
  add_to_current_report(act, dict_get_pattern("ProductProduced"), "Steps[%d]", log++);

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  // On essaie maintenant de reconstruire le projet, mais d'abord, on enregistre la date de création
  // du produit et de tous les fichiers .o pour verifier que rien n'a changé.
  const char * const	*split;
  const char * const	*split2;
  char			buffer2[1024];
  int			y = sizeof(buffer2);

  x = sizeof(buffer);
  snprintf(&command[0], sizeof(command), "ls -l --time-style=full-iso `find . -name '*.o' -or -name '%s'`", product_name);
  if (tcpopen("make module", &command[0], &buffer[0], &x, NULL, 0) != 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Fail to list object and product file for make module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  // On en aura besoin plusieurs fois.
  if ((split = bunny_split(&buffer[0], &tokens[0], true)) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Fail to split object and product list for make module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (!bunny_configuration_getf(exe, &check, "CheckBehaviour") || check)
    {
      // On tente maintenant une construction qui devrait normalement ne rien faire
      if ((res = try_make(act, exe)) != TC_SUCCESS)
	return (res);
      // C'est reparti, on enregistre dans buffer 2.
      if (tcpopen("make module", &command[0], &buffer2[0], &y, NULL, 0) != 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Fail to list object and product file for make module.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if ((split2 = bunny_split(&buffer2[0], &tokens[0], true)) == NULL)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Fail to split object and product list for make module.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      // Et normalement, c'est exactement pareil - sinon c'est qu'il y a eu un probleme
      for (i = 0; split[i] && split2[i]; ++i)
	if (strcmp(split[i], split2[i]))
	  return (report(act, exe, TC_FAILURE, "DoubleMakeFailed", "makefile_invalid_dependencies"));
      // Si on arrive la, c'est que tout matchait. GG.
      bunny_delete_split(split2);
      add_to_current_report(act, dict_get_pattern("MakefileOptimized"), "Steps[%d]", log++);
    }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // On va maintenant supprimer un fichier .o et voir si le fichier seul et le produit sont reconstruit
  size_t		obj;
  size_t		prod;

  // On récupère les index d'un objet et du produit
  for (obj = 0; split[obj] && fnmatch("*.o", split[obj], 0) != 0; ++obj);
  snprintf(&buffer2[0], sizeof(buffer2), "*/%s", product_name);
  for (prod = 0; split[prod] && fnmatch(&buffer2[0], split[prod], 0) != 0; ++prod);

  unlink(strstr(split[obj], "./"));
  // On tente maintenant une construction qui devrait normalement reconstruire le fichier supprimé et le produit
  if ((res = try_make(act, exe)) != TC_SUCCESS)
    return (res);
  // C'est reparti, on enregistre dans buffer 2.
  y = sizeof(buffer2);
  if (tcpopen("make module", &command[0], &buffer2[0], &y, NULL, 0) != 0)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Fail to list object and product file for make module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if ((split2 = bunny_split(&buffer2[0], &tokens[0], true)) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Fail to split object and product list for make module.\n");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (bunny_split_len(split) != bunny_split_len(split2))
    return (report(act, exe, TC_FAILURE, "RebuildSpecificObjectFailed", "makefile_invalid_dependencies")); // LCOV_EXCL_LINE
  for (i = 0; split[i] && split2[i]; ++i)
    if (strcmp(split[i], split2[i]) != 0 && i != obj && i != prod)
      return (report(act, exe, TC_FAILURE, "RebuildSpecificObjectFailed", "makefile_invalid_dependencies"));
  // Si on arrive la, c'est que tout matchait. GG.
  bunny_delete_split(split);
  split = split2;
  split2 = NULL;
  add_to_current_report(act, dict_get_pattern("MakefileDependenciesOk"), "Steps[%d]", log++);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Maintenant on essai "clean", on regarde si il reste des fichiers objets, et si le produit est encore la
  // On reconstruit tout après
  if (system("make clean") != 0)
    return (report(act, exe, TC_FAILURE, "MakeCleanFailed", "makefile_invalid_clean"));
  // On nettoie deux fois, pour verifier le -f sur rm.
  if (system("make clean") != 0)
    return (report(act, exe, TC_FAILURE, "MakeCleanFailed", "makefile_invalid_clean")); // LCOV_EXCL_LINE
  // On verifie que le produit est encore la, et que plus aucun .o n'est la.
  for (i = 0; split[i]; ++i)
    {
      const char *s = strstr(split[i], "./");

      if (file_exists(s))
	// Si le fichier existe, ca DOIT etre le produit, sinon c'est une erreur.
	if (strcmp(&s[2], product_name) != 0)
	  return (report(act, exe, TC_FAILURE, "MakeCleanBroken", "makefile_invalid_clean"));
    }

  add_to_current_report(act, dict_get_pattern("MakefileCleanRuleOk"), "Steps[%d]", log++);
  if ((res = try_make(act, exe)) != TC_SUCCESS)
    return (res);

  /////////////////////////////////////////////////////////////////////
  // Mainteant on essai "fclean", on regarde qu'il ne reste plus rien.
  // On reconstruit tout après.
  if (system("make fclean") != 0)
    return (report(act, exe, TC_FAILURE, "MakeFullCleanFailed", "makefile_invalid_fclean"));
  // On nettoie deux fois, pour verifier le -f sur rm.
  if (system("make fclean") != 0)
    return (report(act, exe, TC_FAILURE, "MakeFullCleanFailed", "makefile_invalid_fclean")); // LCOV_EXCL_LINE
  // On verifie que plus rien n'est la
  for (i = 0; split[i]; ++i)
    if (file_exists(&strrchr(split[i], ' ')[1]))
      return (report(act, exe, TC_FAILURE, "MakeFullCleanBroken", "makefile_invalid_fclean"));
  add_to_current_report(act, dict_get_pattern("MakefileFullCleanRuleOk"), "Steps[%d]", log++);
  if ((res = try_make(act, exe)) != TC_SUCCESS)
    return (res);
  bunny_delete_split(split);

  ///////////////////////////////////////////////////////////////////////////
  // Maintenant on utilise re, normalement tous les fichiers seront construit.
  // Ensuite, ils seront tous reconstruit et on verra si une difference de creation de date apparait
  if (!bunny_configuration_getf(exe, &check, "CheckRe") || check)
    {
      if (system("make re") != 0)
	return (report(act, exe, TC_FAILURE, "MakeReFailed", "makefile_invalid_re"));
      if (tcpopen("make module", &command[0], &buffer[0], &x, NULL, 0) != 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Fail to list object and product file for make module (make re).\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if ((split = bunny_split(&buffer[0], &tokens[0], true)) == NULL)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Fail to split object and product list for make module (make re).\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if (system("make re") != 0)
	return (report(act, exe, TC_FAILURE, "MakeReFailed", "makefile_invalid_re")); // LCOV_EXCL_LINE
      if (tcpopen("make module", &command[0], &buffer[0], &x, NULL, 0) != 0)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Fail to list object and product file for make module (make re2).\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if ((split2 = bunny_split(&buffer[0], &tokens[0], true)) == NULL)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Fail to split object and product list for make module (make re2).\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if (bunny_split_len(split) != bunny_split_len(split2))
	return (report(act, exe, TC_FAILURE, "MakeReGenerationFailed", "makefile_invalid_re")); // LCOV_EXCL_LINE
      // Toutes les lignes devraient être differentes
      for (i = 0; split[i] && split2[i]; ++i)
	if (strcmp(split[i], split2[i]) == 0)
	  return (report(act, exe, TC_FAILURE, "MakeReGenerationFailed", "makefile_invalid_re"));
      add_to_current_report(act, dict_get_pattern("MakefileReRuleOk"), "Steps[%d]", log++);
    }

  const char *med = "simple_makefile";
  bool test = false;
  
  //////////////////////////////////////////////////////////////////////
  // Maintenant, on va verifier check, dans test/ devraient apparaitre des .o et un dossier report.
  if (bunny_configuration_getf(exe, &check, "CheckTests") && check)
    {
      if (system("rm -rf ./tests/report")) {}
      if (file_exists("./tests/") == false)
	return (report(act, exe, TC_FAILURE, "NoTestDirectoryFound", "test_are_missing"));
      if (system("make check") != 0)
	return (report(act, exe, TC_FAILURE, "MakeTestsFailed", "makefile_invalid_check"));
      if (file_exists("./tests/report/") == false)
	return (report(act, exe, TC_FAILURE, "TestReportNotGenerated", "makefile_invalid_check"));
      add_to_current_report(act, dict_get_pattern("MakefileTestRuleOk"), "Steps[%d]", log++);
      med = "makefile";
      test = true;
    }

  ////////////////////////////////////////////
  // Maintenant on va verifier l'installation.
  if (bunny_configuration_getf(exe, &check, "CheckInstall") && check)
    {
      if (system("rm -rf .bin .lib .inc ; mkdir .bin .lib .inc")) {}
      str = "make install INSTALL_BIN_DIR=./.bin INSTALL_LIB_DIR=./.lib INSTALL_INC_DIR=./.inc";
      if (system(str) != 0)
	return (report(act, exe, TC_FAILURE, "MakeInstallFailed", "makefile_invalid_install"));
      bool		ok = true;

      // On cherche a verifier l'installation d'une bibliothèque
      if (fnmatch("lib*.so", product_name, 0) == 0 || fnmatch("lib*.a", product_name, 0) == 0)
	{
	  snprintf(&buffer[0], sizeof(buffer), "./.lib/%s", product_name);
	  if (!file_exists(&buffer[0]))
	    ok = false; // LCOV_EXCL_LINE
	  x = sizeof(buffer);
	  if (tcpopen("make module", "find ./.inc -name '*.h*'", &buffer[0], &x, NULL, 0) != 0)
	    { // LCOV_EXCL_START
	      add_message(&gl_technocore.error_buffer, "Fail to list header file for make module.\n");
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  // Est ce qu'il y a au moins quelques fichiers de remonté?
	  if (strstr(&buffer[0], ".h") == NULL) // Non
	    ok = false;
	}
      // Ou d'un programme
      else
	{
	  snprintf(&buffer[0], sizeof(buffer), "./.bin/%s", product_name);
	  if (!file_exists(&buffer[0]))
	    ok = false;
	}
      if (ok == false)
	return (report(act, exe, TC_FAILURE, "MakeInstallBroken", "makefile_invalid_install"));
      add_to_current_report(act, dict_get_pattern("MakefileInstallRuleOk"), "Steps[%d]", log++);
      if (!test)
	med = "makefile";
      else
	med = "full_makefile";
    }

  return (report(act, exe, TC_SUCCESS, "MakefileOk", med));
}
