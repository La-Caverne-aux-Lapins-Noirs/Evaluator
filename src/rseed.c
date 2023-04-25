/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<lapin.h>
#include		<stdlib.h>

void			rseed(void)
{
  t_bunny_time		now;

  now = bunny_get_time();
  srand(now);
}
