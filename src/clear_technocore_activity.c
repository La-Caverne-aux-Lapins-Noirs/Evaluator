/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

void			clear_technocore_activity(t_technocore_activity	*act)
{
  size_t		i;

  for (i = 0; i < act->nbr_artefacts; ++i)
    bunny_free(act->artefacts[i]);
  act->nbr_artefacts = 0;
  bunny_delete_configuration(act->report);
  act->report = NULL;
  act->current_report = NULL;
}

