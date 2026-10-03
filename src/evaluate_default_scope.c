/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#define                         _DEFAULT_SOURCE
#include                        <ctype.h>
#include                        <stdio.h>
#include                        <string.h>
#include                        <unistd.h>
#include                        "technocore.h"

typedef struct                  s_default_scope
{
  bool                          cleanliness;
  bool                          norm;
  bool                          make;
  bool                          nested_move;
}                               t_default_scope;

static bool                     is_builtin(t_bunny_configuration *act,
                                           const char *module)
{
  const char                    *type;
  const char                    *name;

  type = "Function";
  bunny_configuration_getf(act, &type, "Type");
  if (bunny_strcasecmp(type, "Builtin") != 0)
    return (false);
  if (!bunny_configuration_getf(act, &name, "Module"))
    return (false);
  return (bunny_strcasecmp(name, module) == 0);
}

static bool                     is_make(t_bunny_configuration *act)
{
  const char                    *build;

  if (!is_builtin(act, "Construction"))
    return (false);
  if (!bunny_configuration_getf(act, &build, "Build"))
    return (false);
  return (bunny_strcasecmp(build, "Make") == 0);
}

static bool                     is_forward_move(t_bunny_configuration *act)
{
  const char                    *target;

  if (!is_builtin(act, "Move"))
    return (false);
  if (!bunny_configuration_getf(act, &target, "Target"))
    return (false);
  return (strcmp(target, "-") != 0);
}

static bool                     is_backward_move(t_bunny_configuration *act)
{
  const char                    *target;

  if (!is_builtin(act, "Move"))
    return (false);
  if (!bunny_configuration_getf(act, &target, "Target"))
    return (false);
  return (strcmp(target, "-") == 0);
}

static void                     inspect_scope(t_bunny_configuration *cnf,
                                              int start,
                                              bool bounded,
                                              t_default_scope *scope)
{
  t_bunny_configuration         *act;
  const char                    *str;
  int                           depth;

  memset(scope, 0, sizeof(*scope));
  depth = 0;
  for (int i = start; bunny_configuration_getf(cnf, &act, "Exercises[%d]", i); ++i)
    {
      if (bunny_configuration_getf(act, &str, "."))
        continue ;
      if (is_forward_move(act))
        {
          if (depth == 0)
            scope->nested_move = true;
          depth += 1;
          continue ;
        }
      if (is_backward_move(act))
        {
          if (depth == 0 && bounded)
            break ;
          if (depth > 0)
            depth -= 1;
          continue ;
        }
      if (depth != 0)
        continue ;
      if (is_builtin(act, "Cleanliness"))
        scope->cleanliness = true;
      else if (is_builtin(act, "Norm"))
        scope->norm = true;
      else if (is_make(act))
        scope->make = true;
    }
}

bool                            activity_has_default_scopes(t_bunny_configuration *cnf)
{
  t_bunny_configuration         *act;
  const char                    *str;

  for (int i = 0; bunny_configuration_getf(cnf, &act, "Exercises[%d]", i); ++i)
    {
      if (bunny_configuration_getf(act, &str, "."))
        continue ;
      if (is_forward_move(act))
        return (true);
    }
  return (false);
}

static bool                     default_bool(t_bunny_configuration *cnf,
                                             const char *field,
                                             bool value)
{
  bunny_configuration_getf(cnf, &value, "DefaultEvaluation.%s", field);
  return (value);
}

static bool                     command_has_file(const char *command)
{
  FILE                          *pip;
  char                          buffer[8];

  pip = popen(command, "r");
  if (pip == NULL)
    return (false);
  buffer[0] = '\0';
  if (fgets(buffer, sizeof(buffer), pip) == NULL)
    buffer[0] = '\0';
  pclose(pip);
  return (buffer[0] != '\0');
}

static bool                     contains_norm_sources(void)
{
  return (command_has_file(
      "find . -type f \\( -name '*.c' -o -name '*.h' \\) -print -quit"));
}

static bool                     contains_build_sources(void)
{
  if (file_exists("Makefile"))
    return (true);
  return (command_has_file(
      "find . -type f \\( -name '*.c' -o -name '*.h' -o -name '*.cc' "
      "-o -name '*.cpp' -o -name '*.cxx' -o -name '*.hh' -o -name '*.hpp' "
      "-o -name '*.hxx' \\) -print -quit"));
}

static bool                     begin_report(t_technocore_activity *tech,
                                             int *excnt,
                                             const char *name)
{
  if (!bunny_configuration_setf(tech->report, name,
                                 "Exercises[%d].Name", *excnt))
    return (false);
  if (!bunny_configuration_getf(tech->report, &tech->current_report,
                                "Exercises[%d]", *excnt))
    return (false);
  *excnt += 1;
  return (true);
}

static void                     end_report(t_technocore_activity *tech,
                                           t_technocore_result res)
{
  if (tech->current_report == NULL)
    return ;
  if (!bunny_configuration_getf(tech->current_report, NULL, "Status"))
    bunny_configuration_setf(tech->current_report,
                             res == TC_CRITICAL ? "Critical" :
                             res == TC_FAILURE ? "Failure" : "Success",
                             "Status");
}

static t_bunny_configuration    *new_default_exercise(void)
{
  t_bunny_configuration         *exe;

  if ((exe = bunny_new_configuration()) == NULL)
    return (NULL);
  bunny_configuration_setf(exe, true, "NoMedals");
  return (exe);
}

