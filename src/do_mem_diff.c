/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2024
**
** TechnoCore
*/

#include		"technocore.h"

t_technocore_result	do_mem_diff(t_technocore_activity	*act,
				    const char			*activity_name,
				    const char			*user,
				    const char			*expected,
				    int				len)
{
  int			i;

  for (i = 0; i < len; ++i)
    if (user[i] != expected[i])
      break ;
  if (i == len)
    return (TC_SUCCESS);
  if (add_exercise_message(act, dict_get_pattern("StringsAreDifferent"), activity_name, user, expected, i, '^') == false)
    {
      add_message(&gl_technocore.error_buffer, "Cannot add string diff message for exercise %s.\n", activity_name);
      return (TC_CRITICAL);
    }
  return (TC_FAILURE);
}

