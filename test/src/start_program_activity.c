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
  t_bunny_configuration	*global;
  t_bunny_configuration	*local;
  t_technocore_activity	act;

  assert(dict_open());
  dict_set_language("FR");
  assert(act.current_report = bunny_new_configuration());
  goto Test4;

 Test1: ;
  t_bunny_configuration *empty = bunny_new_configuration();

  assert(start_program_activity("empty", empty, empty, &act) == TC_CRITICAL);
  assert(strcmp(gl_technocore.error_buffer.message, "empty: Missing Command field for program test .\n") == 0);
  
 Test12: ;
  const char		*code9 =
    "\n"
    "[Local\n"
    "  Name = \"Test\"\n"
    "  {Program/n"
    "    [\n"
    "      Command = \"echo Test\"\n"
    "      Timeout = 200\n"
    "      ReturnValue = 0\n"
    "      {Interactions\n"
    "        [\n"
    "          Output = \"Test\"\n"
    "        ]\n"
    "      }\n"
    "    ]\n"
    "  }\n"
    "]"
    ;
  
  char			buffer[4096];

  bunny_delete_configuration(act.current_report);
  memset(&act, 0, sizeof(act));
  assert((act.current_report = bunny_new_configuration()));

  snprintf(buffer, sizeof(buffer), code9);
  assert(global = bunny_read_configuration(BC_DABSIC, buffer, NULL));
  // bunny_save_configuration(BC_DABSIC, "/dev/stderr", global);
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_program_activity("aaa", global, local, &act) == TC_SUCCESS);
  bunny_delete_configuration(global);
  
 Test2: ;
  const char		*code =
    "\n"
    "[Local\n"
    "  Name = \"Test\"\n"
    "  {Program/n"
    "    [\n"
    "      Command = \"echo -n 'Test'\"\n"
    "      Timeout = 200\n"
    "      ReturnValue = 0\n"
    "      {Interactions\n"
    "        [\n"
    "          Output = \"Test%s\"\n"
    "        ]\n"
    "      }\n"
    "    ]\n"
    "  }\n"
    "]"
    ;


  bunny_delete_configuration(act.current_report);
  memset(&act, 0, sizeof(act));
  assert((act.current_report = bunny_new_configuration()));
  
  snprintf(buffer, sizeof(buffer), code, "");
  assert(global = bunny_read_configuration(BC_DABSIC, buffer, NULL));
  // bunny_save_configuration(BC_DABSIC, "/dev/stderr", global);
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_program_activity("aaa", global, local, &act) == TC_SUCCESS);
  bunny_delete_configuration(global);

  bunny_delete_configuration(act.current_report);
  memset(&act, 0, sizeof(act));
  assert((act.current_report = bunny_new_configuration()));
  
  snprintf(buffer, sizeof(buffer), code, "2");
  assert(global = bunny_read_configuration(BC_DABSIC, buffer, NULL));
  // bunny_save_configuration(BC_DABSIC, "/dev/stderr", global);
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_program_activity("aaa", global, local, &act) == TC_FAILURE);
  bunny_delete_configuration(global);
  const char		*tmp;
  const char		*cmp =
    "Test: Les chaines de caractères ne correspondent pas.\n"
    "Votre chaine, puis la notre:\n"
    "Test\n"
    "Test2\n"
    "   ^\n"
    ;
  
  assert(bunny_configuration_getf(act.current_report, &tmp, "Message[0]"));
  puts(cmp);
  puts(tmp);
  assert(strcmp(cmp, tmp) == 0);

 Test3: ;
  // Deux interactions
  const char		*code2 =
    "\n"
    "[Local\n"
    "  Name = \"Test\"\n"
    "  {Program/n"
    "    [\n"
    "      Command = \"cat\"\n"
    "      Timeout = 50\n"
    "      ReturnValue = 0\n"
    "      {Interactions\n"
    "        [\n"
    "          Input = \"Test\\n\"\n"
    "          Output = \"Test\\n\"\n"
    "        ],\n"
    "        [\n"
    "          Input = \"Test2\\n\"\n"
    "          Output = \"Test2\\n\"\n"
    "        ],\n"
    "        [\n"
    "          Input = \"\"\n"
    "          Output = \"\"\n"
    "        ]\n"
    "      }\n"
    "    ]\n"
    "  }\n"
    "]"
    ;

  bunny_delete_configuration(act.current_report);
  memset(&act, 0, sizeof(act));
  assert((act.current_report = bunny_new_configuration()));

  snprintf(buffer, sizeof(buffer), code2);
  assert(global = bunny_read_configuration(BC_DABSIC, buffer, NULL));
  // bunny_save_configuration(BC_DABSIC, "/dev/stderr", global);
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_program_activity("aaa", global, local, &act) == TC_SUCCESS);
  bunny_delete_configuration(global);

 Test4: ;
  // Timeout
  const char		*code3 =
    "\n"
    "[Local\n"
    "  Name = \"Test\"\n"
    "  {Program/n"
    "    [\n"
    "      Command = \"cat\"\n"
    "      Timeout = 1\n"
    "      ReturnValue = 0\n"
    "      {Interactions\n"
    "        [\n"
    "          Output = \"Timeout is coming\"\n"
    "        ]\n"
    "      }\n"
    "    ]\n"
    "  }\n"
    "]"
    ;

  bunny_delete_configuration(act.current_report);
  memset(&act, 0, sizeof(act));
  assert((act.current_report = bunny_new_configuration()));

  snprintf(buffer, sizeof(buffer), code3);
  assert(global = bunny_read_configuration(BC_DABSIC, buffer, NULL));
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_program_activity("aaa", global, local, &act) == TC_FAILURE);
  bunny_delete_configuration(global);

 Test5: ;
  // Mauvaise valeur de retour
  const char		*code4 =
    "\n"
    "[Local\n"
    "  Name = \"Test\"\n"
    "  {Program/n"
    "    [\n"
    "      Command = \"cat\"\n"
    "      Timeout = 20\n"
    "      ReturnValue = 1\n"
    "      {Interactions\n"
    "        [\n"
    "          Input = \"Test\\n\"\n"
    "          Output = \"Test\\n\"\n"
    "        ],\n"
    "        [\n"
    "          Input = \"Test2\\n\"\n"
    "          Output = \"Test2\\n\"\n"
    "        ],\n"
    "        [\n"
    "          Input = \"\"\n"
    "          Output = \"\"\n"
    "        ]\n"
    "      }\n"
    "    ]\n"
    "  }\n"
    "]"
    ;

  bunny_delete_configuration(act.current_report);
  memset(&act, 0, sizeof(act));
  assert((act.current_report = bunny_new_configuration()));

  snprintf(buffer, sizeof(buffer), code4);
  assert(global = bunny_read_configuration(BC_DABSIC, buffer, NULL));
  // bunny_save_configuration(BC_DABSIC, "/dev/stderr", global);
  assert(bunny_configuration_getf(global, &local, "Local"));
  assert(start_program_activity("aaa", global, local, &act) == TC_FAILURE);
  bunny_delete_configuration(global);

  
  return (EXIT_SUCCESS);
}

