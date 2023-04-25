/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<stdio.h>
#include		<stdlib.h>
#include		<errno.h>
#include		<unistd.h>
#include		<fcntl.h>
#include		"technocore.h"

int			main(int		argc,
			     char		**argv)
{
  // On indique le manuel si on lance seul le technocentre.
  if (argc < 2)
    {
      usage(*argv);
      return (EXIT_FAILURE);
    }

  // Ouverture du dictionnaire
  bunny_set_log_filter("syntax,configuration");
  if (dict_open() == false)
    {
      fprintf(stderr, "%s: Impossible to open dictionnary file.\n", argv[0]);
      return (EXIT_FAILURE);
    }

  /*
  ** Fermeture de stdin: on a pas besoin de la vraie entrée standard mais on a besoin de son numéro
  ** Le fd 0 sera réouvert vers un pipe afin de pouvoir glisser des infos dedans
  */
  if (close(STDIN_FILENO) == -1 || pipe(gl_technocore.stdin_pipe) == -1)
    {
      fprintf(stderr, "%s: Cannot set the stdin pipe. %s.\n", *argv, strerror(errno));
      return (EXIT_FAILURE);
    }

  // On enregistre la position actuelle du programme avant de se deplacer dans les dossiers a evaluer
  char			cwd[1024];

  if (getcwd(&cwd[0], sizeof(cwd)) == NULL)
    {
      fprintf(stderr, "%s: Cannot get current working directory. %s.\n", *argv, strerror(errno));
      return (EXIT_FAILURE);
    }
  t_bunny_configuration	*cnf;

  // On se deplace dans chaque dossier courant et on passe a l'etape suivante pour chacun d'entre eux
  for (int i = 1; i < argc; ++i)
    {
      // On joint le dossier de travail à évaluer
      if (chdir(argv[i]) == -1)
	{
	  fprintf(stderr, "%s: Cannot join requested directory %s. %s.\n", *argv, argv[i], strerror(errno));
	  return (EXIT_FAILURE);
	}

      // On charge le fichier de configuration. Si une erreur se produit, on l'affiche.
      bunny_set_error_descriptor(2);

      if ((cnf = bunny_open_configuration(TECHNOCORE_CONFIGURATION, NULL)) == NULL)
	{
	  fprintf(stderr, "%s: Cannot open configuration file for %s.\n", *argv, argv[i]);
	  return (EXIT_FAILURE);
	}
      if ((cnf = bunny_open_configuration("./activity.dab", cnf)) == NULL)
	{
	  fprintf(stderr, "%s: Cannot open activity file for %s.\n", *argv, argv[i]);
	  return (EXIT_FAILURE);
	}
      if (access("./authorized.dab", F_OK | R_OK) == 0)
	if (bunny_open_configuration("./authorized.dab", cnf) == NULL)
	  {
	    fprintf(stderr, "%s: Cannot load authorized functions.\n", *argv);
	    return (EXIT_FAILURE);
	  }
      bunny_set_error_descriptor(-1);

      // On lance l'evaluation de l'activité
      if (start_activity(*argv, cnf) == TC_CRITICAL)
	return (EXIT_FAILURE);

      // On revient au dossier d'avant. Le if est pour inhiber -Wunused-result
      if (chdir(&cwd[0])) {}
    }
  return (EXIT_SUCCESS);
}

