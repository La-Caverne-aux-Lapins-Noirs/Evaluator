/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include			<assert.h>
#include			"technocore.h"

extern t_bunny_configuration	*gl_dictionnary;

int				main(void)
{
  t_bunny_configuration		*cnf;

  assert(dict_open());
  assert(cnf = gl_dictionnary);
  assert(dict_set_language("FR"));
  assert(cnf != gl_dictionnary);
  assert(strcmp("FR", bunny_configuration_get_name(gl_dictionnary)) == 0);
  assert(strcmp(dict_get_pattern("Exercise"), "Exercice%02zu") == 0);
  assert(strcmp(dict_get_pattern("_____"), "_____") == 0);
  return (EXIT_SUCCESS);
}

