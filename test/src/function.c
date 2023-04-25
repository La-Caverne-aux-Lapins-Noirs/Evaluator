/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<dlfcn.h>
#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

void			test_function_name(void)
{
  if (write(1, "", 0)) {}
}

void			test_test_name(void)
{
  if (write(1, "", 0)) {}
}

int			main(void)
{
  // Appel des fonctions pour empecher le retrait des symboles à la compilation

  test_function_name();
  test_test_name();
  // Configuration de test
  t_technocore_activity	act;
  t_bunny_configuration	*gen;
  t_bunny_configuration	*loc;
  const char		*code =
    "[Local\n"
    "  UserLibrary = \"local\"\n"
    "  FunctionName = \"test_function_name\"\n"
    "  TestName = \"test_test_name\"\n"
    "]\n"
    ;

  memset(&act, 0, sizeof(act));
  assert(gen = bunny_read_configuration(BC_DABSIC, code, NULL));
  act.current_report = gen;
  assert(bunny_configuration_getf(gen, &loc, "Local"));

  // On verifie qu'on arrive bien a recuperer les informations depuis la conf
  t_function		*fc;
  t_function		f;
  t_function_evaluation	eval_cnf;
  t_function_evaluation	eval;
  t_user_functions	ufunc;

  memset(&eval_cnf, 0, sizeof(eval_cnf));
  memset(&eval, 0, sizeof(eval));
  assert(f.handler = dlopen(NULL, RTLD_NOW));

  // Si tout est bien rendu
  eval_cnf.func = true;
  eval_cnf.test_func = true;
  get_user_functions(f.handler, &eval_cnf, &eval, loc, &ufunc);
  assert(strcmp(ufunc.func_name, "test_function_name") == 0);
  assert(strcmp(ufunc.test_name, "test_test_name") == 0);
  assert(ufunc.func == test_function_name);
  assert(ufunc.test == test_test_name);

  // Si rien n'est bien rendu
  assert(bunny_configuration_setf(loc, "bad_function_name", "FunctionName"));
  assert(bunny_configuration_setf(loc, "bad_function_name", "TestName"));
  get_user_functions(f.handler, &eval_cnf, &eval, loc, &ufunc);
  assert(strcmp(ufunc.func_name, "bad_function_name") == 0);
  assert(strcmp(ufunc.test_name, "bad_function_name") == 0);
  assert(ufunc.func == NULL);
  assert(ufunc.test == NULL);
  assert(eval.missing_main_function == true);
  assert(eval.missing_test_function == true);

  // Si les fonctions n'étaient pas réclamées...
  eval.missing_main_function = false;
  eval.missing_test_function = false;
  eval_cnf.func = false;
  eval_cnf.test_func = false;
  get_user_functions(f.handler, &eval_cnf, &eval, loc, &ufunc);
  assert(ufunc.func == NULL);
  assert(ufunc.test == NULL);
  assert(ufunc.func_name == NULL);
  assert(ufunc.test_name == NULL);
  assert(eval.missing_main_function == false);
  assert(eval.missing_test_function == false);

  // Test de la suppression
  f.function = main;
  delete_function(&f);
  assert(f.function == NULL);
  assert(f.handler == NULL);

  // Test du récupérateur de fonction et de lib
  // On verifie qu'on arrive bien a récupérer un élément local.
  assert(fc = get_function("argv0", &act, gen, loc));
  assert(fc->function == NULL);
  assert(fc->handler);
  delete_function(fc);
  assert(fc->handler == NULL);
  assert(fc->function == NULL);

  // On verifie qu'on arrive bien a récupérer un élément global.
  assert(bunny_configuration_setf(gen, "local", "UserLibrary"));
  assert(bunny_configuration_setf(gen, NULL, "Local.UserLibrary"));
  assert(fc = get_function("argv0", &act, gen, loc));
  assert(fc->function == NULL);
  assert(fc->handler);
  delete_function(fc);
  assert(fc->handler == NULL);
  assert(fc->function == NULL);

  // On verifie que lorsqu'il y a une erreur de type: je ne trouve pas la lib, elle est notifié
  assert(bunny_configuration_casesf(act.current_report, "Message") <= 0);
  assert(bunny_configuration_setf(gen, "Zob", "UserLibrary"));
  assert(get_function("aaa", &act, gen, loc) == NULL);
  assert(bunny_configuration_casesf(act.current_report, "Message") == 1);

  // On verifie l'erreur en cas d'absence de champ UserLibrary
  assert(bunny_configuration_setf(gen, NULL, "UserLibrary"));
  assert(get_function("aaa", &act, gen, loc) == NULL);
  assert(bunny_configuration_casesf(act.current_report, "Message") == 1);

  return (EXIT_SUCCESS);
}

