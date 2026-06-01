/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define			_GNU_SOURCE
#include		<sys/wait.h>
#include		<errno.h>
#include		<stdio.h>
#include		<stdint.h>
#include		<string.h>
#include		<time.h>
#include		<unistd.h>
#include		"technocore.h"

typedef struct		s_macro_guard
{
  uint64_t		seed;
  uint64_t		expected;
  unsigned int		success_base;
  char			cookie[128];
  char			suffix[64];
  char			state[96];
  char			index[96];
  char			step[96];
  char			forbidden[96];
} 			t_macro_guard;

static int		command_return_value(int		status)
{
  if (status == -1)
    return (-1);
  if (WIFEXITED(status))
    return (WEXITSTATUS(status));
  if (WIFSIGNALED(status))
    return (128 + WTERMSIG(status));
  return (status);
}

static uint64_t		macro_guard_next(uint64_t		state,
					 uint64_t		index,
					 int			ok)
{
  index += 1;
  state ^= index * 0x9E3779B97F4A7C15ULL;
  state ^= ok ? 0xA5A5A5A5A5A5A5A5ULL : 0x5A5A5A5A5A5A5A5AULL;
  state *= 0x100000001B3ULL;
  state ^= state >> 29;
  state *= 0xBF58476D1CE4E5B9ULL;
  state ^= state >> 32;
  return (state);
}

static uint64_t		macro_guard_expected(uint64_t		seed,
				     int			nbr)
{
  uint64_t		state;

  state = seed;
  for (int i = 0; i < nbr + 1; ++i)
    state = macro_guard_next(state, i, 1);
  return (state);
}

static void		prepare_macro_guard(t_macro_guard		*guard)
{
  uint64_t		mix;

  mix = (uint64_t)time(NULL);
  mix ^= (uint64_t)getpid() * 0x9E3779B97F4A7C15ULL;
  mix ^= (uint64_t)(uintptr_t)guard * 0xBF58476D1CE4E5B9ULL;
  guard->seed = macro_guard_next(mix, 0, 1);
  guard->success_base = 0x5A00u + (unsigned int)(guard->seed & 0x3FFFu);
  snprintf(&guard->suffix[0], sizeof(guard->suffix),
	   "%016llx",
	   (unsigned long long)guard->seed);
  snprintf(&guard->cookie[0], sizeof(guard->cookie),
	   "TECHNOCORE_MACRO_FINAL_%s", &guard->suffix[0]);
  snprintf(&guard->state[0], sizeof(guard->state),
	   "tc_macro_state_%s", &guard->suffix[0]);
  snprintf(&guard->index[0], sizeof(guard->index),
	   "tc_macro_index_%s", &guard->suffix[0]);
  snprintf(&guard->step[0], sizeof(guard->step),
	   "tc_macro_step_%s", &guard->suffix[0]);
  snprintf(&guard->forbidden[0], sizeof(guard->forbidden),
	   "tc_macro_forbidden_%s", &guard->suffix[0]);
}

static bool		write_c_string(FILE			*fil,
			       const char		*str)
{
  fputc('"', fil);
  for (size_t i = 0; str[i]; ++i)
    {
      if (str[i] == '\\' || str[i] == '"')
	{
	  if (fputc('\\', fil) == EOF || fputc(str[i], fil) == EOF)
	    return (false);
	}
      else if (str[i] == '\n')
	{
	  if (fputs("\\n", fil) == EOF)
	    return (false);
	}
      else if (str[i] == '\r')
	{
	  if (fputs("\\r", fil) == EOF)
	    return (false);
	}
      else if (fputc(str[i], fil) == EOF)
	return (false);
    }
  return (fputc('"', fil) != EOF);
}

static bool		write_one_header(FILE			*fil,
				 const char		*header)
{
  if (fprintf(fil, "#include ") < 0)
    return (false);
  if (header[0] == '<')
    {
      if (fprintf(fil, "%s\n", header) < 0)
	return (false);
    }
  else if (fprintf(fil, "\"%s\"\n", header) < 0)
    return (false);
  return (true);
}

static bool		write_header_field(FILE			*fil,
				   t_bunny_configuration	*exe,
				   const char			*field,
				   bool				*found)
{
  const char		*header;

  if (bunny_configuration_getf(exe, &header, "%s", field))
    {
      if (write_one_header(fil, header) == false)
	return (false);
      *found = true;
    }
  for (int idx = 0; bunny_configuration_getf(exe, &header, "%s[%d]", field, idx); ++idx)
    {
      if (write_one_header(fil, header) == false)
	return (false);
      *found = true;
    }
  return (true);
}

