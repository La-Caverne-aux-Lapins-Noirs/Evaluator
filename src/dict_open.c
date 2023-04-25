/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

t_bunny_configuration	*gl_dictionnary;

void			dict_close(void)
{
  if (gl_dictionnary)
    bunny_delete_configuration(gl_dictionnary);
}

bool			dict_open(void)
{
  bunny_set_error_descriptor(2);
  if ((gl_dictionnary = bunny_open_configuration(TECHNOCORE_DICTIONNARY, NULL)) == NULL)
    return (false);
  bunny_set_error_descriptor(-1);
  atexit(dict_close);
  return (true);
}

