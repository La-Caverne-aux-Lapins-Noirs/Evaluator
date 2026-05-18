/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
**
** TechnoCore
*/

#include			<ctype.h>
#include			<limits.h>
#include			<stdint.h>
#include			<stdio.h>
#include			<stdlib.h>
#include			<string.h>
#include			"technocore.h"

void				init_audio_diff_configuration
(t_audio_diff_configuration	*cnf)
{
  if (cnf == NULL)
    return ;
  cnf->amplitude_tolerance = 512;
  cnf->neighbor_radius = 0;
  cnf->sample_rate = 44100;
  cnf->channels = 1;
}

static const t_audio_diff_configuration	*get_cnf
(const t_audio_diff_configuration *cnf,
 t_audio_diff_configuration	*local)
{
  if (cnf != NULL)
    return (cnf);
  init_audio_diff_configuration(local);
  return (local);
}

static size_t			max_size
(size_t				a,
 size_t				b)
{
  return (a > b ? a : b);
}

static int			abs_delta
(int16_t			a,
 int16_t			b)
{
  return (abs((int)a - (int)b));
}

static int16_t			clamp_pcm16
(int				value)
{
  if (value > INT16_MAX)
    return (INT16_MAX);
  if (value < INT16_MIN)
    return (INT16_MIN);
  return ((int16_t)value);
}

static int16_t			diff_sample
(int16_t			user,
 int16_t			ref)
{
  int				diff;

  diff = ((int)user - (int)ref) * 8;
  return (clamp_pcm16(diff));
}

static bool			valid_pcm_arguments
(const int16_t			*user,
 size_t				user_sample_count,
 const int16_t			*ref,
 size_t				ref_sample_count,
 const t_audio_diff_configuration *cnf)
{
  if ((user == NULL && user_sample_count != 0) ||
      (ref == NULL && ref_sample_count != 0))
    return (false);
  if (cnf->channels == 0 || cnf->sample_rate == 0)
    return (false);
  if (user_sample_count % cnf->channels != 0 ||
      ref_sample_count % cnf->channels != 0)
    return (false);
  return (true);
}

static int			best_sample_delta
(const int16_t			*user,
 size_t				user_sample_count,
 const int16_t			*ref,
 size_t				ref_sample_count,
 size_t				index,
 const t_audio_diff_configuration *cnf)
{
  size_t			channel;
  size_t			frame;
  size_t			frames;
  int				radius;
  int				best;

  if (index >= ref_sample_count)
    return (INT_MAX);
  radius = cnf->neighbor_radius;
  if (radius < 0)
    radius = 0;
  channel = index % cnf->channels;
  frame = index / cnf->channels;
  frames = user_sample_count / cnf->channels;
  best = INT_MAX;
  for (int offset = -radius; offset <= radius; ++offset)
    {
      int			nframe;
      size_t			nindex;
      int			delta;

      nframe = (int)frame + offset;
      if (nframe < 0 || (size_t)nframe >= frames)
	continue ;
      nindex = (size_t)nframe * cnf->channels + channel;
      delta = abs_delta(user[nindex], ref[index]);
      if (delta < best)
	best = delta;
    }
  return (best);
}

static void			clear_audio_diff
(t_audio_diff			*diff,
 const t_audio_diff_configuration *cnf,
 size_t				sample_count)
{
  memset(diff, 0, sizeof(*diff));
  diff->sample_rate = cnf->sample_rate;
  diff->channels = cnf->channels;
  diff->sample_count = sample_count;
}

