/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<string.h>
#include		<errno.h>
#include		"technocore.h"

t_technocore_result	no_output(t_technocore_activity	*act,
				  int			fd,
				  const char		*out,
				  const char		*file)
{
  static char		trace[512] = {0};
  static int		mistake_count = 0;
  char			buffer[512];
  ssize_t		rd;

  // Ce n'est pas la peine de faire 25 fois les mêmes tests si on a deja
  // établi la présence d'une erreur
  if (trace[0] == '\0' || strcmp(trace, file) != 0)
    {
      snprintf(trace, sizeof(trace), "%s", file);
      mistake_count = 0;
    }
  else if (mistake_count != 0)
    return (TC_SUCCESS);
  
  if ((rd = read(fd, buffer, sizeof(buffer) - 1)) == -1 && errno != EAGAIN)
    { // LCOV_EXCL_START
      snprintf(buffer, sizeof(buffer), "%s", __FILE__);
      *strchrnul(buffer, '.') = '\0';
      add_message(&gl_technocore.error_buffer,
		  dict_get_pattern("ReadFail"),
		  out, buffer);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  else if (rd > 0)
    {
      mistake_count += 1;
      buffer[rd] = '\0';
      if (add_exercise_message
	  (act, dict_get_pattern("NoPrintWereExpectedOn"), "stdout", buffer
	   ) == false)
	{ // LCOV_EXCL_START
	  snprintf(buffer, sizeof(buffer), "%s", file);
	  *strchrnul(buffer, '.') = '\0';
	  add_message(&gl_technocore.error_buffer,
		      dict_get_pattern("TraceFail"),
		      buffer);
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
    }
  return (TC_SUCCESS);
}

