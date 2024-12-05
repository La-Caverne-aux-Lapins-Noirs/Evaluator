/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

void			evaluate_performances(t_func_eval_mod	*fem,
					      t_perf_test	perfcall,
					      void		*tcfunc,
					      void		*func,
					      void		*data,
					      size_t		datasiz,
					      size_t		nmemb)
{
  char			buffer[4096];
  void			*bench[nmemb];
  size_t		i;

  for (i = 0; i < nmemb; ++i)
    bench[i] = (char*)data + datasiz * i;

  size_t		user_ram;
  size_t		tc_ram;
  size_t		ram_now;

  t_bunny_time		user_perf;
  t_bunny_time		tc_perf;
  t_bunny_time		time_now;

  char temp_bench[datasiz];

  time_now = bunny_get_time();
  ram_now = get_allocated_byte_count();
  for (i = 0; i < nmemb * 100; ++i)
    {
      memcpy(temp_bench, bench[i % nmemb], datasiz);
      perfcall(tcfunc, temp_bench);
    }
  tc_perf = bunny_get_time() - time_now;
  tc_ram = get_allocated_byte_count() - ram_now;
  // On épuise le pipe
  while (read(gl_technocore.stdout_pipe[0], buffer, sizeof(buffer)) > 0);
  while (read(gl_technocore.stderr_pipe[0], buffer, sizeof(buffer)) > 0);

  ram_now = get_allocated_byte_count();
  time_now = bunny_get_time();
  for (i = 0; i < nmemb * 100; ++i)
    {
      memcpy(temp_bench, bench[i % nmemb], datasiz);
      perfcall(func, temp_bench);
    }
  user_perf = bunny_get_time() - time_now;
  user_ram = get_allocated_byte_count() - ram_now;
  // On épuise le pipe
  while (read(gl_technocore.stdout_pipe[0], buffer, sizeof(buffer)) > 0);
  while (read(gl_technocore.stderr_pipe[0], buffer, sizeof(buffer)) > 0);

  if (fem->criterias.perf_ratio > 0)
    fem->result.perf_ratio = (double)user_perf / tc_perf;
  else
    fem->result.perf_ratio = -1;
  if (fem->criterias.ram_ratio > 0)
    {
      if (tc_ram != 0)
	fem->result.ram_ratio = (double)user_ram / tc_ram;
      else if (user_ram != 0)
	fem->result.ram_ratio = -user_ram;
      else
	fem->result.ram_ratio = 1;
    }
  else
    fem->result.ram_ratio = -1;
}
