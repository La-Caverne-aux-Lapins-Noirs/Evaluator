/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
**
** TechnoCore
*/

#define				_GNU_SOURCE
#include			<errno.h>
#include			<fcntl.h>
#include			<stdio.h>
#include			<stdlib.h>
#include			<string.h>
#include			<sys/stat.h>
#include			<unistd.h>
#include			"technocore.h"

static const char		lapin_header[] =
"#ifndef TC_FAKE_LAPIN_H\n"
"# define TC_FAKE_LAPIN_H\n"
"# include <stdbool.h>\n"
"# include <stddef.h>\n"
"typedef struct s_bunny_vm110n\n"
"{\n"
"  unsigned char digital_outputs;\n"
"  unsigned char digital_inputs;\n"
"  unsigned char analog_outputs[2];\n"
"  unsigned char analog_inputs[2];\n"
"} t_bunny_vm110n;\n"
"t_bunny_vm110n *bunny_new_vm110n(int device);\n"
"bool bunny_vm110n_write(t_bunny_vm110n *vm);\n"
"bool bunny_vm110n_read(t_bunny_vm110n *vm);\n"
"void bunny_delete_vm110n(t_bunny_vm110n *vm);\n"
"void bunny_usleep(unsigned int usec);\n"
"#endif\n";