static t_technocore_result      default_cleanliness(const char *argv0,
                                                    t_bunny_configuration *cnf,
                                                    t_technocore_activity *tech,
                                                    int *excnt)
{
  t_bunny_configuration         *exe;
  t_technocore_result           res;

  if ((exe = new_default_exercise()) == NULL)
    return (TC_CRITICAL);
  if (!begin_report(tech, excnt, dict_get_pattern("DefaultCleanliness")))
    {
      bunny_delete_configuration(exe);
      return (TC_CRITICAL);
    }
  res = evaluate_cleanliness(argv0, cnf, exe, tech);
  end_report(tech, res);
  bunny_delete_configuration(exe);
  return (res);
}

static bool                     norm_path(t_bunny_configuration *cnf,
                                         char *buffer,
                                         size_t len)
{
  const char                    *path;
  const char                    *root;
  const char                    *school;

  if (bunny_configuration_getf(cnf, &path, "DefaultEvaluation.NormConfiguration"))
    {
      snprintf(buffer, len, "%s", path);
      return (true);
    }
  if (!bunny_configuration_getf(cnf, &root, "RootDir") ||
      !bunny_configuration_getf(cnf, &school, "School"))
    return (false);
  snprintf(buffer, len, "%s%sressources/schools/%s/standard_style.dab",
           root, root[0] != '\0' && root[strlen(root) - 1] == '/' ? "" : "/",
           school);
  return (true);
}

static t_technocore_result      default_norm(const char *argv0,
                                             t_bunny_configuration *cnf,
                                             t_technocore_activity *tech,
                                             int *excnt)
{
  t_bunny_configuration         *exe;
  t_technocore_result           res;
  char                          path[PATH_MAX];

  if (!norm_path(cnf, path, sizeof(path)))
    {
      add_message(&gl_technocore.error_buffer,
                  "Cannot resolve default norm configuration.\n");
      return (TC_CRITICAL);
    }
  if ((exe = bunny_new_configuration()) == NULL)
    return (TC_CRITICAL);
  if (bunny_open_configuration(path, exe) == NULL)
    {
      add_message(&gl_technocore.error_buffer,
                  "Cannot open default norm configuration %s.\n", path);
      bunny_delete_configuration(exe);
      return (TC_CRITICAL);
    }
  if (!bunny_configuration_getf(exe, NULL, "Tolerance"))
    bunny_configuration_setf(exe, 10, "Tolerance");
  bunny_configuration_setf(exe, true, "NoMedals");
  if (!begin_report(tech, excnt, dict_get_pattern("DefaultNorm")))
    {
      bunny_delete_configuration(exe);
      return (TC_CRITICAL);
    }
  res = evaluate_file_c_norm(argv0, cnf, exe, tech);
  end_report(tech, res);
  bunny_delete_configuration(exe);
  return (res);
}

static t_technocore_result      default_make(const char *argv0,
                                             t_bunny_configuration *cnf,
                                             t_technocore_activity *tech,
                                             int *excnt)
{
  t_bunny_configuration         *exe;
  t_technocore_result           res;
  bool                          check;
  bool                          install;

  if ((exe = new_default_exercise()) == NULL)
    return (TC_CRITICAL);
  check = default_bool(cnf, "Check", true);
  install = default_bool(cnf, "Install", false);
  bunny_configuration_setf(exe, true, "CheckBehaviour");
  bunny_configuration_setf(exe, true, "CheckRe");
  bunny_configuration_setf(exe, (int)check, "CheckTests");
  bunny_configuration_setf(exe, (int)install, "CheckInstall");
  if (!begin_report(tech, excnt, dict_get_pattern("DefaultMake")))
    {
      bunny_delete_configuration(exe);
      return (TC_CRITICAL);
    }
  res = evaluate_make_build(argv0, cnf, exe, tech);
  end_report(tech, res);
  if (file_exists("Makefile"))
    (void)system("make fclean > /dev/null 2>&1");
  bunny_delete_configuration(exe);
  return (res);
}

t_technocore_result             evaluate_default_scope(const char *argv0,
                                                        t_bunny_configuration *cnf,
                                                        t_technocore_activity *tech,
                                                        int *excnt,
                                                        int start,
                                                        bool bounded)
{
  t_default_scope               scope;
  t_technocore_result           res;
  t_technocore_result           final;
  bool                          has_norm_sources;
  bool                          has_build_sources;
  bool                          enabled;

  enabled = default_bool(cnf, "Enabled", true);
  if (!enabled)
    return (TC_SUCCESS);
  inspect_scope(cnf, start, bounded, &scope);
  if (bounded && scope.nested_move)
    return (TC_SUCCESS);
  has_norm_sources = contains_norm_sources();
  has_build_sources = contains_build_sources();
  final = TC_SUCCESS;
  if (default_bool(cnf, "Cleanliness", true) && !scope.cleanliness)
    {
      res = default_cleanliness(argv0, cnf, tech, excnt);
      if (res == TC_CRITICAL)
        return (res);
      if (res == TC_FAILURE)
        final = TC_FAILURE;
    }
  if (has_norm_sources && default_bool(cnf, "Norm", true) && !scope.norm)
    {
      res = default_norm(argv0, cnf, tech, excnt);
      if (res == TC_CRITICAL)
        return (res);
      if (res == TC_FAILURE)
        final = TC_FAILURE;
    }
  if (has_build_sources && default_bool(cnf, "Make", true) && !scope.make)
    {
      res = default_make(argv0, cnf, tech, excnt);
      if (res == TC_CRITICAL)
        return (res);
      if (res == TC_FAILURE)
        final = TC_FAILURE;
    }
  return (final);
}
