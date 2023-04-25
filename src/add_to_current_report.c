/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<unistd.h>
#include		"technocore.h"

bool			add_to_current_report(t_technocore_activity	*act,
					      const char		*value,
					      const char		*pattern,
					      ...)
{
  va_list		lst;
  char			buffer[2048];

  va_start(lst, pattern);
  vsnprintf(&buffer[0], sizeof(buffer), pattern, lst);
  if (bunny_configuration_setf(act->current_report, value, "%s", &buffer[0]) == false)
    {
      add_message(&gl_technocore.error_buffer, "Cannot set %s inside %s.\n", value, &buffer[0]);  // LCOV_EXCL_LINE
      return (false);  // LCOV_EXCL_LINE
    }
  return (true);
}
