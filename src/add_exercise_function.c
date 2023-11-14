/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

////////////////////////////////////////////////////
// LE NOM DE LA FONCTION DOIT AVOIR ETE VERIFIE ! //
////////////////////////////////////////////////////

bool			add_exercise_function(t_technocore_activity	*act,
					      t_bunny_configuration	*glob,
					      const char		*func)
{
  // On ajoute aux fonctions autorisées pour les prochains exercices
  if (!bunny_configuration_setf(bunny_configuration_get_root(glob),
				1, "AuthorizedFunctions.%s", func))
    return (false);
  // On ajoute au rapport
  return (bunny_configuration_setf(act->current_report, 1, "Functions.%s", func));
}