static bool		write_headers(FILE			*fil,
			      t_bunny_configuration	*exe)
{
  bool			found;

  found = false;
  if (write_header_field(fil, exe, "Header", &found) == false ||
      write_header_field(fil, exe, "Headers", &found) == false)
    return (false);
  return (found);
}

static bool		write_one_macro_check(FILE			*fil,
				      const char		*macro)
{
  return (fprintf(fil,
		  "#ifndef %s\n"
		  "# error \"Missing macro %s\"\n"
		  "#endif\n",
		  macro,
		  macro) >= 0);
}

static bool		write_macro_check_field(FILE			*fil,
					t_bunny_configuration	*exe,
					const char		*field)
{
  const char		*macro;

  if (bunny_configuration_getf(exe, &macro, "%s", field))
    if (write_one_macro_check(fil, macro) == false)
      return (false);
  for (int i = 0; bunny_configuration_getf(exe, &macro, "%s[%d]", field, i); ++i)
    if (write_one_macro_check(fil, macro) == false)
      return (false);
  return (true);
}

static bool		write_macro_checks(FILE			*fil,
				   t_bunny_configuration	*exe)
{
  return (write_macro_check_field(fil, exe, "MacroName") &&
	  write_macro_check_field(fil, exe, "MacroNames"));
}

static bool		write_guard_support(FILE			*fil,
				    const t_macro_guard	*guard)
{
  return (fprintf(fil,
		"\n"
		"static unsigned long long %s = 0x%016llxULL;\n"
		"static unsigned long long %s = 0ULL;\n"
		"\n"
		"static unsigned long long %s(int ok)\n"
		"{\n"
		"  %s += 1ULL;\n"
		"  %s ^= %s * 0x9E3779B97F4A7C15ULL;\n"
		"  %s ^= ok ? 0xA5A5A5A5A5A5A5A5ULL : 0x5A5A5A5A5A5A5A5AULL;\n"
		"  %s *= 0x100000001B3ULL;\n"
		"  %s ^= %s >> 29;\n"
		"  %s *= 0xBF58476D1CE4E5B9ULL;\n"
		"  %s ^= %s >> 32;\n"
		"  return (%s);\n"
		"}\n"
		"\n"
		"static int %s(int code)\n"
		"{\n"
		"  (void)code;\n"
		"  %s(0);\n"
		"  return (0);\n"
		"}\n"
		"\n"
		"#undef exit\n"
		"#define exit(code) %s((int)(code))\n"
		"#undef _Exit\n"
		"#define _Exit(code) %s((int)(code))\n"
		"#undef abort\n"
		"#define abort() %s(134)\n"
		"\n",
		guard->state,
		(unsigned long long)guard->seed,
		guard->index,
		guard->step,
		guard->index,
		guard->state,
		guard->index,
		guard->state,
		guard->state,
		guard->state,
		guard->state,
		guard->state,
		guard->state,
		guard->state,
		guard->state,
		guard->forbidden,
		guard->step,
		guard->forbidden,
		guard->forbidden,
		guard->forbidden) >= 0);
}

static bool		write_test_function(FILE			*fil,
				    t_bunny_configuration	*cnf,
				    int			idx,
				    const t_macro_guard	*guard)
{
  const char		*str;
  const char		*expected;
  unsigned int		success;

  success = guard->success_base + (unsigned int)idx;
  if (fprintf(fil, "static int technocore_macro_test_%d(void)\n{\n", idx) < 0)
    return (false);
  if (bunny_configuration_getf(cnf, &str, "Code"))
    {
      if (fprintf(fil, "%s\n", str) < 0)
	return (false);
    }
  else if (bunny_configuration_getf(cnf, &str, "Expression") &&
	   bunny_configuration_getf(cnf, &expected, "Expected"))
    {
      if (fprintf(fil,
		  "  if (!((%s) == (%s)))\n"
		  "    return (1);\n",
		  str,
		  expected) < 0)
	return (false);
    }
  else
    return (false);
  return (fprintf(fil,
		  "  %s(1);\n"
		  "  return (%u);\n"
		  "}\n\n",
		  guard->step,
		  success) >= 0);
}