bool				tc_audio_compare_pcm16
(const int16_t			*user,
 size_t				user_sample_count,
 const int16_t			*ref,
 size_t				ref_sample_count,
 const t_audio_diff_configuration *config,
 t_audio_diff			*diff,
 int16_t			*diff_samples)
{
  t_audio_diff_configuration	local;
  const t_audio_diff_configuration *cnf;
  t_audio_diff			local_diff;
  size_t			common;
  size_t			max;
  double			sum_delta;

  cnf = get_cnf(config, &local);
  if (valid_pcm_arguments(user, user_sample_count, ref, ref_sample_count, cnf) == false)
    return (false);
  if (diff == NULL)
    diff = &local_diff;
  max = max_size(user_sample_count, ref_sample_count);
  common = user_sample_count < ref_sample_count ? user_sample_count : ref_sample_count;
  clear_audio_diff(diff, cnf, max);
  sum_delta = 0;
  if (diff_samples != NULL)
    for (size_t i = 0; i < max; ++i)
      {
	int16_t		us = i < user_sample_count ? user[i] : 0;
	int16_t		rs = i < ref_sample_count ? ref[i] : 0;

	diff_samples[i] = diff_sample(us, rs);
      }
  for (size_t i = 0; i < common; ++i)
    {
      int		delta;

      delta = best_sample_delta(user, user_sample_count, ref, ref_sample_count, i, cnf);
      diff->checked_samples += 1;
      if (delta <= cnf->amplitude_tolerance)
	diff->matched_samples += 1;
      else
	diff->mismatched_samples += 1;
      if (delta != INT_MAX)
	{
	  if (delta > diff->max_delta)
	    diff->max_delta = delta;
	  sum_delta += delta;
	}
    }
  if (max > common)
    diff->mismatched_samples += max - common;
  if (diff->checked_samples != 0)
    diff->mean_delta = sum_delta / diff->checked_samples;
  return (diff->mismatched_samples == 0 && user_sample_count == ref_sample_count);
}

static bool			write_wav_header
(FILE				*fp,
 size_t				sample_count,
 unsigned int			sample_rate,
 unsigned int			channels)
{
  unsigned char			header[44];
  uint64_t			data_bytes;
  uint64_t			byte_rate;
  uint64_t			block_align;

  data_bytes = sample_count * sizeof(int16_t);
  block_align = channels * sizeof(int16_t);
  byte_rate = (uint64_t)sample_rate * block_align;
  if (data_bytes > UINT32_MAX || data_bytes + 36 > UINT32_MAX ||
      block_align > UINT16_MAX || byte_rate > UINT32_MAX ||
      channels > UINT16_MAX)
    return (false);
  memset(header, 0, sizeof(header));
  memcpy(&header[0], "RIFF", 4);
  set_u32le(&header[4], (uint32_t)(36 + data_bytes));
  memcpy(&header[8], "WAVE", 4);
  memcpy(&header[12], "fmt ", 4);
  set_u32le(&header[16], 16);
  set_u16le(&header[20], 1);
  set_u16le(&header[22], channels);
  set_u32le(&header[24], sample_rate);
  set_u32le(&header[28], (uint32_t)byte_rate);
  set_u16le(&header[32], (uint16_t)block_align);
  set_u16le(&header[34], 16);
  memcpy(&header[36], "data", 4);
  set_u32le(&header[40], (uint32_t)data_bytes);
  return (fwrite(header, sizeof(header), 1, fp) == 1);
}

bool				tc_audio_save_wav_pcm16
(const char			*file,
 const int16_t			*samples,
 size_t				sample_count,
 unsigned int			sample_rate,
 unsigned int			channels)
{
  FILE				*fp;

  if (file == NULL || (samples == NULL && sample_count != 0) ||
      sample_rate == 0 || channels == 0 || sample_count % channels != 0)
    return (false);
  if ((fp = fopen(file, "wb")) == NULL)
    return (false);
  if (write_wav_header(fp, sample_count, sample_rate, channels) == false)
    goto Fail;
  for (size_t i = 0; i < sample_count; ++i)
    {
      unsigned char	buf[2];

      set_u16le(&buf[0], (uint16_t)samples[i]);
      if (fwrite(buf, sizeof(buf), 1, fp) != 1)
	goto Fail;
    }
  return (fclose(fp) == 0);

 Fail:
  fclose(fp);
  return (false);
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
    name = "audio";
  for (j = 0; name[j] && j + 1 < outlen; ++j)
    if (isalnum((unsigned char)name[j]) || name[j] == '_' || name[j] == '-')
      out[j] = name[j];
    else
      out[j] = '_';
  out[j] = '\0';
  if (j == 0)
    snprintf(out, outlen, "audio");
}

static bool			make_artefact_name
(char				*out,
 size_t				outlen,
 const char			*name,
 size_t				index,
 const char			*suffix)
{
  char				safe[80];
  int				ret;

  sanitize_name(&safe[0], sizeof(safe), name);
  ret = snprintf(out, outlen, "audio_%02zu_%s_%s.wav", index, safe, suffix);
  return (ret > 0 && (size_t)ret < outlen);
}

