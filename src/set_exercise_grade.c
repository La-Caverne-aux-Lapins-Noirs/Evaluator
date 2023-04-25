/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

bool			set_exercise_grade(t_technocore_activity	*act,
					   double			grade)
{
  return (bunny_configuration_setf(act->current_report, grade, "Grade"));
}

