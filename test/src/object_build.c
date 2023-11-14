/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"
// LCOV_EXCL_START
bool			almost_equal(const char		*a,
				     const char		*b,
				     int		max)
{
  int			mid = ('z' - 'A') / 2;
  int			ca;
  int			cb;
  int			i;
  int			j;

  ca = 0;
  for (i = 0; a[i] && i < max; ++i)
    ca += a[i];
  cb = 0;
  for (j = 0; b[j] && j < i && j < max; ++j)
    cb += b[j];
  while (j < i && j < max)
    {
      cb += mid;
      j += 1;
    }
  i = (i + j) / 2;
  mid *= i;
  double		k;
  double		l;

  k = (double)ca / mid;
  l = (double)cb / mid;
  return (fabs(k - l) < 0.2);
}
// LCOV_EXCL_STOP
int			main(void)
{
  t_bunny_configuration	*local;
  t_technocore_activity	act;
  const char		*str;

  assert(dict_open());
  assert(dict_set_language("EN"));

  memset(&act, 0, sizeof(act));
  // Aucune valeur configuré, toutes celles par defaut seront utilisées
  assert(local = bunny_new_configuration());
  assert(act.current_report = bunny_new_configuration());

  //// IL Y A UN WARNING, MAIS ON L'ACCEPTE

  // Le fichier a compiler va generer un warning qu'on va accepter.
  assert(bunny_configuration_setf(local, 1, "Tolerance"));
  assert(bunny_configuration_setf(local, "echo ./warning.c", "SearchCommand"));
  // Contient un fichier .c qui sera compilé en .o
  if (chdir("./test/src/res/object_build") != 0)
    if (chdir("./src/res/object_build") != 0)
      if (chdir("res/object_build") != 0)
	assert(chdir("object_build") == 0);
  assert(evaluate_object_build("a", local, local, &act) == TC_SUCCESS);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The compilation was achieved despite the inconsistancies. (1 warning(s) with a 1 margin).\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].File"));
  assert(strcmp(str, "./warning.c") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].Status"));
  assert(strcmp(str, "Warning") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].Message"));
  assert(strstr(str, "makes pointer from integer"));

  /// IL Y A UN WARNING ET ON NE L'ACCEPTE PAS

  bunny_delete_node(local, "Conclusion");
  bunny_delete_node(local, "FailedObject");
  bunny_delete_node(local, "Tolerance");
  assert(evaluate_object_build("a", local, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The compilation encountered too many inconsistancies (1 warning(s) with a 0 margin).\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].File"));
  assert(strcmp(str, "./warning.c") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].Status"));
  assert(strcmp(str, "Warning") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].Message"));
  assert(strstr(str, "makes pointer from integer"));

  /// IL Y A DES ERREURS

  bunny_delete_node(local, "Conclusion");
  bunny_delete_node(local, "FailedObject");
  bunny_delete_node(local, "SearchCommand"); // Comme ca, on verifie aussi la commande par defaut
  assert(bunny_configuration_setf(local, 0, "Tolerance"));
  assert(evaluate_object_build("a", local, local, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The compilation was not possible (1 error(s), 1 warning(s) with a 0 margin).\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].File"));
  assert(strcmp(str, "./warning.c") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].Status"));
  assert(strcmp(str, "Warning") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[0].Message"));
  assert(strstr(str, "makes pointer from integer"));

  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[1].File"));
  assert(strcmp(str, "./error.c") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[1].Status"));
  assert(strcmp(str, "Error") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "FailedObject[1].Message"));
  assert
    (almost_equal
     (str,
      "res/object_build/error.c: In function ‘lel’:\n"
      "res/object_build/error.c:5:3: error: ‘i’ undeclared (first use in this function)\n"
      "    5 |   i +\n"
      "      |   ^\n"
      "res/object_build/error.c:5:3: note: each undeclared identifier is reported only once for each function it appears in\n"
      "res/object_build/error.c:6:1: error: expected expression before ‘}’ token\n"
      "    6 | }\n"
      "      | ^\n",
      50) == 0);

  /// CA COMPILE BIEN

  bunny_delete_node(local, "Conclusion");
  bunny_delete_node(local, "FailedObject");
  assert(bunny_configuration_setf(local, "echo ./ok.c", "SearchCommand"));
  assert(bunny_configuration_setf(local, 0, "Tolerance"));
  assert(evaluate_object_build("a", local, local, &act) == TC_SUCCESS);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The compilation went correctly.\n") == 0);
  assert(!bunny_configuration_getf(act.current_report, NULL, "FailedObject"));

  /// CA COMPILE BIEN ET CA LINKE AUSSI

  bunny_delete_node(local, "Conclusion");
  bunny_delete_node(local, "FailedObject");
  assert(bunny_configuration_setf(local, "echo ./ok.c", "SearchCommand"));
  assert(bunny_configuration_setf(local, 0, "Tolerance"));
  assert(bunny_configuration_setf(local, true, "Link"));
  assert(evaluate_object_build("a", local, local, &act) == TC_SUCCESS);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The compilation went correctly.\n") == 0);
  assert(!bunny_configuration_getf(act.current_report, NULL, "FailedObject"));
  assert(file_exists("./ok.c.out"));

  assert(system("find . -name '*.*o' -delete") == 0);
  return (EXIT_SUCCESS);
}

