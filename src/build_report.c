/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<errno.h>
#include		"technocore.h"

bool			build_report(t_technocore_activity		*tech)
{
  char			buffer[8128];
  size_t		i;
  int			ret;

  if (bunny_save_configuration(BC_DABSIC, "./report.dab", tech->report) == false)
    return (false);
  i = snprintf(&buffer[0], sizeof(buffer), "tar cfz report.tar.gz report.dab");
  for (size_t j = 0; j < tech->nbr_artefacts; ++j)
    i += snprintf(&buffer[i], sizeof(buffer) - i, " %s", tech->artefacts[j]);
  ret = system(&buffer[0]);
  return (ret == 0);
}

