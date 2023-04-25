/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

extern char		gl_last_new_type;

void			load_norm_configuration(t_parsing	*p,
						t_bunny_configuration *e);

int			main(void)
{
  t_parsing		p;
  t_bunny_configuration	*cnf;
  t_technocore_activity	a;
  const char		*s;
  ssize_t		i;

  memset(&p, 0, sizeof(p));
  memset(&a, 0, sizeof(a));
  assert(cnf = bunny_new_configuration());
  gl_bunny_read_whitespace = read_whitespace;

  goto FINAL;

  /////////////////////////
  // ON TESTE LE PARSING //
  /////////////////////////

  i = 0;
  s = "      \n\r\t      /* lel \n   */\n    // lol      \n     !";
  assert(read_whitespace(s, &i) == true);
  assert(s[i] == '!');

  i = 0;
  assert(read_whitespace("//    ", &i) == true);

  i = 0;
  assert(read_whitespace("/*    ", &i) == false);

  i = 0; assert(read_identifier(&p, "lel42_", &i, false) == 1);
  i = 0; assert(read_identifier(&p, "42lel_", &i, false) == 0);
  i = 0; assert(read_identifier(&p, "const", &i, false) == 0);
  i = 0; assert(read_identifier(&p, "const", &i, true) == 1);
  i = 0; assert(read_identifier_list(&p, "lel, lol, lul", &i) == 1);
  i = 0; assert(read_identifier_list(&p, "lel, 42, lol", &i) == -1);
  i = 0; assert(read_identifier_list(&p, "", &i) == 0);

  i = 0; assert(read_jump_statement(&p, "return (5);", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "return ;", &i) == 1);

  i = 0; assert(read_jump_statement(&p, "goto Identifier;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "goto Identifier", &i) == -1);
  i = 0; assert(read_jump_statement(&p, "goto ", &i) == -1);

  i = 0; assert(read_jump_statement(&p, "continue ;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "continue ", &i) == -1);

  i = 0; assert(read_jump_statement(&p, "break ;", &i) == 1);
  i = 0; assert(read_jump_statement(&p, "break ", &i) == -1);


  i = 0; assert(read_primary_expression(&p, "Identifier", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "\"This is it...\n\"", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "42", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "4.2", &i) == 1);
  i = 0; assert(read_primary_expression(&p, "   ", &i) == 0);

  i = 0; assert(read_postfix_expression(&p, "Identifier.SubIdentifier->FarIdentifier++--", &i) == 1);

  i = 0; assert(read_unary_operator(&p, "&", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "*", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "+", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "-", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "~", &i) == 1);
  i = 0; assert(read_unary_operator(&p, "!", &i) == 1);

  i = 0; assert(read_unary_expression(&p, "++--~(const double)sizeof(int)", &i) == 1);

  i = 0;
  s =
    "register auto unsigned float const volatile "
    "* const * identifier, * const id2 = a || b && c | d ^ e & f == g != h <= i >= j < k > l << m >> n "
    "+ o - p * q / r % (int)s;"
    ;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  s = "int var[42] = {0, 1, 2, 3, 4};";
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  assert(read_declaration(&p, "int i", &i) == -1);

  i = 0;
  assert(read_declaration(&p, "int i = ", &i) == -1);

  i = 0;
  assert(read_statement(&p, "puts(\"lel\");", &i) == 1); // une expression

  i = 0;
  s = "i = 42, j = 43";
  if (read_expression(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "i == 42 ? 5 + 1 : 3 - lel";
  if (read_conditional_expression(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "const int * function_symbol() {}";
  if (read_function_declaration(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "const int * function_symbol();";
  if (read_declaration(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "for (i = 0; i < 10; ++i) { }";
  if (read_iteration_statement(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "if ((i = new()) == -1) { }";
  if (read_selection_statement(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "const int * function_symbol(int a, double b, ...) {\n"
    "if (i == 42) { puts(\"C'est cool\", 5[2], 'c'); i += 1; } else if (*i == 43) { } else atoi(52);\n"
    "start:\n"
    "while (i < 100) { i = &i + 1; }\n"
    "do { /* yeah */ i -= 32 + ~j; } while (j != !k);\n"
    "for (i = 50; i < 100; ++i) { break ; continue ; }\n"
    "goto start;\n"
    "return (57 + 1);\n"
    "}\n";
  if (read_function_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "typedef struct s_struct {"
    "int a:35;\n"
    "short;\n"
    "double b;\n"
    "} t_struct;";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "typedef int lel;";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "typedef union u_union {"
    "int a:35;\n"
    "short;\n"
    "double b;\n"
    "} t_struct;";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "typedef enum e_enum {"
    "CONSTANT,\n"
    "ZOUBIDA = 42,\n"
    "ANOTHER\n"
    "} t_enum;";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "int main(void);";
  p.last_error_id = -1;
  if (read_declaration(&p, s, &i) != 1)
    goto Error;
  i = 0;
  p.last_error_id = -1;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "int global = 42;\n"
    "void switch_function() {\n"
    "switch (i + 2) {\n"
    "case Symbol: puts('e');\n"
    "case OtherSymbol: strtol(57);\n"
    "default: { return (0); }\n"
    "}\n"
    "}\n"
    ;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s = "typedef void * __timer_t;";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "typedef float float_t;\n"
    "typedef double double_t;\n"
    ;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "extern int __fpclassify (double __value) __attribute__ ((__nothrow__ , __leaf__));"
    ;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "int func(double, int);"
    ;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "extern double frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); "
    "extern double __frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));"
    ;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  i = 0;
  s =
    "extern double modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)); "
    "extern double __modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));"
    ;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;

  ////////////////////////////
  // ON TESTE LA MOULINETTE //
  ////////////////////////////

 FINAL:
  // Décompte de fonctions par fichier
  i = 0;
  bunny_configuration_setf(cnf, 2, "FunctionPerFile[0]");
  bunny_configuration_setf(cnf, -3, "FunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = " void prototype(void); int global = 42; void func1(void){ } void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_per_file.active);
  assert(p.function_per_file.counter == 3);
  assert(p.function_per_file.value == 2);
  assert(p.function_per_file.pts == 3);

  // Décompte de fonction non statique
  i = 0;
  bunny_configuration_setf(cnf, 1, "NonStaticFunctionPerFile[0]");
  bunny_configuration_setf(cnf, -1, "NonStaticFunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = "  static void func1(void){ } static void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.non_static_function_per_file.active);
  assert(p.non_static_function_per_file.counter == 1);
  assert(p.non_static_function_per_file.value == 1);
  assert(p.non_static_function_per_file.pts == 1);

  // Verification des styles, suffixes et prefixes A CODER
  i = 0;
  bunny_configuration_setf(cnf, 1, "NonStaticFunctionPerFile[0]");
  bunny_configuration_setf(cnf, -1, "NonStaticFunctionPerFile[1]");
  load_norm_configuration(&p, cnf);
  s = "  static void func1(void){ } static void func2(void){ } void func3(void){ } ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.non_static_function_per_file.active);
  assert(p.non_static_function_per_file.counter == 1);
  assert(p.non_static_function_per_file.value == 1);
  assert(p.non_static_function_per_file.pts == 1);

  // Verification des styles des symboles
  /// ON COMMENCE AVEC PREFIX ET MIXED CASE
  i = 0;
  bunny_configuration_setf(cnf, MIXED_CASE, "FunctionNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "FunctionNameStyle.Points");

  bunny_configuration_setf(cnf, "PFX_", "FunctionNameInfix.Value");
  bunny_configuration_setf(cnf, "Prefix", "FunctionNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "FunctionNameInfix.Points");

  load_norm_configuration(&p, cnf);
  s = " void PFX_FUNCTION_NAME(void) { } void PFX_FUNCTION_NAME(void);  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_style.active);
  assert(p.function_style.value == 0);
  assert(p.function_style.pts == 10);
  assert(p.function_style.counter == 0);
  assert(p.function_infix.active);
  assert(strncmp(p.function_infix.value, "PFX_", 4) == 0);
  assert(p.function_infix.pts == 8);
  assert(p.function_infix.counter == 0);

  i = 0;
  s = " void FUNCTION_NAME(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_infix.counter == 1);

  i = 0;
  s = " void FUNCTION_NAME(void);  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  // Les prototypes ne comptent pas dans le décompte des fonctions mal écrites
  // car on peut prototyper des fonctions externes
  assert(p.function_infix.counter == 1);

  i = 0;
  s = " void pfx_function_name(void) {}  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_infix.counter == 1);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void pfx_function_name(void);  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  // Comme au dessus, ca ne compte pas quand c'est juste un prototype
  assert(p.function_infix.counter == 1);
  assert(p.function_style.counter == 1);

  i = 0;
  bunny_configuration_setf(cnf, "Suffix", "FunctionNameInfix.Position");
  bunny_configuration_setf(cnf, "_suffix", "FunctionNameInfix.Value");
  bunny_configuration_setf(cnf, SNAKE_CASE, "FunctionNameStyle.Value");
  load_norm_configuration(&p, cnf);
  s = " void function_name_suffix(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_infix.counter == 0);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfx_function_name(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_infix.counter == 1);
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void pfxfunctionname(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_infix.counter == 2);
  assert(p.function_style.counter == 1);

  i = 0;
  s = " void pfx_function__name(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_infix.counter == 3);
  assert(p.function_style.counter == 2);

  i = 0;
  s = " void PFX_FUNCTION_NAME(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_infix.counter == 4);
  assert(p.function_style.counter == 3);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  i = 0;
  bunny_configuration_setf(cnf, CAMEL_CASE, "FunctionNameStyle.Value");
  load_norm_configuration(&p, cnf);
  s = " void theFunctionName(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void TheFunctionName(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_style.counter == 1);

  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();

  i = 0;
  bunny_configuration_setf(cnf, PASCAL_CASE, "FunctionNameStyle.Value");
  s = " void TheFunctionName(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  load_norm_configuration(&p, cnf);
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_style.counter == 0);

  i = 0;
  s = " void theFunctionName(void) { }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.function_style.counter == 1);

  // Vérification des variables globales
  i = 0;
  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  
  bunny_configuration_setf(cnf, SNAKE_CASE, "GlobalVariableNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "GlobalVariableNameStyle.Points");
  bunny_configuration_setf(cnf, "gl_", "GlobalVariableNameInfix.Value");
  bunny_configuration_setf(cnf, "Prefix", "GlobalVariableNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "GlobalVariableNameInfix.Points");

  bunny_configuration_setf(cnf, MIXED_CASE, "LocalVariableNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "LocalVariableNameStyle.Points");
  bunny_configuration_setf(cnf, "_l", "LocalVariableNameInfix.Value");
  bunny_configuration_setf(cnf, "Suffix", "LocalVariableNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "LocalVariableNameInfix.Points");
  bunny_configuration_setf(cnf, 1, "LocalVariableInlineInitForbidden.Value");
  bunny_configuration_setf(cnf, 15, "LocalVariableInlineInitForbidden.Points");

  load_norm_configuration(&p, cnf);
  s = " int gl_value = 50;  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.global_variable_style.active);
  assert(p.global_variable_infix.active);
  assert(p.global_variable_style.value == SNAKE_CASE);
  assert(p.global_variable_style.counter == 0);
  assert(p.global_variable_infix.counter == 0);

  i = 0;
  s = " int value = 50;  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.global_variable_style.counter == 0);
  assert(p.global_variable_infix.counter == 1);

  i = 0;
  s = " int func(void) { int LOCAL_L; LOCAL_L = 52; return 2; }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.local_variable_infix.active);
  assert(p.local_variable_infix.counter == 0);
  assert(p.local_variable_style.counter == 0);

  i = 0;
  s = " int func(void) { int LOCAL_L = 52; return 2; }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.local_variable_infix.active);
  assert(p.local_variable_infix.counter == 0);
  assert(p.local_variable_style.counter == 0);
  assert(p.local_variable_inline_init_forbidden.counter == 1);

  i = 0;
  s = " int func(void) { int local; local = 52; return (2); }  ";
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.local_variable_infix.counter == 1);
  assert(p.local_variable_style.counter == 1);

  /// STRUCT
  bunny_configuration_setf(cnf, SNAKE_CASE, "StructNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "StructNameStyle.Points");
  bunny_configuration_setf(cnf, "s_", "StructNameStyle.Value");
  bunny_configuration_setf(cnf, "Prefix", "StructNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "StructNameInfix.Points");

  bunny_configuration_setf(cnf, SNAKE_CASE, "StructAttributeNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "StructAttributeNameStyle.Points");
  bunny_configuration_setf(cnf, "sa_", "StructAttributeNameStyle.Value");
  bunny_configuration_setf(cnf, "Prefix", "StructAttributeNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "StructAttributeNameInfix.Points");

  bunny_configuration_setf(cnf, SNAKE_CASE, "EnumNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "EnumNameStyle.Points");
  bunny_configuration_setf(cnf, "E_", "EnumNameStyle.Value");
  bunny_configuration_setf(cnf, "Prefix", "EnumNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "EnumNameInfix.Points");

  bunny_configuration_setf(cnf, MIXED_CASE, "EnumConstantStyle.Value");
  bunny_configuration_setf(cnf, 10, "EnumConstantStyle.Points");
  bunny_configuration_setf(cnf, "C_", "EnumConstantStyle.Value");
  bunny_configuration_setf(cnf, "Prefix", "EnumConstantInfix.Position");
  bunny_configuration_setf(cnf, 8, "EnumConstantInfix.Points");

  bunny_configuration_setf(cnf, SNAKE_CASE, "UnionNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "UnionNameStyle.Points");
  bunny_configuration_setf(cnf, "u_", "UnionNameStyle.Value");
  bunny_configuration_setf(cnf, "Prefix", "UnionNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "UnionNameInfix.Points");

  bunny_configuration_setf(cnf, SNAKE_CASE, "UnionAttributeNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "UnionAttributeNameStyle.Points");
  bunny_configuration_setf(cnf, "su_", "UnionAttributeNameStyle.Value");
  bunny_configuration_setf(cnf, "Prefix", "UnionAttributeNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "UnionAttributeNameInfix.Points");
  
  bunny_configuration_setf(cnf, SNAKE_CASE, "TypedefNameStyle.Value");
  bunny_configuration_setf(cnf, 10, "TypedefNameStyle.Points");
  bunny_configuration_setf(cnf, "t_", "TypedefNameStyle.Value");
  bunny_configuration_setf(cnf, "Prefix", "TypedefNameInfix.Position");
  bunny_configuration_setf(cnf, 8, "TypedefNameInfix.Points");

  bunny_configuration_setf(cnf, 1, "TypedefMatching.Value");
  bunny_configuration_setf(cnf, 3, "TypedefNameStyle.Points");
  
  load_norm_configuration(&p, cnf);
  s =
    " struct s_struct { int sa_a; int sa_b; int sa_c; }; \n"
    " typedef struct s_lol { int sa_d; int sa_e; int sa_f; } t_lol; \n"
    " union u_union { int su_a; int su_b; int su_c; }; \n"
    " typedef union u_lel { int su_d; int su_e; int su_f; } t_lel; \n"
    " enum e_enum { E_LOL_ENUM, E_LAL_ENUM }; \n"
    " typedef enum e_lal { E_THE_ENUM, E_THA_ENUM } t_lal; \n"
    ;
  i = 0;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.struct_style.counter == 0);
  assert(p.enum_style.counter == 0);
  assert(p.enum_constant_style.counter == 0);
  assert(p.union_style.counter == 0);
  assert(p.struct_attribute_style.counter == 0);
  assert(p.union_attribute_style.counter == 0);
  assert(p.typedef_style.counter == 0);

  s =
    " struct sx_STRUCT { int sax_A; int sax_B; int sax_C; }; \n"
    " typedef struct sx_LOL { int sax_D; int sax_E; int sax_F; } tx_LOL; \n"
    " union ux_UNION { int sux_A; int sux_B; int sux_C; }; \n"
    " typedef union ux_LEL { int sux_D; int sux_E; int sux_F; } tx_LEL; \n"
    " enum ex_ENUM { EX_lol_enum, EX_lal_enum }; \n"
    " typedef enum ex_LAL { EX_the_enum, EX_tha_enum } tx_LALOL; \n"
    ;
  i = 0;
  p.last_error_id = -1;
  gl_last_new_type = 0;
  if (read_translation_unit(&p, s, &i) != 1)
    goto Error;
  assert(p.struct_style.counter == 2);
  assert(p.enum_style.counter == 2);
  assert(p.enum_constant_style.counter == 4);
  assert(p.union_style.counter == 2);
  assert(p.struct_attribute_style.counter == 6);
  assert(p.union_attribute_style.counter == 6);
  assert(p.typedef_style.counter == 3);
  assert(p.typedef_matching.counter == 1);
  
  /////////////////////////////////
  // GRAND TEST FINAL DE PARSING //
  /////////////////////////////////

  p.last_error_id = -1;
  gl_last_new_type = 0;
  s = NULL;
  bunny_delete_configuration(cnf);
  cnf = bunny_new_configuration();
  if (evaluate_file_c_norm("./../include/technocore.h", cnf, cnf, &a) != TC_SUCCESS)
    goto Error;

  return (EXIT_SUCCESS);

 Error:
  if (s)
    printf("Stopped at %s\nIn:\n%s\n\n", &s[i], s);
  for (int i = 0; i < p.last_error_id; ++i)
    printf("%s\n", p.last_error_msg[i]);
  return (EXIT_FAILURE);
}
