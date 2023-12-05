/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

void			get_func_eval_mod(t_bunny_configuration		*global,
					  t_bunny_configuration		*local,
					  t_func_eval_mod		*fem,
					  void				*handler)
{
  get_evaluation_axis(global, local, &fem->criterias);
  memset(&fem->result, 0, sizeof(fem->result));
  purge_message(&fem->module_messages);
  get_user_functions(handler, &fem->criterias, &fem->result, local, &fem->user_functions);
}

