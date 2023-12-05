/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<string.h>
#include		"technocore.h"

t_technocore_result	module_fail(const char		*label,
				    const char		*file)
{
  char			buffer[512];

  snprintf(buffer, sizeof(buffer), "%s", file);
  *strchrnul(buffer, '.') = '\0';
  add_message(&gl_technocore.error_buffer,
	      dict_get_pattern(label),
	      buffer);
  return (TC_CRITICAL);
}

