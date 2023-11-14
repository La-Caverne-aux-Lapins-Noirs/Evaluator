/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_bunny_configuration	*cnf;
  t_technocore_activity	act;
  const char		*str;

  assert(dict_open());
  assert(dict_set_language("EN"));

  memset(&act, 0, sizeof(act));
  assert(cnf = bunny_new_configuration());
  assert(bunny_configuration_setf(cnf, "program", "ProductName"));
  assert(bunny_configuration_setf(cnf, true, "CheckBehaviour"));
  assert(bunny_configuration_setf(cnf, true, "CheckRe"));
  assert(bunny_configuration_setf(cnf, true, "CheckTests"));
  assert(bunny_configuration_setf(cnf, true, "CheckInstall"));
  assert(act.current_report = bunny_new_configuration());
  if (chdir("./test/src/res/make") != 0)
    if (chdir("./src/res/make") != 0)
      if (chdir("./res/make") != 0)
	assert(chdir("./make/") == 0);

  /////////////////////////////////////////////
  // Etape 1: Le Makefile n'existe pas.
  assert(chdir("make0") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The Makefile is missing.\n") == 0);
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////////
  // Etape 2: La commande make echoue
  assert(chdir("make1") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your default make command generates an error.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 3: Le produit n'est pas généré
  assert(chdir("make2") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The product was not built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 4: Les objets n'ont pas été généré
  assert(chdir("make2_2") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "No object file were produced by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 5: La/le recompilation/relinkage a lieu.
  assert(chdir("make3") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your makefile does not avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 6: Il manque la dependance entre les etapes de compilation
  /*
  assert(chdir("make5") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your makefile des not rebuild modified or deleted compilation unit.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);
  */

  //////////////////////////////////////
  // Etape 7: La règle clean genere une erreur
  assert(chdir("make6") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your clean rule generates an error.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 8: La règle clean ne fonctionne pas
  assert(chdir("make7") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your clean rule does not clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 9: La règle fclean genere une erreur
  assert(chdir("make8") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your fclean rule generates an error.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 10: La règle fclean genere une erreur
  assert(chdir("make9") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your fclean rule does not clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 11: La règle re genere une erreur
  assert(chdir("makeA") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your re rule generates an error.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 12: La règle re ne regenere rien
  assert(chdir("makeB") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your re rule does not regenerate the compilation unit and product.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 13: Il n'y a pas de dossier tests
  assert(chdir("makeC") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "There is not ./tests directory.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[6]"));
  assert(strcmp(str, "Your re rule does rebuild well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 14: La regle check genere une erreur
  assert(chdir("makeD") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your check rule generates an error.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[6]"));
  assert(strcmp(str, "Your re rule does rebuild well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 15: La regle check genere une erreur
  assert(chdir("makeE") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The unit test report was not generated.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[6]"));
  assert(strcmp(str, "Your re rule does rebuild well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 16: La regle d'installation genere une erreur
  assert(chdir("makeF") == 0);
  assert(system("rm -rf .bin .lib .inc") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your install rule generates an error.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[6]"));
  assert(strcmp(str, "Your re rule does rebuild well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[7]"));
  assert(strcmp(str, "Your check rule does run unit tests and generate a report.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  //////////////////////////////////////
  // Etape 17: La regle d'installation n'installe pas.
  assert(chdir("makeG") == 0);
  assert(system("rm -rf .bin .lib .inc") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_FAILURE);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your install rule does not install well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[6]"));
  assert(strcmp(str, "Your re rule does rebuild well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[7]"));
  assert(strcmp(str, "Your check rule does run unit tests and generate a report.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  ///////////////////////////
  // Etape 18: Tout va bien. On installe un programme
  assert(chdir("makeH") == 0);
  assert(system("rm -rf .bin .lib .inc") == 0);
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_SUCCESS);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your construction system is correct.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[6]"));
  assert(strcmp(str, "Your re rule does rebuild well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[7]"));
  assert(strcmp(str, "Your check rule does run unit tests and generate a report.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[8]"));
  assert(strcmp(str, "Your install rule does install well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  ///////////////////////////
  // Etape 19: Tout va bien. On installe une bibliothèque
  assert(chdir("makeI") == 0);
  assert(system("rm -rf .bin .lib .inc") == 0);
  assert(bunny_configuration_setf(cnf, "libprogram.a", "ProductName"));
  assert(evaluate_make_build("a", cnf, cnf, &act) == TC_SUCCESS);

  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "Your construction system is correct.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[0]"));
  assert(strcmp(str, "The Makefile was found.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[1]"));
  assert(strcmp(str, "Your product was built by your makefile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[2]"));
  assert(strcmp(str, "Your makefile does avoid recompilation if unneccessary.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[3]"));
  assert(strcmp(str, "Your makefile detects modified compilation unit and recompile them.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[4]"));
  assert(strcmp(str, "Your clean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[5]"));
  assert(strcmp(str, "Your fclean rule does clean well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[6]"));
  assert(strcmp(str, "Your re rule does rebuild well.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[7]"));
  assert(strcmp(str, "Your check rule does run unit tests and generate a report.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Steps[8]"));
  assert(strcmp(str, "Your install rule does install well.\n") == 0);
  bunny_delete_node(act.current_report, "Steps");
  bunny_delete_node(act.current_report, "Conclusion");
  assert(chdir("..") == 0);

  return (EXIT_SUCCESS);
}
