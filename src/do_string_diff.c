/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

t_technocore_result	do_string_diff(t_technocore_activity	*act,
				       const char		*activity_name,
				       const char		*user,
				       const char		*expected,
				       int			maxlen)
{
  char			*first = NULL;
  char			*second = NULL;
  int			i;

  if (bunny_write_cstring(user, &first, 0) == false)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Not enough memory to turn user string into cstring.\n", activity_name);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (bunny_write_cstring(expected, &second, 0) == false)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Not enough memory to turn reference string into cstring for exercise %s.\n", activity_name);
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  i = 0;
  while ((user[i] && user[i] == expected[i]) && (maxlen < 0 || i < maxlen))
    i += 1;
  if ((maxlen >= 0 && i == maxlen) || (user[i] == expected[i]))
    return (TC_SUCCESS);
  
  if (add_exercise_message(act, dict_get_pattern("StringsAreDifferent"), activity_name, user, expected, i, '^') == false)
    {
      add_message(&gl_technocore.error_buffer, "Cannot add string diff message for exercise %s.\n", activity_name);
      return (TC_CRITICAL);
    }
  return (TC_FAILURE);
}