static bool		write_main_function(FILE			*fil,
				    t_bunny_configuration	*exe,
				    int			nbr,
				    const t_macro_guard	*guard)
{
  t_bunny_configuration	*cnf;
  const char		*name;

  if (fprintf(fil, "static const char *technocore_macro_test_names[%d] =\n{\n", nbr) < 0)
    return (false);
  for (int i = 0; i < nbr; ++i)
    {
      name = NULL;
      bunny_configuration_getf(exe, &cnf, "Tests[%d]", i);
      bunny_configuration_getf(cnf, &name, "Name");
      if (name == NULL)
	name = bunny_configuration_get_address(cnf);
      if (fprintf(fil, "  ") < 0 || write_c_string(fil, name) == false ||
	  fprintf(fil, ",\n") < 0)
	return (false);
    }
  if (fprintf(fil,
	      "};\n\n"
	      "int main(void)\n"
	      "{\n"
	      "  unsigned long long final;\n"
	      "  int ret;\n\n"
	      "  (void)%s;\n",
	      guard->forbidden) < 0)
    return (false);
  for (int i = 0; i < nbr; ++i)
    if (fprintf(fil,
		"  ret = technocore_macro_test_%d();\n"
		"  if (ret != %u)\n"
		"    {\n"
		"      fprintf(stderr, \"FAIL %%d %%s\\n\", %d, technocore_macro_test_names[%d]);\n"
		"      return (ret == 0 ? 1 : ret);\n"
		"    }\n",
		i,
		guard->success_base + (unsigned int)i,
		i,
		i) < 0)
      return (false);
  return (fprintf(fil,
		  "  final = %s(1);\n"
		  "  printf(\"%s %%016llx\\n\", final);\n"
		  "  if (final != 0x%016llxULL)\n"
		  "    return (1);\n"
		  "  return (0);\n"
		  "}\n",
		  guard->step,
		  guard->cookie,
		  (unsigned long long)guard->expected) >= 0);
}

static bool		generate_macro_source(const char		*path,
				      t_bunny_configuration	*exe,
				      int			*nbr,
				      t_macro_guard		*guard)
{
  t_bunny_configuration	*cnf;
  const char		*str;
  FILE			*fil;

  if ((fil = fopen(path, "w")) == NULL)
    return (false);
  if (fprintf(fil,
	      "#include <stdio.h>\n"
	      "#include <stdlib.h>\n") < 0 ||
      write_headers(fil, exe) == false ||
      write_macro_checks(fil, exe) == false ||
      write_guard_support(fil, guard) == false)
    goto Failure;
  if (bunny_configuration_getf(exe, &str, "Prelude"))
    if (fprintf(fil, "%s\n", str) < 0)
      goto Failure;
  *nbr = 0;
  for (int i = 0; bunny_configuration_getf(exe, &cnf, "Tests[%d]", i); ++i)
    {
      if (write_test_function(fil, cnf, i, guard) == false)
	goto Failure;
      *nbr += 1;
    }
  guard->expected = macro_guard_expected(guard->seed, *nbr);
  if (*nbr == 0 || write_main_function(fil, exe, *nbr, guard) == false)
    goto Failure;
  if (fclose(fil) == EOF)
    return (false);
  return (true);

 Failure:
  fclose(fil);
  return (false);
}

static t_technocore_result	macro_report(t_technocore_activity	*act,
					     t_bunny_configuration	*exe,
					     t_technocore_result	res,
					     const char		*sub,
					     const char		*med)
{
  bool				ret;

  ret = add_to_current_report(act, dict_get_pattern(sub), "Conclusion");
  if (!bunny_configuration_getf(exe, NULL, "NoMedals"))
    ret = add_exercise_medal(act, med) && ret;
  if (ret)
    return (res);
  add_message(&gl_technocore.error_buffer,
	      "Fail to write conclusion or medal for macro module (%s).\n",
	      sub);
  return (TC_CRITICAL);
}

static void		remove_macro_files(const char		*source,
				   const char		*product,
				   bool			keep)
{
  if (keep == false)
    {
      unlink(source);
      unlink(product);
    }
}

static bool		macro_marker_found(const char		*output,
				   const t_macro_guard	*guard)
{
  char			marker[256];

  snprintf(&marker[0], sizeof(marker),
	   "%s %016llx",
	   guard->cookie,
	   (unsigned long long)guard->expected);
  return (strstr(output, &marker[0]) != NULL);
}

