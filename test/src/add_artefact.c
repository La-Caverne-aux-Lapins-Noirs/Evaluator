/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  const char		*x = "aaa";
  t_technocore_activity	act;
  int			i;

  memset(&act, 0, sizeof(act));
  for (i = 0; i < NBRCELL(act.artefacts); ++i)
    assert(add_artefact(&act, x));
  assert(act.nbr_artefacts == NBRCELL(act.artefacts));
  for (i = 0; i < NBRCELL(act.artefacts); ++i)
    {
      assert(strcmp(act.artefacts[i], x) == 0);
      assert(act.artefacts[i] != x);
    }
  assert(add_artefact(&act, "bbb") == false);
  for (i = 0; i < NBRCELL(act.artefacts); ++i)
    bunny_free(act.artefacts[i]);
  act.nbr_artefacts = 0;
  for (i = 0; i < NBRCELL(act.artefacts); ++i)
    assert(add_artefact(&act, x));

  act.report = bunny_new_configuration();
  clear_technocore_activity(&act);
  assert(act.nbr_artefacts == 0);
  assert(act.report == NULL);
  assert(act.current_report == NULL);

  // Seulement avec une LibLapin avec l'allocateur bunny_malloc activé
  /*
  if (bunny_malloc != malloc)
    {
      bunny_malloc_failure(true);
      assert(add_artefact(&act, x) == false);
    }
  */
  return (EXIT_SUCCESS);
}


