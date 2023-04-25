/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
** Implémenté à partir de la grammaire du C ANSI:
** https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
**
** Cette section a pour vocation à rejoindre ou être dupliqué dans la LibLapin
** pour la récupération des types et fonctions des .h pour Dabsic
*/

#include		<ctype.h>
#include		"technocore.h"

t_technocore_result evaluate_file_c_norm(const char		*file,
					 t_bunny_configuration	*gen,
					 t_bunny_configuration	*exe,
					 t_technocore_activity	*act)
{
  (void)file;
  (void)gen;
  (void)exe;
  (void)act;
  // Cette fonction utilisera CCCCrawler.
  return (TC_SUCCESS);
}
