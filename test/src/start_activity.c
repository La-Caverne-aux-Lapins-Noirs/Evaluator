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
  int				document_was_evaluated;
  const char			*code =
    "Language = \"FR\"\n"
    "MaximumRam = 1024 * 1024\n"
    "[DefaultEvaluation\n"
    "  Enabled = false\n"
    "]\n"
    "{Exercises\n"
    "  \"LEGACY DOCBUILDER DIRECTIVE\",\n"
    "  [\n"
    "    Name = \"DOCUMENT\"\n"
    "    Type = \"Document\"\n"
    "    ConditionalVar = \"this is deliberately invalid\"\n"
    "    Module = \"Critical\"\n"
    "    StopOnFailure = true\n"
    "    SetVar = \"DocumentWasEvaluated\"\n"
    "  ],\n"
    "  [\n"
    "    Name = \"DEMO\"\n"
    "    Type = \"Builtin\"\n"
    "    Module = \"Failure\"\n"
    "    StopOnFailure = true\n"
    "  ]\n"
    "}\n"
    ;

  assert(dict_open());
  assert((cnf = bunny_read_configuration(BC_DABSIC, code, NULL)));
  assert(start_activity("aaa", cnf) == TC_FAILURE);
  assert(strcmp(bunny_configuration_get_name(gl_dictionnary), "FR") == 0);
  assert(get_max_heap_size() == 1024 * 1024);
  assert(!bunny_configuration_getf(cnf, &document_was_evaluated,
                                   "Variables.DocumentWasEvaluated"));

  const char                    *implicit =
    "Language = \"FR\"\n"
    "[DefaultEvaluation\n"
    "  Cleanliness = true\n"
    "  Norm = false\n"
    "  Make = false\n"
    "]\n";
  t_bunny_configuration         *report;
  const char                    *str;

  assert(system("rm -rf default_scope_tmp && mkdir default_scope_tmp") == 0);
  assert(chdir("default_scope_tmp") == 0);
  bunny_delete_configuration(cnf);
  assert((cnf = bunny_read_configuration(BC_DABSIC, implicit, NULL)));
  assert(start_activity("aaa", cnf) == TC_SUCCESS);
  assert((report = bunny_open_configuration("./report.dab", NULL)));
  assert(bunny_configuration_getf(report, &str, "Exercises[0].Name"));
  assert(strcmp(str, "Propreté du rendu") == 0);
  assert(bunny_configuration_getf(report, &str, "Exercises[0].Status"));
  assert(strcmp(str, "Success") == 0);
  bunny_delete_configuration(report);
  assert(chdir("..") == 0);
  assert(system("rm -rf default_scope_tmp") == 0);

  const char                    *explicit_scope =
    "Language = \"FR\"\n"
    "[DefaultEvaluation\n"
    "  Cleanliness = true\n"
    "  Norm = false\n"
    "  Make = false\n"
    "]\n"
    "{Exercises\n"
    "  [\n"
    "    Type = \"Builtin\"\n"
    "    Module = \"Move\"\n"
    "    Target = \"scope\"\n"
    "    NoReport\n"
    "  ],\n"
    "  [\n"
    "    Type = \"Builtin\"\n"
    "    Module = \"Cleanliness\"\n"
    "    Name = \"Explicit cleanliness\"\n"
    "    NoMedals\n"
    "  ],\n"
    "  [\n"
    "    Type = \"Builtin\"\n"
    "    Module = \"Move\"\n"
    "    Target = \"-\"\n"
    "    NoReport\n"
    "  ]\n"
    "}\n";

  assert(system("rm -rf default_scope_tmp && mkdir -p default_scope_tmp/scope") == 0);
  assert(chdir("default_scope_tmp") == 0);
  bunny_delete_configuration(cnf);
  assert((cnf = bunny_read_configuration(BC_DABSIC, explicit_scope, NULL)));
  assert(start_activity("aaa", cnf) == TC_SUCCESS);
  assert((report = bunny_open_configuration("./report.dab", NULL)));
  assert(bunny_configuration_getf(report, &str, "Exercises[0].Name"));
  assert(strcmp(str, "Explicit cleanliness") == 0);
  assert(!bunny_configuration_getf(report, &str, "Exercises[1].Name"));
  bunny_delete_configuration(report);
  assert(chdir("..") == 0);
  assert(system("rm -rf default_scope_tmp") == 0);
  return (EXIT_SUCCESS);
}
