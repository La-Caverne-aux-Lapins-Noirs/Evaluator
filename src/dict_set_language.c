/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

bool			dict_set_language(const char		*s)
{
  return (bunny_configuration_getf(gl_dictionnary, &gl_dictionnary, "%s", s));
}

