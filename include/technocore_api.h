/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#ifndef				__TECHNOCORE_API_H__
# define			__TECHNOCORE_API_H__
# include			<lapin.h>
# include			<stdint.h>

# define			NOINLINE				__attribute__ ((noinline))

// Le type de la fonction de test élève
typedef int			(*t_test_func)(void);

// Un système d'écriture lorsque stdout et stderr sont fermé.
typedef struct			s_message
{
  char				message[1024 * 8];
  size_t			length;
}				t_message;

bool				add_message(t_message			*msg,
					    const char			*str,
					    ...) _BFMT(2, 3);
# define			purge_message(msg)			(msg)->length = 0
# define			message_len(msg)			((msg)->length)
# define			get_message(msg)			((msg)->message)

// Les entrées et sorties du programme
typedef struct			s_technocore
{
  // Le pipe qui remplace l'entrée standard
  int				stdin_pipe[2];
  int				stdout_pipe[2];
  int				stderr_pipe[2];

  int				stdin;
  int				stdout;
  int				stderr;

  t_message			error_buffer;
}				t_technocore;
extern t_technocore		gl_technocore;

// Les conclusions possibles d'une evaluation
typedef enum			e_technocore_result
  {
   TC_CRITICAL,			// On arrete tout. Le système de correction a planté.
   TC_FAILURE,			// On arrete l'évaluation de l'élève.
   TC_SUCCESS			// On continu.
  }				t_technocore_result;

// Une instance de technocore_activity existe par élève
typedef struct			s_technocore_activity
{
  // Le rapport d'evaluation. Le général
  t_bunny_configuration		*report;
  // Celui de l'exercice en particulier
  t_bunny_configuration		*current_report;

  // Les éléments à rendre à l'élève, dont le compte rendu.
  char				*artefacts[64];
  size_t			nbr_artefacts;

  char				pathstack[8][128];
  int				nbr_path;
}				t_technocore_activity;

// Le type de fonction recherché dans les modules de correction
typedef t_technocore_result	(*t_technocore_main)(void			*user_handler,
						     t_bunny_configuration	*general_conf,
						     t_bunny_configuration	*exe_conf,
						     t_technocore_activity	*act);
typedef t_technocore_result	t_technocore_mainf(void				*user_handler,
						   t_bunny_configuration	*general_conf,
						   t_bunny_configuration	*exe_conf,
						   t_technocore_activity	*act);

/*
** L'énumération trigger permet d'indiquer le comportement de la fonction testé par les élèves
** lorsqu'ils doivent vérifier qu'une fonction du technocentre fonctionne.
** TCFUNC_WORKING indique que la fonction du technocentre doit fonctionner.
** Une valeur positive indique un comportement défectueux défini au cas par cas.
** La valeur negative TCFUNC_CHEAT_TEST est utilisé lorsque la fonction de l'élève est testé, avant
** que sa fonction de test ne le soit. Elle sert à vérifier que l'élève n'utilise pas la fonction
** du technocentre dans son programme!
** La fonction du technocentre, si elle est appellée alors que le tirgger vaut TCFUNC_CHEAT_TEST
** passe le trigger en TCFUNC_CHEATER et colle une fessée à l'élève sous la forme d'un TC_FAILURE.
*/
typedef enum			e_trigger
  {
   TCFUNC_CHEATER		= -2,
   TCFUNC_CHEAT_TEST		= -1,
   TCFUNC_WORKING		= 0,
   TCFUNC_DEFENSIVE		= 1,
   TCFUNC_BAD_ERRNO		= 2,
   TCFUNC_NO_ERROR_HANDLING	= 3,
   TCFUNC_NO_CORNER_CASE	= 4,
   TCFUNC_BROKEN		= 5,
   TCFUNC_LAST_CASE		= TCFUNC_BROKEN
  }				t_trigger;

