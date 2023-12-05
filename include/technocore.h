/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#ifndef			__TECHNOCORE_H__
# define		__TECHNOCORE_H__
# include		<stdbool.h>
# include		"technocore_api.h"

#ifdef			NDEBUG

# ifndef		TECHNOCORE_CONFIGURATION
#  define		TECHNOCORE_CONFIGURATION			"/opt/technocore/configuration.dab"
# endif
# ifndef		TECHNOCORE_DICTIONNARY
#  define		TECHNOCORE_DICTIONNARY				"/opt/technocore/dictionnary.dab"
# endif

#else

# ifndef		TECHNOCORE_CONFIGURATION
#  define		TECHNOCORE_CONFIGURATION			"./configuration.dab"
# endif
# ifndef		TECHNOCORE_DICTIONNARY
#  define		TECHNOCORE_DICTIONNARY				"./dictionnary.dab"
# endif

#endif

void			clear_technocore_activity(t_technocore_activity	*act);

// Boucle d'exercices et outils de boucle d'exercices
t_technocore_result	start_activity(const char			*argv,
				       t_bunny_configuration		*cnf);
t_technocore_result	start_function_activity(const char		*argv0,
						t_bunny_configuration	*gen_cnf,
						t_bunny_configuration	*act_cnf,
						t_technocore_activity	*act);
t_technocore_result	start_program_activity(const char		*argv0,
					       t_bunny_configuration	*gen_cnf,
					       t_bunny_configuration	*act_cnf,
					       t_technocore_activity	*act);
t_technocore_result	start_builtin_activity(const char		*argv0,
					       t_bunny_configuration	*gen_cnf,
					       t_bunny_configuration	*act_cnf,
					       t_technocore_activity	*act);
bool			build_report(t_technocore_activity		*tech);

typedef struct		s_function
{
  void			*handler;
  void			*function;
}			t_function;

t_function		*get_function(const char			*argv0,
				      t_technocore_activity		*act,
				      t_bunny_configuration		*general_conf,
				      t_bunny_configuration		*exercise_conf);
void			delete_function(t_function			*func);
t_function		*get_evaluator(const char			*argv0,
				       t_bunny_configuration		*general_conf,
				       t_bunny_configuration		*exercise_conf);

// Divers
void			usage(char					*argv0);
void			sighandler(int					sig);

bool			dict_open(void);
bool			dict_set_language(const char			*str);
const char		*dict_get_pattern(const char			*str);
extern
t_bunny_configuration	*gl_dictionnary;


// Sous fonctions de l'API
void			get_evaluation_axis(t_bunny_configuration	*global_cnf,
					    t_bunny_configuration	*local_cnf,
					    t_function_evaluation	*eval);

void			get_user_functions(void				*handler,
					   t_function_evaluation	*eval_cnf,
					   t_function_evaluation	*eval,
					   t_bunny_configuration	*local_cnf,
					   t_user_functions		*ufunc);

int			tcsystem(const char				*cmd,
				 ...);
int			tcpopen(const char				*module_name,
				const char				*cmd,
				char					*out,
				int					*max,
				char					*message,
				size_t					msg_size);

bool			retrieve_all_files(char				*path,
					   size_t			maxpath,
					   char				**files,
					   size_t			*browse,
					   size_t			cells,
					   size_t			cellsize,
					   const char			*ext);

bool			add_all_medals(t_technocore_activity		*act,
				       t_bunny_configuration		*exe);

typedef t_technocore_result t_builtin_module(const char			*argv,
					     t_bunny_configuration	*general_cnf,
					     t_bunny_configuration	*local_cnf,
					     t_technocore_activity	*act);

t_builtin_module	evaluate_cleanliness;
t_builtin_module	evaluate_construction;
t_builtin_module	evaluate_mcq;
t_builtin_module	evaluate_style;
t_builtin_module	execute_command;
t_builtin_module	execute_move;
t_builtin_module	evaluate_cheat;
t_builtin_module	evaluate_file_c_norm;

// Sous fonction de evaluate_construction
t_builtin_module	evaluate_make_build;
t_builtin_module	evaluate_object_build;
t_builtin_module	evaluate_full_build;

#endif	/*		__TECHNOCORE_H__				*/