static t_technocore_result	add_audio_artefact
(t_technocore_activity		*act,
 const char			*file)
{
  if (add_artefact(act, file) == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot add audio artefact %s to report.\n", file);
      return (TC_CRITICAL);
    }
  return (TC_SUCCESS);
}

static t_technocore_result	write_audio_artefacts
(t_technocore_activity		*act,
 const char			*name,
 const int16_t			*user,
 size_t				user_sample_count,
 const int16_t			*ref,
 size_t				ref_sample_count,
 const int16_t			*diff_samples,
 size_t				diff_sample_count,
 const t_audio_diff_configuration *cnf)
{
  char				user_file[160];
  char				ref_file[160];
  char				diff_file[160];
  size_t			index;

  index = act->nbr_artefacts;
  if (make_artefact_name(ref_file, sizeof(ref_file), name, index, "ref") == false ||
      make_artefact_name(user_file, sizeof(user_file), name, index, "user") == false ||
      make_artefact_name(diff_file, sizeof(diff_file), name, index, "diff") == false)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot build audio artefact names for %s.\n", name ? name : "audio");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (tc_audio_save_wav_pcm16(ref_file, ref, ref_sample_count,
			      cnf->sample_rate, cnf->channels) == false ||
      tc_audio_save_wav_pcm16(user_file, user, user_sample_count,
			      cnf->sample_rate, cnf->channels) == false ||
      tc_audio_save_wav_pcm16(diff_file, diff_samples, diff_sample_count,
			      cnf->sample_rate, cnf->channels) == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot write audio artefacts for %s.\n", name ? name : "audio");
      return (TC_CRITICAL);
    }
  if (add_audio_artefact(act, ref_file) != TC_SUCCESS ||
      add_audio_artefact(act, user_file) != TC_SUCCESS ||
      add_audio_artefact(act, diff_file) != TC_SUCCESS)
    return (TC_CRITICAL);
  return (TC_SUCCESS);
}

t_technocore_result		do_audio_diff
(t_technocore_activity		*act,
 const char			*name,
 const int16_t			*user,
 size_t				user_sample_count,
 const int16_t			*ref,
 size_t				ref_sample_count,
 const t_audio_diff_configuration *config)
{
  t_audio_diff_configuration	local;
  const t_audio_diff_configuration *cnf;
  t_audio_diff			diff;
  int16_t			*diff_samples;
  size_t			diff_sample_count;
  bool				ok;

  cnf = get_cnf(config, &local);
  if (valid_pcm_arguments(user, user_sample_count, ref, ref_sample_count, cnf) == false)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot compare invalid audio buffers for %s.\n", name ? name : "audio");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  diff_sample_count = max_size(user_sample_count, ref_sample_count);
  if ((diff_samples = calloc(diff_sample_count == 0 ? 1 : diff_sample_count,
			     sizeof(*diff_samples))) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot allocate audio diff for %s.\n", name ? name : "audio");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  ok = tc_audio_compare_pcm16(user, user_sample_count, ref, ref_sample_count,
			      cnf, &diff, diff_samples);
  if (ok)
    {
      free(diff_samples);
      return (TC_SUCCESS);
    }
  if (write_audio_artefacts(act, name, user, user_sample_count, ref, ref_sample_count,
			    diff_samples, diff_sample_count, cnf) != TC_SUCCESS)
    {
      free(diff_samples);
      return (TC_CRITICAL);
    }
  free(diff_samples);
  if (user_sample_count != ref_sample_count)
    {
      if (add_exercise_message
	  (act, dict_get_pattern("AudioSampleCountsAreDifferent"),
	   name ? name : "audio", user_sample_count, ref_sample_count) == false)
	return (TC_CRITICAL);
      return (TC_FAILURE);
    }
  if (add_exercise_message
      (act, dict_get_pattern("SoundsAreDifferent"),
       name ? name : "audio",
       diff.mismatched_samples,
       diff.max_delta,
       diff.mean_delta,
       diff.checked_samples) == false)
    return (TC_CRITICAL);
  return (TC_FAILURE);
}
