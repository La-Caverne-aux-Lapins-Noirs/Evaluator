/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

/*
** Ce fichier ne teste pas grand chose.
** Il est présent afin de servir d'exemple sur la manière de tester le technocentre.
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  char			buffer[1024];
  int			pip[2];
  ssize_t		len;

  assert(pipe(pip) == 0);
  assert(close(2) == 0);
  assert(dup(pip[1]) == 2);

  const char		*out =
    "aaa: Usage is:\n"
    "\taaa directory+\n"
    ;
  usage("aaa");
  len = read(pip[0], &buffer[0], sizeof(buffer) - 1);
  assert(len == (ssize_t)strlen(out));
  buffer[strlen(out)] = '\0';
  assert(strcmp(out, &buffer[0]) == 0);
  return (EXIT_SUCCESS);
}

