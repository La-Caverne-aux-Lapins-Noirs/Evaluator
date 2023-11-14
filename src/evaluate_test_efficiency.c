/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

void			evaluate_test_efficiency(t_trigger		*trigger,
						 t_func_eval_mod	*fem,
						 t_test_func		test,
						 int			maxbkcase)
{
  int			bkcase;
  bool			ok;

  fem->result.test_func = true;

  //////////////////////////////////////////////////////
  /// On construit une fonction en panne. On verifie que l'eleve le detecte
  /// Ensuite on construit une fonction qui **** gère  les cas particuliers ****
  /// Si l'élève n'a écrit que des tests nominaux, ca marchera
  /// Si il a fait des tests complets, ca marchera aussi
  if (fem->criterias.mostly_testing)
    {
      ok = true;
      for (bkcase = 0; bkcase < maxbkcase; ++bkcase)
	{
	  // Notre fonction est tout le temps cassée
	  *trigger = TCFUNC_BROKEN;
	  ok = ok && test() != 0;
	}
      
      for (bkcase = 0; bkcase < maxbkcase; ++bkcase)
	{
	  // Notre fonction est *parfaite*
	  *trigger = TCFUNC_NO_CORNER_CASE;
	  ok = ok && test() == 0;
	}
	  
      fem->result.mostly_testing = ok;
    }

  
  //////////////////////////////////////////////////////
  /// On propose une fonction qui ne gere pas les extremas mais gere les elements
  /// principaux
  // Si l'élève le detecte, cela signifie qu'il teste les extremas.
  if (fem->criterias.fully_testing)
    {
      ok = true;
      for (bkcase = 0; bkcase < maxbkcase; ++bkcase)
	{
	  // Notre fonction est imparfaite, elle comporte
	  // une faiblesse sur les extremas
	  *trigger = TCFUNC_WORKING;
	  ok = ok && test() != 0;
	}
      fem->result.fully_testing = ok;
    }

  //////////////////////////////////////////////////////////
  // L'élève est censé tester des éléments de programmation défensive
  // La fonction doit donc echouer en recevant des trucs petés
  if (fem->criterias.defensive_testing)
    {
      ok = true;
      for (bkcase = 0; bkcase < maxbkcase; ++bkcase)
	{
	  *trigger = TCFUNC_DEFENSIVE;
	  ok = ok && test() != 0;
	}
      fem->result.defensive_testing = ok;
    }

  //////////////////////////////////////////////////////
  // La fonction du TC doit echouer a établir errno
  if (fem->criterias.errno_testing)
    {
      ok = true;
      
      for (bkcase = 0; bkcase < maxbkcase; ++ bkcase)
	{
	  *trigger = TCFUNC_BAD_ERRNO;
	  ok = ok && test() != 0;
	}
      fem->result.errno_testing = ok;
    }

  //////////////////////////////////////////////////////
  // La fonction du TC doit echouer a renvoyer une erreur
  if (fem->criterias.error_testing)
    {
      ok = true;
      for (bkcase = 0; bkcase < maxbkcase; ++ bkcase)
	{
	  *trigger = TCFUNC_NO_ERROR_HANDLING;
	  ok = ok && test() != 0;
	}
      fem->result.error_testing = ok;
    }
}
