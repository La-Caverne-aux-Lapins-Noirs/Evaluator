/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

bool			add_artefact(t_technocore_activity	*act,
				     const char			*str)
{
  if (act->nbr_artefacts >= NBRCELL(act->artefacts))
    return (false);
  if ((act->artefacts[act->nbr_artefacts] = bunny_strdup(str)) == NULL)
    return (false);
  act->nbr_artefacts += 1;
  return (true);
}