t_technocore_result		evaluate_macro(const char			*argv,
				       t_bunny_configuration		*gen,
				       t_bunny_configuration		*exe,
				       t_technocore_activity		*act)
{
  t_macro_guard			guard;
  char				command[4096];
  char				output[64 * 1024];
  const char			*source;
  const char			*product;
  const char			*build;
  const char			*run;
  int				max;
  int				ret;
  int				nbr;
  int				timeout;
  bool				keep;

  (void)argv;
  (void)gen;
  source = ".technocore_macro_test.c";
  product = ".technocore_macro_test";
  build = NULL;
  run = NULL;
  timeout = 2;
  keep = false;
  prepare_macro_guard(&guard);
  bunny_configuration_getf(exe, &source, "SourceName");
  bunny_configuration_getf(exe, &product, "ProductName");
  bunny_configuration_getf(exe, &build, "BuildCommand");
  bunny_configuration_getf(exe, &run, "RunCommand");
  bunny_configuration_getf(exe, &timeout, "Timeout");
  bunny_configuration_getf(exe, &keep, "KeepFiles");
  if (!bunny_configuration_getf(exe, NULL, "Header") &&
      !bunny_configuration_getf(exe, NULL, "Headers[0]"))
    return (macro_report(act, exe, TC_CRITICAL, "MissingMacroHeader", "macro_internal_error"));
  if (!bunny_configuration_getf(exe, NULL, "Tests[0]"))
    return (macro_report(act, exe, TC_CRITICAL, "MissingMacroTests", "macro_internal_error"));
  if (generate_macro_source(source, exe, &nbr, &guard) == false)
    return (macro_report(act, exe, TC_CRITICAL, "MacroGenerationFailed", "macro_internal_error"));
  if (build)
    snprintf(&command[0], sizeof(command), build, source, product);
  else
    snprintf(&command[0], sizeof(command),
	     "gcc -std=c11 -Wall -Wextra -I. -I./include/ %s -o %s 2>&1",
	     source,
	     product);
  max = sizeof(output);
  ret = command_return_value(tcpopen("macro module", &command[0], &output[0], &max, NULL, 0));
  if (ret != 0)
    {
      add_to_current_report(act, &output[0], "Compilation.Output");
      add_to_current_report(act, "Error", "Compilation.Status");
      remove_macro_files(source, product, keep);
      return (macro_report(act, exe, TC_FAILURE, "MacroCompileFail", "macro_not_build"));
    }
  add_to_current_report(act, "Success", "Compilation.Status");
  if (keep == false)
    unlink(source);
  if (run)
    snprintf(&command[0], sizeof(command), run, product);
  else
    snprintf(&command[0], sizeof(command), "timeout %d ./%s 2>&1", timeout, product);
  max = sizeof(output);
  ret = command_return_value(tcpopen("macro module", &command[0], &output[0], &max, NULL, 0));
  add_to_current_report(act, &output[0], "Execution.Output");
  if (ret != 0)
    {
      if (ret == 124)
	add_exercise_message(act, dict_get_pattern("Timeout"), timeout);
      if (strstr(&output[0], &guard.cookie[0]) != NULL)
	{
	  add_to_current_report(act, dict_get_pattern("MacroGuardFail"), "Execution.Status");
	  remove_macro_files(source, product, keep);
	  return (macro_report(act, exe, TC_FAILURE, "MacroGuardFail", "macro_failed"));
	}
      remove_macro_files(source, product, keep);
      return (macro_report(act, exe, TC_FAILURE, "MacroRunFail", "macro_failed"));
    }
  if (macro_marker_found(&output[0], &guard) == false)
    {
      add_to_current_report(act, dict_get_pattern("MacroGuardFail"), "Execution.Status");
      remove_macro_files(source, product, keep);
      return (macro_report(act, exe, TC_FAILURE, "MacroGuardFail", "macro_failed"));
    }
  add_to_current_report(act, "Success", "Execution.Status");
  snprintf(&command[0], sizeof(command), "%d", nbr);
  add_to_current_report(act, &command[0], "Tests");
  remove_macro_files(source, product, keep);
  return (macro_report(act, exe, TC_SUCCESS, "MacroTestsOk", "macro_tests"));
}
