/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include			<assert.h>
#include			"technocore.h"

extern t_bunny_configuration	*gl_dictionnary;
size_t				get_max_heap_size(void);

int				main(void)
{
  t_bunny_configuration		*cnf;
  const char			*code =
    "Language = \"FR\"\n"
    "MaximumRam = 1024 * 1024\n"
    "{Exercises\n"
    "  [\n"
    "    Name = \"DEMO\"\n"
    "    Type = \"Builtin\"\n"
    "    Module = \"Failure\"\n"
    "    StopOnFailure = true\n"
    "  ]\n"
    "}\n"
    ;

  assert(dict_open());
  assert(cnf = bunny_read_configuration(BC_DABSIC, code, NULL));
  assert(start_activity("aaa", cnf) == TC_FAILURE);
  assert(strcmp(bunny_configuration_get_name(gl_dictionnary), "FR") == 0);
  assert(get_max_heap_size() == 1024 * 1024);
  return (EXIT_SUCCESS);
}
