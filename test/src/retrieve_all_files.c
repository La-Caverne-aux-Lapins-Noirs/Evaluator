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
  char			path[512];
  char			*buffer[16];
  size_t		nbr;
  size_t		fnd;

  for (size_t i = 0; i < NBRCELL(buffer); ++i)
    buffer[i] = alloca(64);
  if (chdir("./raf/") != 0)
    if (chdir("./res/raf/") != 0)
      if (chdir("./src/res/raf") != 0)
	assert(chdir("./test/src/raf") == 0);

  nbr = 0;
  strcpy(path, ".");
  assert(retrieve_all_files(path, NBRCELL(path), buffer, &nbr, 16, 64, ".x"));
  assert(nbr == 7);
  fnd = 0;
  for (size_t i = 0; i < nbr; ++i)
    if (strcmp(buffer[i], "./file0.x") == 0)
      fnd += 1;
    else if (strcmp(buffer[i], "./file1.x") == 0)
      fnd += 1;
    else if (strcmp(buffer[i], "./file2.x") == 0)
      fnd += 1;
    else if (strcmp(buffer[i], "./sub1/file3.x") == 0)
      fnd += 1;
    else if (strcmp(buffer[i], "./sub1/file4.x") == 0)
      fnd += 1;
    else if (strcmp(buffer[i], "./sub2/file5.x") == 0)
      fnd += 1;
    else if (strcmp(buffer[i], "./sub2/file6.x") == 0)
      fnd += 1;
  assert(fnd == nbr);
  return (0);
}