typedef struct			s_function_evaluation
{
  // A propos de la fonction elle meme
  bool				func;
  // La fonction élève marche globalement, hors cas particulier
  bool				mostly_working;
  // La fonction élève marche dans l'ensemble de définition de la fonction
  bool				fully_working;
  // La fonction ne plante pas, même si les paramètres sont mauvais
  bool				defensive_programming;
  // Gestion des erreurs par valeur de retour
  bool				signal_error;
  // Gestion des types d'erreur via errno
  bool				use_errno;
  // Ratio de performence vs le technocentre
  double			perf_ratio;
  // Consommation mémoire vs le technocentre
  double			ram_ratio;

  // A propos de la fonction de test
  bool				test_func;
  // La fonction de test marche globalement
  bool				mostly_testing;
  // La fonction de test verifie absolument tout l'ensemble de definition
  bool				fully_testing;
  // La fonction de test verifie si la fonction est ecrite en programmation defensive
  bool				defensive_testing;
  // La fonction de test verifie si les cas d'erreur sont signalés
  bool				error_testing;
  // La fonction verifie aussi que errno renvoit la bonne valeur
  bool				errno_testing;

  // Element d'evaluation seul:
  bool				cheater;
  bool				missing_main_function;
  bool				missing_test_function;
}				t_function_evaluation;

typedef struct			s_user_functions
{
  const char			*func_name;
  const char			*test_name;
  void				*func;
  void				*test;
}				t_user_functions;

typedef struct			s_func_eval_mod
{
  t_function_evaluation		criterias;
  t_function_evaluation		result;
  t_user_functions		user_functions;
  t_message			module_messages;
}				t_func_eval_mod;

void				get_func_eval_mod(t_bunny_configuration		*global,
						  t_bunny_configuration		*local,
						  t_func_eval_mod		*fem,
						  void				*handler);


// Utilisé par les plugins.
t_technocore_result		build_function_report(t_technocore_activity	*act,
						      t_bunny_configuration	*exe,
						      t_func_eval_mod		*fem);

bool				add_artefact(t_technocore_activity		*act,
					     const char				*str);
bool				set_exercise_grade(t_technocore_activity	*act,
						   double			grade);
bool				add_exercise_medal(t_technocore_activity	*act,
						   const char			*medal);
bool				add_exercise_function(t_technocore_activity	*act,
						      t_bunny_configuration	*cnf,
						      const char		*func);
bool				add_exercise_all_medals(t_technocore_activity	*act,
							t_bunny_configuration	*exe,
							const char		*field);
bool				add_exercise_message(t_technocore_activity	*act,
						     const char			*msg,
						     ...) _BFMT(2, 3);
void				rseed(void);

size_t				get_allocated_byte_count(void);

bool				add_to_current_report(t_technocore_activity	*act,
						      const char		*value,
						      const char		*pattern,
						      ...) _BFMT(3, 4);

bool				file_exists(const char				*str);

t_technocore_result		do_string_diff(t_technocore_activity		*act,
					       const char			*name,
					       const char			*user,
					       const char			*ref,
					       int				maxlen);
t_technocore_result		do_mem_diff(t_technocore_activity		*act,
					    const char				*name,
					    const char				*user,
					    const char				*ref,
					    int					len);

typedef struct			s_graphic_diff_configuration
{
  int				component_tolerance;
  int				neighbor_radius;
  bool				ignore_zero_reference;
  bool				use_alpha_traps;
} 				t_graphic_diff_configuration;

typedef struct			s_graphic_diff
{
  unsigned int			width;
  unsigned int			height;
  size_t			compared_pixels;
  size_t			matched_pixels;
  size_t			mismatched_pixels;
  size_t			trap_pixels;
} 				t_graphic_diff;

void				init_graphic_diff_configuration(t_graphic_diff_configuration*cnf);
bool				tc_graphic_compare_buffers(const unsigned int	*user,
							   const unsigned int	*ref,
							   unsigned int		width,
							   unsigned int		height,
							   const t_graphic_diff_configuration*cnf,
							   t_graphic_diff	*diff,
							   unsigned int		*diff_pixels);
bool				tc_graphic_compare_pixelarray(const t_bunny_pixelarray *user,
							      const t_bunny_pixelarray *ref,
							      const t_graphic_diff_configuration *cnf,
							      t_graphic_diff	*diff,
							      unsigned int	*diff_pixels);
