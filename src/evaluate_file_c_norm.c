/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCentre
*/

#define			_DEFAULT_SOURCE
#include		<crawler.h>
#include		"technocore.h"

#define			MAXPATH					256

t_technocore_result evaluate_file_c_norm(const char		*argv0,
					 t_bunny_configuration	*gen,
					 t_bunny_configuration	*exe,
					 t_technocore_activity	*act)
{
  char			basepath[512];
  char			*files[1024 * 8];
  size_t		cnt;
  t_parsing		p;
  ssize_t		i;

  (void)argv0;
  (void)gen;
  
  for (i = 0; i < NBRCELL(files); ++i)
    files[i] = alloca(MAXPATH);
  
  cnt = 0;
  memset(&p, 0, sizeof(p));
  if (!retrieve_all_files(basepath, NBRCELL(basepath), files, &cnt, NBRCELL(files), MAXPATH, ".c"))
    {
      add_message(&gl_technocore.error_buffer, "Cannot retrieve all %s files from repository.\n", ".c");
      return (TC_CRITICAL);
    }
  load_norm_configuration(&p, exe);

  for (size_t j = 0; j < cnt; ++cnt)
    {
      char		*code;

      // Pour chaque fichier .c
      if ((code = load_c_file(files[j], exe, true)) == NULL)
	return (TC_CRITICAL);
      i = 0;
      read_translation_unit(&p, files[j], code, &i, false);
      free(code);
    }

  for (int j = 0; j < p.last_error_id;  ++j)
    if (add_exercise_message(act, p.last_error_msg[j]) == false)
      {
	add_message(&gl_technocore.error_buffer, "Cannot add norm messages.\n");
	return (TC_CRITICAL);
      }
  
  if (p.nbr_error_points > p.maximum_error_points)
    {
      if (add_exercise_message(act, dict_get_pattern("TooManyNormError"), p.maximum_error_points, p.nbr_error_points, p.nbr_mistakes) == false)
	{
	  add_message(&gl_technocore.error_buffer, "Cannot add norm messages.\n");
	  return (TC_CRITICAL);
	}
      return (TC_FAILURE);
    }

  return (TC_SUCCESS);
}
