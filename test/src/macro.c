/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<assert.h>
#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<unistd.h>
#include		"technocore.h"

static void		write_file(const char		*file,
				   const char		*content)
{
  FILE			*fil;

  assert(fil = fopen(file, "w"));
  assert(fputs(content, fil) != EOF);
  assert(fclose(fil) == 0);
}

static void		reset_report(t_technocore_activity	*act)
{
  if (act->current_report)
    bunny_delete_configuration(act->current_report);
  assert(act->current_report = bunny_new_configuration());
}

int			main(void)
{
  t_bunny_configuration	*cnf;
  t_technocore_activity	act;
  const char		*str;

  assert(dict_open());
  assert(dict_set_language("EN"));
  memset(&act, 0, sizeof(act));
  assert(cnf = bunny_new_configuration());
  reset_report(&act);
  assert(system("rm -rf macro_tmp && mkdir macro_tmp") == 0);
  assert(chdir("macro_tmp") == 0);

  write_file("student.h",
	     "#ifndef STUDENT_H_\n"
	     "# define STUDENT_H_\n"
	     "# define EFSQUARE(x) ((x) * (x))\n"
	     "# define EFABS(x) ((x) < 0 ? -(x) : (x))\n"
	     "#endif\n");
  assert(bunny_configuration_setf(cnf, "Macro", "Module"));
  assert(bunny_configuration_setf(cnf, "student.h", "Header"));
  assert(bunny_configuration_setf(cnf, "EFSQUARE", "MacroNames[0]"));
  assert(bunny_configuration_setf(cnf, "EFABS", "MacroNames[1]"));
  assert(bunny_configuration_setf(cnf, "square positive", "Tests[0].Name"));
  assert(bunny_configuration_setf(cnf, "EFSQUARE(3)", "Tests[0].Expression"));
  assert(bunny_configuration_setf(cnf, "9", "Tests[0].Expected"));
  assert(bunny_configuration_setf(cnf, "abs negative", "Tests[1].Name"));
  assert(bunny_configuration_setf(cnf, "if (EFABS(-42) != 42) return (1);", "Tests[1].Code"));
  assert(evaluate_macro("a", cnf, cnf, &act) == TC_SUCCESS);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "All macro tests passed.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Compilation.Status"));
  assert(strcmp(str, "Success") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Execution.Status"));
  assert(strcmp(str, "Success") == 0);

  reset_report(&act);
  write_file("student.h",
	     "#ifndef STUDENT_H_\n"
	     "# define STUDENT_H_\n"
	     "# define EFSQUARE(x) ((x) + (x))\n"
	     "# define EFABS(x) ((x) < 0 ? -(x) : (x))\n"
	     "#endif\n");
  assert(evaluate_macro("a", cnf, cnf, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "At least one macro test failed.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Execution.Output"));
  assert(strstr(str, "FAIL 0 square positive"));

  reset_report(&act);
  write_file("student.h",
	     "#ifndef STUDENT_H_\n"
	     "# define STUDENT_H_\n"
	     "# define EFSQUARE(x) (exit(0), 9)\n"
	     "# define EFABS(x) ((x) < 0 ? -(x) : (x))\n"
	     "#endif\n");
  assert(evaluate_macro("a", cnf, cnf, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The macro test program did not reach the expected guarded final state.\n") == 0);

  reset_report(&act);
  write_file("student.h",
	     "#ifndef STUDENT_H_\n"
	     "# define STUDENT_H_\n"
	     "# define EFABS(x) ((x) < 0 ? -(x) : (x))\n"
	     "#endif\n");
  assert(evaluate_macro("a", cnf, cnf, &act) == TC_FAILURE);
  assert(bunny_configuration_getf(act.current_report, &str, "Conclusion"));
  assert(strcmp(str, "The macro test source file does not compile.\n") == 0);
  assert(bunny_configuration_getf(act.current_report, &str, "Compilation.Output"));
  assert(strstr(str, "Missing macro EFSQUARE"));

  assert(chdir("..") == 0);
  assert(system("rm -rf macro_tmp") == 0);
  return (EXIT_SUCCESS);
}
