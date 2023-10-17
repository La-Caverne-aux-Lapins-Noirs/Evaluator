/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
**
** TechnoCentre
*/

#include		<ctype.h>
#include		"technocore.h"

t_technocore_result evaluate_file_c_norm(const char		*file,
					 t_bunny_configuration	*gen,
					 t_bunny_configuration	*exe,
					 t_technocore_activity	*act)
{
  (void)file;
  (void)gen;
  (void)exe;
  (void)act;
  /*
  t_parsing		p;
  const char		*code;
  ssize_t		i;
  int			errpts;

  memset(&p, 0, sizeof(p));
  load_norm_configuration(&p, exe);

  // Parcours de l'ensemble des fichiers .c
  while (1)
    {
      // Pour chaque fichier .c
      i = 0;
      if ((code = load_c_file(cfile, exe, true)) == NULL)
	return (TC_CRITICAL);
      read_translation_unit(&p, cfile, code, &i, true);
      errpts += p.nbr_error_points;
      free(code);
    }

  if (errpts > p.maximum_error_points)
    {
      //
      return (TC_FAILURE);
    }

  return (TC_SUCCESS);
  */
}
