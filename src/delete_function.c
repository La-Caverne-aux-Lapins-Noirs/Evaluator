/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<dlfcn.h>
#include		"technocore.h"

void			delete_function(t_function	*func)
{
  if (func->handler)
    {
      dlclose(func->handler);
      func->handler = NULL;
    }
  func->function = NULL;
}

