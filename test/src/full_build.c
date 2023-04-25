/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"
// LCOV_EXCL_START
int			xstrcmp(const char		*a,
				const char		*b)
{
  size_t		i;

  for (i = 0; a[i] && b[i]; ++i)
    if (a[i] != b[i])
      {
	printf("%s\n", a);
	printf("%s\n", b);
	for (size_t j = 0; j < i; ++j)
	  printf(" ");
	printf("^\n");
	return (a[i] - b[i]);
      }
  return (a[i] - b[i]);
}

bool			almost_equal(const char		*a,
				     const char		*b,
				     int		max)
{
  int			mid = ('z' - 'A') / 2;
  int			ca;
  int			cb;
  int			i;
  int			j;

  ca = 0;
  for (i = 0; a[i] && i < max; ++i)
    ca += a[i];
  cb = 0;
  for (j = 0; b[j] && j < i && j < max; ++j)
    cb += b[j];
  while (j < i && j < max)
    {
      cb += mid;
      j += 1;
    }
  i = (i + j) / 2;
  mid *= i;
  double		k;
  double		l;

  k = (double)ca / mid;
  l = (double)cb / mid;
  return (fabs(k - l) < 0.2);
}
// LCOV_EXCL_STOP
int			main(void)
{
  t_bunny_configuration	*cnf;
  t_technocore_activity	act;
  const char		*str;

  assert(dict_open());
  assert(dict_set_language("EN"));

  memset(&act, 0, sizeof(act));
  assert(cnf = bunny_new_configuration());
  assert(act.current_report = bunny_new_configuration());

  // Ca marche.
  assert(bunny_configuration_setf(cnf, "gcc ok.c", "BuildCommand"));
  assert(chdir("./src/res/object_build") == 0);
  assert(evaluate_full_build("a", cnf, cnf, &act) == TC_SUCCESS);
  assert(!bunny_configuration_getf(act.current_report, NULL, "Message"));
  assert(!bunny_configuration_getf(act.current_report, NULL, "Status"));
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The construction went correctly.\n") == 0);

  // Ca ne marche pas.
  assert(bunny_configuration_setf(cnf, "gcc error.c", "BuildCommand"));
  assert(evaluate_full_build("a", cnf, cnf, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &str, "Message"));
  assert
    (almost_equal
     (str,
      "test/src/res/object_build/test.c: In function ‘func’:\n"
      "test/src/res/object_build/test.c:5:15: warning: initialization of ‘void *’",
      100) == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Status"));
  assert(strcmp(str, "Error") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The construction was not possible.\n") == 0);

  return (EXIT_SUCCESS);
}