bool				tc_graphic_save_bmp24(const char		*file,
						      const unsigned int	*pixels,
						      unsigned int		width,
						      unsigned int		height);
bool				tc_graphic_save_pixelarray_bmp24(const char				*file,
								 const t_bunny_pixelarray		*pix);
t_technocore_result		do_graphic_diff(t_technocore_activity		*act,
						const char			*name,
						const t_bunny_pixelarray	*user,
						const t_bunny_pixelarray	*ref,
						const t_graphic_diff_configuration *cnf);

void				evaluate_test_efficiency(t_trigger		*trigger,
							 t_func_eval_mod	*fem,
							 t_test_func		test,
							 int			maxbkcase);


typedef struct			s_audio_diff_configuration
{
  int				amplitude_tolerance;
  int				neighbor_radius;
  unsigned int			sample_rate;
  unsigned int			channels;
} 				t_audio_diff_configuration;

typedef struct			s_audio_diff
{
  unsigned int			sample_rate;
  unsigned int			channels;
  size_t			sample_count;
  size_t			checked_samples;
  size_t			matched_samples;
  size_t			mismatched_samples;
  int				max_delta;
  double			mean_delta;
} 				t_audio_diff;

void				init_audio_diff_configuration
					(t_audio_diff_configuration	*cnf);
bool				tc_audio_compare_pcm16
					(const int16_t			*user,
					 size_t				user_sample_count,
					 const int16_t			*ref,
					 size_t				ref_sample_count,
					 const t_audio_diff_configuration	*cnf,
					 t_audio_diff			*diff,
					 int16_t			*diff_samples);
bool				tc_audio_save_wav_pcm16
					(const char				*file,
					 const int16_t			*samples,
					 size_t				sample_count,
					 unsigned int			sample_rate,
					 unsigned int			channels);
t_technocore_result		do_audio_diff(t_technocore_activity	*act,
					      const char		*name,
					      const int16_t		*user,
					      size_t			user_sample_count,
					      const int16_t		*ref,
					      size_t			ref_sample_count,
					      const t_audio_diff_configuration *cnf);


typedef struct			s_trace_diff_configuration
{
  bool				ignore_carriage_return;
} 				t_trace_diff_configuration;

typedef struct			s_trace_diff
{
  size_t			user_length;
  size_t			ref_length;
  size_t			first_mismatch;
} 				t_trace_diff;

void				init_trace_diff_configuration
					(t_trace_diff_configuration	*cnf);
bool				tc_trace_compare_text
					(const char			*user,
					 const char			*ref,
					 const t_trace_diff_configuration *cnf,
					 t_trace_diff			*diff);
bool				tc_trace_save_text
					(const char			*file,
					 const char			*trace);
t_technocore_result		do_trace_diff(t_technocore_activity	*act,
					      const char		*type,
					      const char		*name,
					      const char		*user,
					      const char		*ref,
					      const t_trace_diff_configuration *cnf);
t_technocore_result		do_vm110n_trace_diff
					(t_technocore_activity	*act,
					 const char			*name,
					 const char			*user,
					 const char			*ref,
					 const t_trace_diff_configuration *cnf);


bool				dict_open(void);
bool				dict_set_language(const char			*str);
const char			*dict_get_pattern(const char			*str);
t_technocore_result		no_output(t_technocore_activity			*act,
					  int					fd,
					  const char				*out,
					  const char				*fil);
t_technocore_result		module_fail(const char				*lab,
					    const char				*fil);

typedef void			(*t_perf_test)(void				*f,
					       void				*b);
void				evaluate_performances(t_func_eval_mod		*fem,
						      t_perf_test		pcall,
						      void			*tcf,
						      void			*fun,
						      void			*data,
						      size_t			dsiz,
						      size_t			nmb);

# define			CAT(a, b)					\
  a ## b
# define			TCRFUNC(name)					\
  CAT(_technocore_, name)
# define			TCFUNC(name)					\
  CAT(technocore_, name)
# define			EVFUNC(name)					\
  CAT(evaluate_, name)

void				tcdebug(const char				*pat,
					...);

#endif	/*			__TECHNOCORE_API_H__				*/