static const char		vm110n_runtime[] =
"#include <errno.h>\n"
"#include <stdbool.h>\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <string.h>\n"
"#include <unistd.h>\n"
"#include \"lapin.h\"\n"
"typedef struct s_tc_vm110n_input\n"
"{\n"
"  unsigned int din;\n"
"  unsigned int ain0;\n"
"  unsigned int ain1;\n"
"} t_tc_vm110n_input;\n"
"static t_tc_vm110n_input gl_inputs[256];\n"
"static size_t gl_input_count = 0;\n"
"static size_t gl_input_index = 0;\n"
"static int gl_loaded = 0;\n"
"static FILE *gl_trace = NULL;\n"
"static unsigned int gl_write_count = 0;\n"
"static unsigned int gl_max_writes = 0;\n"
"static void tc_close_trace(void)\n"
"{\n"
"  if (gl_trace != NULL)\n"
"    {\n"
"      fclose(gl_trace);\n"
"      gl_trace = NULL;\n"
"    }\n"
"}\n"
"static FILE *tc_trace(void)\n"
"{\n"
"  const char *path;\n"
"  if (gl_trace != NULL)\n"
"    return gl_trace;\n"
"  path = getenv(\"TC_VM110N_TRACE\");\n"
"  if (path == NULL || path[0] == '\\0')\n"
"    path = \"tc_vm110n_trace.txt\";\n"
"  gl_trace = fopen(path, \"ab\");\n"
"  return gl_trace;\n"
"}\n"
"static void tc_log(const char *fmt, unsigned int a, unsigned int b, unsigned int c)\n"
"{\n"
"  FILE *fp = tc_trace();\n"
"  if (fp != NULL)\n"
"    {\n"
"      fprintf(fp, fmt, a, b, c);\n"
"      fflush(fp);\n"
"    }\n"
"}\n"
"static unsigned int tc_parse_max_writes(void)\n"
"{\n"
"  const char *str = getenv(\"TC_VM110N_MAX_WRITES\");\n"
"  char *end = NULL;\n"
"  long value;\n"
"  if (str == NULL || str[0] == '\\0')\n"
"    return 0;\n"
"  value = strtol(str, &end, 0);\n"
"  if (end == str || value <= 0)\n"
"    return 0;\n"
"  return (unsigned int)value;\n"
"}\n"
"static void tc_load_scenario(void)\n"
"{\n"
"  const char *path;\n"
"  FILE *fp;\n"
"  if (gl_loaded)\n"
"    return;\n"
"  gl_loaded = 1;\n"
"  gl_max_writes = tc_parse_max_writes();\n"
"  path = getenv(\"TC_VM110N_SCENARIO\");\n"
"  if (path == NULL || path[0] == '\\0')\n"
"    return;\n"
"  fp = fopen(path, \"r\");\n"
"  if (fp == NULL)\n"
"    return;\n"
"  while (gl_input_count < sizeof(gl_inputs) / sizeof(gl_inputs[0]))\n"
"    {\n"
"      int din;\n"
"      int ain0;\n"
"      int ain1;\n"
"      int ret = fscanf(fp, \" %i %i %i\", &din, &ain0, &ain1);\n"
"      if (ret != 3)\n"
"        break;\n"
"      gl_inputs[gl_input_count].din = din & 0xFF;\n"
"      gl_inputs[gl_input_count].ain0 = ain0 & 0xFF;\n"
"      gl_inputs[gl_input_count].ain1 = ain1 & 0xFF;\n"
"      gl_input_count += 1;\n"
"    }\n"
"  fclose(fp);\n"
"}\n"
"t_bunny_vm110n *bunny_new_vm110n(int device)\n"
"{\n"
"  t_bunny_vm110n *vm;\n"
"  tc_load_scenario();\n"
"  vm = calloc(1, sizeof(*vm));\n"
"  tc_log(\"NEW device=%u\\n\", (unsigned int)device, 0, 0);\n"
"  return vm;\n"
"}\n"
"bool bunny_vm110n_write(t_bunny_vm110n *vm)\n"
"{\n"
"  if (vm == NULL)\n"
"    return false;\n"
"  tc_load_scenario();\n"
"  tc_log(\"WRITE dout=0x%02X aout0=%03u aout1=%03u\\n\",\n"
"         vm->digital_outputs & 0xFF, vm->analog_outputs[0] & 0xFF,\n"
"         vm->analog_outputs[1] & 0xFF);\n"
"  gl_write_count += 1;\n"
"  if (gl_max_writes != 0 && gl_write_count >= gl_max_writes)\n"
"    {\n"
"      tc_close_trace();\n"
"      _exit(0);\n"
"    }\n"
"  return true;\n"
"}\n"
"bool bunny_vm110n_read(t_bunny_vm110n *vm)\n"
"{\n"
"  size_t index;\n"
"  if (vm == NULL)\n"
"    return false;\n"
"  tc_load_scenario();\n"
"  index = gl_input_index;\n"
"  if (gl_input_count != 0)\n"
"    {\n"
"      if (index >= gl_input_count)\n"
"        index = gl_input_count - 1;\n"
"      vm->digital_inputs = gl_inputs[index].din & 0x1F;\n"
"      vm->analog_inputs[0] = gl_inputs[index].ain0 & 0xFF;\n"
"      vm->analog_inputs[1] = gl_inputs[index].ain1 & 0xFF;\n"
"      gl_input_index += 1;\n"
"    }\n"
"  tc_log(\"READ din=0x%02X ain0=%03u ain1=%03u\\n\",\n"
"         vm->digital_inputs & 0xFF, vm->analog_inputs[0] & 0xFF,\n"
"         vm->analog_inputs[1] & 0xFF);\n"
"  return true;\n"
"}\n"
"void bunny_delete_vm110n(t_bunny_vm110n *vm)\n"
"{\n"
"  if (vm != NULL)\n"
"    free(vm);\n"
"  tc_log(\"DELETE\\n\", 0, 0, 0);\n"
"  tc_close_trace();\n"
"}\n"
"void bunny_usleep(unsigned int usec)\n"
"{\n"
"  tc_load_scenario();\n"
"  tc_log(\"SLEEP usec=%u\\n\", usec, 0, 0);\n"
"}\n";

static bool			write_text_file
(const char			*path,
 const char			*content)
{
  FILE				*fp;

  if ((fp = fopen(path, "wb")) == NULL)
    return (false);
  if (content != NULL && content[0] != '\0')
    if (fwrite(content, strlen(content), 1, fp) != 1)
      {
	fclose(fp);
	return (false);
      }
  return (fclose(fp) == 0);
}

static bool			read_text_file
(const char			*path,
 char				**out)
{
  FILE				*fp;
  long				size;
  char				*buffer;

  *out = NULL;
  if ((fp = fopen(path, "rb")) == NULL)
    return (false);
  if (fseek(fp, 0, SEEK_END) != 0 || (size = ftell(fp)) < 0 ||
      fseek(fp, 0, SEEK_SET) != 0)
    goto Fail;
  if ((buffer = malloc((size_t)size + 1)) == NULL)
    goto Fail;
  if (size != 0 && fread(buffer, (size_t)size, 1, fp) != 1)
    {
      free(buffer);
      goto Fail;
    }
  buffer[size] = '\0';
  if (fclose(fp) != 0)
    {
      free(buffer);
      return (false);
    }
  *out = buffer;
  return (true);

 Fail:
  fclose(fp);
  return (false);
}

