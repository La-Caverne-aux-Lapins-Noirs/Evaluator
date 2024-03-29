/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCentre
*/

#define			_DEFAULT_SOURCE
#include		<crawler.h>
#include		"technocore.h"

t_technocore_result evaluate_file_c_norm(const char		*argv0,
					 t_bunny_configuration	*gen,
					 t_bunny_configuration	*exe,
					 t_technocore_activity	*act)
{
  static char		files[1024 * 8][256];
  char			basepath[512] = ".";
  size_t		cnt;
  t_parsing		p;
  ssize_t		i;

  (void)argv0;
  (void)gen;
  
  cnt = 0;
  memset(&p, 0, sizeof(p));
  if (!retrieve_all_files(basepath, NBRCELL(basepath), &cnt, NBRCELL(files), NBRCELL(files[0]), files, ".c"))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Cannot retrieve all %s files from repository.\n", ".c");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  load_norm_configuration(&p, exe);

  for (size_t j = 0; j < cnt; ++j)
    {
      char		*code;

      // Pour chaque fichier .c
      if ((code = load_c_file(files[j], exe, true)) == NULL)
	return (TC_CRITICAL);
      i = 0;
      reset_norm_status(&p);
      read_translation_unit(&p, files[j], code, &i, false, true);
    }

  for (int j = 0; j <= p.last_error_id;  ++j)
    if (add_exercise_message(act, p.last_error_msg[j]) == false)
      { // LCOV_EXCL_START
	add_message(&gl_technocore.error_buffer, "Cannot add norm messages.\n");
	return (TC_CRITICAL);
      } // LCOV_EXCL_STOP
  
  if (p.maximum_error_points != -1 && p.nbr_error_points > p.maximum_error_points)
    {
      if (add_exercise_message(act, dict_get_pattern("TooManyNormError"), p.maximum_error_points, p.nbr_error_points, p.nbr_mistakes) == false)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Cannot add norm conclusion.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      if (!add_exercise_medal(act, "norm_rejected"))
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Cannot add norm medal.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
      return (TC_FAILURE);
    }
  if (p.nbr_error_points == 0)
    {
      if (!add_exercise_medal(act, "norm_perfect"))
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Cannot add norm medal.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
    }
  else if (p.nbr_error_points < p.maximum_error_points)
    {
      if (!add_exercise_medal(act, "norm_correct"))
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Cannot add norm medal.\n");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
    }

  return (TC_SUCCESS);
}
