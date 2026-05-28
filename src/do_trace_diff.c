/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
**
** TechnoCore
*/

#include			<ctype.h>
#include			<stdio.h>
#include			<stdlib.h>
#include			<string.h>
#include			"technocore.h"

void				init_trace_diff_configuration
(t_trace_diff_configuration	*cnf)
{
  if (cnf == NULL)
    return ;
  cnf->ignore_carriage_return = true;
}

static const t_trace_diff_configuration *get_cnf
(const t_trace_diff_configuration *cnf,
 t_trace_diff_configuration	*local)
{
  if (cnf != NULL)
    return (cnf);
  init_trace_diff_configuration(local);
  return (local);
}

static bool			same_char
(char				user,
 char				ref,
 const t_trace_diff_configuration *cnf)
{
  if (cnf->ignore_carriage_return && user == '\r' && ref == '\n')
    return (true);
  if (cnf->ignore_carriage_return && user == '\n' && ref == '\r')
    return (true);
  return (user == ref);
}

bool				tc_trace_compare_text
(const char			*user,
 const char			*ref,
 const t_trace_diff_configuration *config,
 t_trace_diff			*diff)
{
  t_trace_diff_configuration	local;
  const t_trace_diff_configuration *cnf;
  t_trace_diff			local_diff;
  size_t			i;

  cnf = get_cnf(config, &local);
  if (user == NULL || ref == NULL)
    return (false);
  if (diff == NULL)
    diff = &local_diff;
  diff->user_length = strlen(user);
  diff->ref_length = strlen(ref);
  diff->first_mismatch = 0;
  for (i = 0; user[i] != '\0' && ref[i] != '\0'; ++i)
    if (same_char(user[i], ref[i], cnf) == false)
      {
	diff->first_mismatch = i;
	return (false);
      }
  diff->first_mismatch = i;
  return (user[i] == ref[i]);
}

bool				tc_trace_save_text
(const char			*file,
 const char			*trace)
{
  FILE				*fp;

  if (file == NULL || trace == NULL)
    return (false);
  if ((fp = fopen(file, "wb")) == NULL)
    return (false);
  if (fwrite(trace, strlen(trace), 1, fp) != 1 && trace[0] != '\0')
    {
      fclose(fp);
      return (false);
    }
  return (fclose(fp) == 0);
}

static void			sanitize_name
(char				*out,
 size_t				outlen,
 const char			*name)
{
  size_t			j;

  if (outlen == 0)
    return ;
  if (name == NULL || name[0] == '\0')
    name = "trace";
  for (j = 0; name[j] && j + 1 < outlen; ++j)
    if (isalnum((unsigned char)name[j]) || name[j] == '_' || name[j] == '-')
      out[j] = name[j];
    else
      out[j] = '_';
  out[j] = '\0';
  if (j == 0)
    snprintf(out, outlen, "trace");
}

static bool			make_artefact_name
(char				*out,
 size_t				outlen,
 const char			*type,
 const char			*name,
 size_t				index,
 const char			*suffix)
{
  char				safe_type[48];
  char				safe_name[80];
  int				ret;

  sanitize_name(&safe_type[0], sizeof(safe_type), type ? type : "trace");
  sanitize_name(&safe_name[0], sizeof(safe_name), name ? name : "trace");
  ret = snprintf(out, outlen, "trace_%02zu_%s_%s_%s.txt",
		 index, safe_type, safe_name, suffix);
  return (ret > 0 && (size_t)ret < outlen);
}

static t_technocore_result	add_trace_artefact
(t_technocore_activity		*act,
 const char			*file)
{
  if (add_artefact(act, file) == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot add trace artefact %s to report.\n", file);
      return (TC_CRITICAL);
    }
  return (TC_SUCCESS);
}

static t_technocore_result	write_trace_artefacts
(t_technocore_activity		*act,
 const char			*type,
 const char			*name,
 const char			*user,
 const char			*ref)
{
  char				user_file[160];
  char				ref_file[160];
  size_t			index;

  index = act->nbr_artefacts;
  if (make_artefact_name(ref_file, sizeof(ref_file), type, name, index, "ref") == false ||
      make_artefact_name(user_file, sizeof(user_file), type, name, index, "user") == false)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot build trace artefact names for %s.\n", name ? name : "trace");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (tc_trace_save_text(ref_file, ref) == false ||
      tc_trace_save_text(user_file, user) == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot write trace artefacts for %s.\n", name ? name : "trace");
      return (TC_CRITICAL);
    }
  if (add_trace_artefact(act, ref_file) != TC_SUCCESS ||
      add_trace_artefact(act, user_file) != TC_SUCCESS)
    return (TC_CRITICAL);
  return (TC_SUCCESS);
}

t_technocore_result		do_trace_diff
(t_technocore_activity		*act,
 const char			*type,
 const char			*name,
 const char			*user,
 const char			*ref,
 const t_trace_diff_configuration *config)
{
  t_trace_diff_configuration	local;
  const t_trace_diff_configuration *cnf;
  t_trace_diff			diff;

  cnf = get_cnf(config, &local);
  if (user == NULL || ref == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot compare null traces for %s.\n", name ? name : "trace");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (tc_trace_compare_text(user, ref, cnf, &diff))
    return (TC_SUCCESS);
  if (write_trace_artefacts(act, type, name, user, ref) != TC_SUCCESS)
    return (TC_CRITICAL);
  if (add_exercise_message
      (act, dict_get_pattern("TracesAreDifferent"),
       type ? type : "trace",
       name ? name : "trace",
       diff.first_mismatch,
       diff.user_length,
       diff.ref_length) == false)
    return (TC_CRITICAL);
  return (TC_FAILURE);
}

t_technocore_result		do_vm110n_trace_diff
(t_technocore_activity		*act,
 const char			*name,
 const char			*user,
 const char			*ref,
 const t_trace_diff_configuration *cnf)
{
  return (do_trace_diff(act, "VM110N", name, user, ref, cnf));
}