static t_technocore_result	prepare_vm110n_trace
(const char			*argv0,
 const char			*name,
 t_bunny_configuration		*node,
 t_program_trace		*trace)
{
  const char			*scenario;
  int				max_writes;
  char				path[PATH_MAX];

  if (bunny_configuration_getf(node, &trace->expected, "Expected") == false &&
      bunny_configuration_getf(node, &trace->expected, "Trace") == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "%s: Missing VM110N.Expected field for program test %s.\n",
		  argv0, name);
      return (TC_CRITICAL);
    }
  scenario = "";
  max_writes = 0;
  bunny_configuration_getf(node, &scenario, "Scenario");
  bunny_configuration_getf(node, &max_writes, "MaxWrites");
  snprintf(trace->type, sizeof(trace->type), "VM110N");
  snprintf(trace->helper_dir, sizeof(trace->helper_dir), ".technocore_vm110n");
  snprintf(trace->trace_file, sizeof(trace->trace_file), "%s/trace.txt", trace->helper_dir);
  snprintf(trace->scenario_file, sizeof(trace->scenario_file), "%s/scenario.txt", trace->helper_dir);
  snprintf(trace->max_writes, sizeof(trace->max_writes), "%d", max_writes);
  if (mkdir(trace->helper_dir, 0755) == -1 && errno != EEXIST)
    {
      add_message(&gl_technocore.error_buffer,
		  "%s: Cannot create VM110N helper directory for %s: %s.\n",
		  argv0, name, strerror(errno));
      return (TC_CRITICAL);
    }
  snprintf(path, sizeof(path), "%s/lapin.h", trace->helper_dir);
  if (write_text_file(path, lapin_header) == false)
    goto WriteFail;
  snprintf(path, sizeof(path), "%s/vm110n.c", trace->helper_dir);
  if (write_text_file(path, vm110n_runtime) == false ||
      write_text_file(trace->scenario_file, scenario) == false ||
      write_text_file(trace->trace_file, "") == false)
    goto WriteFail;
  trace->enabled = true;
  return (TC_SUCCESS);

 WriteFail:
  add_message(&gl_technocore.error_buffer,
	      "%s: Cannot write VM110N helper files for %s: %s.\n",
	      argv0, name, strerror(errno));
  return (TC_CRITICAL);
}

t_technocore_result		prepare_program_trace
(const char			*argv0,
 const char			*name,
 t_bunny_configuration		*program,
 t_program_trace		*trace)
{
  t_bunny_configuration		*node;

  memset(trace, 0, sizeof(*trace));
  if (bunny_configuration_getf(program, &node, "VM110N"))
    return (prepare_vm110n_trace(argv0, name, node, trace));
  return (TC_SUCCESS);
}

void				apply_program_trace_environment
(const t_program_trace		*trace)
{
  if (trace == NULL || trace->enabled == false)
    return ;
  if (strcmp(trace->type, "VM110N") == 0)
    {
      setenv("TC_VM110N_TRACE", trace->trace_file, 1);
      setenv("TC_VM110N_SCENARIO", trace->scenario_file, 1);
      setenv("TC_VM110N_MAX_WRITES", trace->max_writes, 1);
    }
}

t_technocore_result		check_program_trace
(t_technocore_activity		*act,
 const char			*name,
 const t_program_trace		*trace)
{
  t_technocore_result		ret;
  char				*user;

  if (trace == NULL || trace->enabled == false)
    return (TC_SUCCESS);
  if (read_text_file(trace->trace_file, &user) == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot read %s trace file %s.\n", trace->type, trace->trace_file);
      return (TC_CRITICAL);
    }
  if (strcmp(trace->type, "VM110N") == 0)
    ret = do_vm110n_trace_diff(act, name, user, trace->expected, NULL);
  else
    ret = do_trace_diff(act, trace->type, name, user, trace->expected, NULL);
  free(user);
  return (ret);
}
