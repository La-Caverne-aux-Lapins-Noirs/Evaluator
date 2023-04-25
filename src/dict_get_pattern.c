/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

const char		*dict_get_pattern(const char			*str)
{
  static const char	*val;
  // static const char	*no_string = "";

  if (gl_dictionnary == NULL && dict_open() == false)
    return (NULL);
  if (bunny_configuration_getf(gl_dictionnary, &val, str))
    return (val);
  // return (no_string);
  return (str);
}

