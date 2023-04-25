/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

// Fonction caché de la LibLapin
typedef struct		s_memhead
{
  size_t		alloc_count;
  size_t		total_count;
}			t_memhead;

t_memhead		*memory_head(bool			reset);
// LCOV_EXCL_START
size_t			get_allocated_byte_count(void)
{
  t_memhead		*head = memory_head(false);

  return (head->total_count);
}
// LCOV_EXCL_STOP
